#include "neural_network.h"
#include "weights_initialization/weights_initializer.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>

// START NEURON

Neuron::Neuron(std::size_t inputSize, std::unique_ptr<WeightsInitializer> wInit,
               std::size_t randomSeed) {
  auto [mean, stddev] = wInit->getDistParams(inputSize);

  std::mt19937 rng(randomSeed + Neuron::n);
  std::normal_distribution<float> dist(mean, stddev);

  Neuron::n++;

  this->inputSize = inputSize;
  this->weights = std::vector<float>(inputSize + 1);

  for (float &w : this->weights) {
    w = dist(rng);
  }
}

Neuron::Neuron(const Neuron &neuron) {
  this->inputSize = neuron.inputSize;
  this->weights = neuron.weights;
}

float Neuron::feedForward(const std::vector<float> &input) {
  if (input.size() != this->weights.size() - 1) {
    throw std::invalid_argument(
        "feedForward: expected " + std::to_string(this->weights.size() - 1) +
        " inputs, but got " + std::to_string(input.size()));
  }

  // std::cout // << "Output = w[0] = " // << this->weights[0] // << std::endl;
  float sum = this->weights[0];

  for (std::size_t i = 0; i < input.size(); ++i) {
    // std::cout // << "Output += input[" // << i // << "] * w[" // << i + 1
    // << "] = " // << input[i] // << " * " // << this->weights[i + 1] // << " =
    // "
    // << input[i] * this->weights[i + 1] // << std::endl;
    sum += input[i] * this->weights[i + 1];
  }

  // std::cout // << "Output: " // << sum // << std::endl;
  return sum;
}

std::vector<float> &Neuron::getWeights() { return this->weights; }

float Neuron::getDelta() { return this->delta; }

void Neuron::setDelta(float delta) { this->delta = delta; }

float Neuron::getOutput() { return this->output; }

void Neuron::setOutput(float output) { this->output = output; }

// END NEURON

// START LAYER

Layer::Layer(std::size_t nInputs, std::size_t nOutputs,
             std::unique_ptr<ActivationFunction> activationFunction,
             std::unique_ptr<WeightsInitializer> wInit) {
  this->inputSize = nInputs;
  this->outputSize = nOutputs;

  this->activationFunction = move(activationFunction);
  this->neurons.reserve(nOutputs);

  for (std::size_t i = 0; i < nOutputs; ++i) {
    this->neurons.emplace_back(Neuron(nInputs, wInit->clone()));
  }
}

Layer::Layer(const Layer &layer) {
  this->inputSize = layer.inputSize;
  this->outputSize = layer.outputSize;

  this->neurons = layer.neurons;
  this->activationFunction = layer.activationFunction->clone();
}

std::vector<float> Layer::feedForward(const std::vector<float> &input) {
  std::vector<float> output;
  output.reserve(neurons.size());

  for (Neuron &n : neurons) {
    output.emplace_back(n.feedForward(input));
  }

  activationFunction->activate(output);

  for (std::size_t i = 0; i < neurons.size(); ++i) {
    neurons[i].setOutput(output[i]);
  }

  return output;
}

std::size_t Layer::getInputSize() { return this->inputSize; }

std::size_t Layer::getOutputSize() { return this->outputSize; }

std::vector<Neuron> &Layer::getNeurons() { return this->neurons; }

ActivationFunction &Layer::getActivationFunction() {
  return *this->activationFunction;
}

// END LAYER

// START NEURAL NETWORK

NeuralNetwork::NeuralNetwork(const std::vector<Layer> &layers) {
  if (layers.empty()) {
    throw std::invalid_argument("The Network must have at least one layer");
  }

  this->layers = std::vector<Layer>(layers);

  for (std::size_t i = 1; i < layers.size(); ++i) {
    std::size_t prevOut = this->layers[i - 1].getOutputSize();
    std::size_t nextIn = this->layers[i].getInputSize();

    if (prevOut != nextIn) {
      throw std::invalid_argument(
          "Layer " + std::to_string(i) + " expects " + std::to_string(nextIn) +
          " inputs, but previous layer outputs " + std::to_string(prevOut));
    }
  }
}

std::vector<float> NeuralNetwork::feedForward(const std::vector<float> &input) {
  std::vector<float> output = input;

  for (std::size_t i = 0; i < this->layers.size(); ++i) {
    // std::cout // << "Layer " // << i // << std::endl;
    output = this->layers[i].feedForward(output);
  }

  return output;
}

float computeLoss(const std::vector<float> &output,
                  const std::vector<float> &target) {
  float loss = 0.0;
  std::size_t n = output.size();

  for (std::size_t i = 0; i < n; ++i) {
    float diff = output[i] - target[i];
    loss += diff * diff;
  }

  return loss / static_cast<float>(n);
}

std::vector<float> computeLossDerivative(const std::vector<float> &output,
                                         const std::vector<float> &target) {
  std::size_t n = output.size();
  std::vector<float> diff(n);

  for (std::size_t i = 0; i < n; ++i) {
    diff[i] = 2.0 * (output[i] - target[i]) / static_cast<float>(n);
  }

  return diff;
}

