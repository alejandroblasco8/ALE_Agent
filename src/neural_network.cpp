#include "neural_network.h"
#include "weights_initialization/weights_initializer.h"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>

std::vector<double> accuracy(const std::vector<int> &preds,
                             const std::vector<int> &targets, int nClasses);

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

  float sum = this->weights[0];

  for (std::size_t i = 0; i < input.size(); ++i) {
    sum += input[i] * this->weights[i + 1];
  }

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

void NeuralNetwork::backPropagation(int yTrue) {
  for (int i = static_cast<int>(layers.size()) - 1; i >= 0; --i) {

    Layer &layer = layers[i];
    std::vector<Neuron> &ns = layer.getNeurons();
    std::vector<float> errors;

    if (i == static_cast<int>(layers.size()) - 1) {

      for (std::size_t j = 0; j < ns.size(); ++j) {
        float p = ns[j].getOutput();
        float delta = p - (j == static_cast<std::size_t>(yTrue));
        ns[j].setDelta(delta);
      }
    } else {
      std::vector<Neuron> &next = layers[i + 1].getNeurons();

      for (std::size_t j = 0; j < ns.size(); ++j) {
        float err = 0.f;
        for (std::size_t k = 0; k < next.size(); ++k)
          err += next[k].getWeights()[j + 1] * next[k].getDelta();
        errors.push_back(err);
      }
      for (std::size_t j = 0; j < ns.size(); ++j) {
        float dAct =
            layer.getActivationFunction().derivative(ns[j].getOutput());
        ns[j].setDelta(errors[j] * dAct);
      }
    }
  }
}

void NeuralNetwork::updateWeights(const std::vector<float> &input,
                                  float learningRate) {
  std::vector<float> layerInput = input;

  for (std::size_t i = 0; i < layers.size(); ++i) {
    Layer &layer = layers[i];
    std::vector<Neuron> &neurons = layer.getNeurons();

    for (std::size_t j = 0; j < neurons.size(); ++j) {
      auto &neuron = neurons[j];

      neuron.getWeights()[0] -= learningRate * neuron.getDelta();

      for (std::size_t k = 0; k < layerInput.size(); ++k) {
        neuron.getWeights()[k + 1] -=
            learningRate * neuron.getDelta() * layerInput[k];
      }
    }

    layerInput.clear();
    for (auto &neuron : neurons) {
      layerInput.push_back(neuron.getOutput());
    }
  }
}

void NeuralNetwork::printWeights() {
  std::cout << "{\n";
  for (std::size_t l = 0; l < layers.size(); ++l) {
    auto &layer = layers[l];
    auto &neurons = layer.getNeurons();
    std::cout << "  {\n";
    for (auto &neuron : neurons) {
      auto &w = neuron.getWeights();
      std::cout << "    {";
      for (std::size_t i = 0; i < w.size(); ++i) {
        std::cout << w[i];
        if (i < w.size() - 1)
          std::cout << ", ";
      }
      std::cout << "},\n";
    }
    std::cout << "  }";
    if (l < layers.size() - 1)
      std::cout << ",";
    std::cout << "\n";
  }
  std::cout << "};\n";
}

void NeuralNetwork::train(const std::vector<std::vector<float>> &xTrain,
                          const std::vector<int> &yTrain,
                          const std::vector<std::vector<float>> &xVal,
                          const std::vector<int> &yVal, int epochs,
                          float learningRate) {
  const int nClasses = *std::max_element(yTrain.begin(), yTrain.end()) + 1;
  std::cout << std::fixed << std::setprecision(2);

  for (int epoch = 0; epoch < epochs; ++epoch) {
    auto t0 = std::chrono::steady_clock::now();

    for (std::size_t k = 0; k < xTrain.size(); ++k) {
      feedForward(xTrain[k]);
      backPropagation(yTrain[k]);
      updateWeights(xTrain[k], learningRate);
    }

    std::vector<int> trainPreds, valPreds;
    trainPreds.reserve(xTrain.size());
    valPreds.reserve(xVal.size());

    for (const auto &s : xTrain) {
      auto out = feedForward(s);
      trainPreds.push_back(static_cast<int>(std::distance(
          out.begin(), std::max_element(out.begin(), out.end()))));
    }

    for (const auto &s : xVal) {
      auto out = feedForward(s);
      valPreds.push_back(static_cast<int>(std::distance(
          out.begin(), std::max_element(out.begin(), out.end()))));
    }

    auto accTrain = accuracy(trainPreds, yTrain, nClasses);
    auto accVal = accuracy(valPreds, yVal, nClasses);

    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                  std::chrono::steady_clock::now() - t0)
                  .count();

    std::cout << "Epoch " << epoch + 1 << '/' << epochs << " | " << ms
              << " ms\n";

    std::cout << "  Train acc global: " << accTrain[0] << '%';
    for (int c = 0; c < nClasses; ++c)
      std::cout << " | class " << c << ": " << accTrain[c + 1] << '%';
    std::cout << '\n';

    std::cout << "  Val   acc global: " << accVal[0] << '%';
    for (int c = 0; c < nClasses; ++c)
      std::cout << " | class " << c << ": " << accVal[c + 1] << '%';
    std::cout << "\n\n";
  }

  // printWeights();
}

// END NEURAL NETWORK