void NeuralNetwork::backPropagation(const std::vector<float> &expected) {
  for (int i = this->layers.size() - 1; i >= 0; --i) {
    Layer &layer = this->layers[i];
    std::vector<Neuron> &neurons = layer.getNeurons();

    std::vector<float> errors;

    // Hidden layers
    if (std::size_t(i) != this->layers.size() - 1) {
      // std::cout // << "Hidden layer " // << i // << std::endl;
      std::vector<Neuron> &nextNeurons = this->layers[i + 1].getNeurons();

      // For each neuron of the layer
      for (std::size_t j = 0; j < neurons.size(); ++j) {
        // std::cout // << "Neuron " // << j // << std::endl;
        float error = 0.0;

        for (std::size_t k = 0; k < nextNeurons.size(); ++k) {
          float neuronErr =
              nextNeurons[k].getWeights()[j + 1] * nextNeurons[k].getDelta();
          error += neuronErr;
          // std::cout // << "Error " // << neuronErr // << std::endl;
        }

        // std::cout // << "Layer error: " // << error // << std::endl;
        errors.push_back(error);
      }
      // Output layer
    } else {
      // std::cout // << "Output layer " // << i // << std::endl;

      for (std::size_t j = 0; j < neurons.size(); ++j) {
        // std::cout // << "Neuron " // << j // << std::endl;
        float output = neurons[j].getOutput();
        // std::cout // << "Output: " // << output // << std::endl;
        // std::cout // << "Expected: " // << expected[j] // << std::endl;
        // std::cout // << "Error: " // << output - expected[j] // << std::endl;
        errors.push_back(output - expected[j]);
      }
    }

    // std::cout // << "[Backpropagation::CalculatingDeltas]" // << std::endl;
    for (std::size_t j = 0; j < neurons.size(); ++j) {
      // std::cout // << "Neuron " // << j // << std::endl;
      float derivate =
          layer.getActivationFunction().derivative(neurons[j].getOutput());
      // std::cout // << "Derivate: " // << derivate // << std::endl;
      // std::cout // << "Error: " // << errors[j] // << std::endl;
      // std::cout // << "Delta: " // << errors[j] * derivate // << std::endl;
      neurons[j].setDelta(errors[j] * derivate);
    }
  }
}

void NeuralNetwork::updateWeights(const std::vector<float> &input,
                                  float learningRate) {
  std::vector<float> layerInput = input;

  for (std::size_t i = 0; i < layers.size(); ++i) {
    // std::cout // << "Layer " // << i // << std::endl;
    Layer &layer = layers[i];
    std::vector<Neuron> &neurons = layer.getNeurons();

    for (std::size_t j = 0; j < neurons.size(); ++j) {
      auto &neuron = neurons[j];
      // std::cout // << "  Neuron " // << j // << '\n';

      // std::cout // << "    Old bias: " // << neuron.getWeights()[0] // <<
      // '\n';
      neuron.getWeights()[0] -= learningRate * neuron.getDelta();
      // std::cout // << "    New bias: " // << neuron.getWeights()[0] // <<
      // '\n';

      for (std::size_t k = 0; k < layerInput.size(); ++k) {
        // std::cout // << "    Old w[" // << k + 1
        // << "]: " // << neuron.getWeights()[k + 1] // << '\n';
        neuron.getWeights()[k + 1] -=
            learningRate * neuron.getDelta() * layerInput[k];
        // std::cout // << "    New w[" // << k + 1
        // << "]: " // << neuron.getWeights()[k + 1] // << '\n';
      }
    }

    layerInput.clear();
    for (auto &neuron : neurons) {
      layerInput.push_back(neuron.getOutput());
    }
  }
}

void NeuralNetwork::train(const std::vector<std::vector<float>> &inputs,
                          const std::vector<int> &targets, int epochs,
                          float learningRate) {
  // For each epoch
  std::cout << "Starting training..." << std::endl;
  for (int epoch = 0; epoch < epochs; ++epoch) {
    std::cout << "Epoch: " << epoch << std::endl;
    // std::cout // << "__________"  << std::endl;

    // Por each sample
    for (std::size_t i = 0; i < inputs.size(); ++i) {
      // std::cout // << "Sample: " // << i // << std::endl;
      // std::cout // << "----------" // << std::endl;
      // std::cout // << "[Feeding Forward]" // << std::endl;

      std::vector<float> outputs = feedForward(inputs[i]);

      std::vector<float> expected(
          *max_element(targets.begin(), targets.end()) + 1, 0.0);
      expected[targets[i]] = 1.0;

      // std::cout // << "[Backpropagation]" // << std::endl;
      backPropagation(expected);
      // std::cout // << "[UpdateWeights]" // << std::endl;
      updateWeights(inputs[i], learningRate);
    }
  }
}

// END NEURAL NETWORK