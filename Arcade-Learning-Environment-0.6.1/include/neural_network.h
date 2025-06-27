#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

#include "activation_functions/activation_function.h"
#include "weights_initialization/kaiming.h"
#include "weights_initialization/weights_initializer.h"

#include <memory>
#include <vector>

class Neuron {
private:
  inline static std::size_t n = 0;

  float delta;
  float output;
  size_t inputSize;
  std::vector<float> weights;

public:
  Neuron(
      size_t nInputs,
      std::unique_ptr<WeightsInitializer> wInit = std::make_unique<Kaiming>(),
      std::size_t randomSeed = 1);
  Neuron(const Neuron &neuron);

  float feedForward(const std::vector<float> &input);

  void setOutput(float output);
  float getOutput();

  void setDelta(float delta);
  float getDelta();

  std::vector<float> &getWeights();
};

class Layer {
private:
  size_t inputSize;
  size_t outputSize;

  std::vector<Neuron> neurons;
  std::unique_ptr<ActivationFunction> activationFunction;

public:
  Layer(
      std::size_t nInputs, std::size_t nOutputs,
      std::unique_ptr<ActivationFunction> activationFunction,
      std::unique_ptr<WeightsInitializer> wInit = std::make_unique<Kaiming>());
  Layer(const Layer &layer);

  std::vector<float> feedForward(const std::vector<float> &input);

  size_t getInputSize();
  size_t getOutputSize();

  std::vector<Neuron> &getNeurons();
  ActivationFunction &getActivationFunction();
};

class NeuralNetwork {
private:
  std::vector<Layer> layers;

  void backPropagation(const std::vector<float> &expected);
  void updateWeights(const std::vector<float> &input, float learningRate);

public:
  NeuralNetwork(const std::vector<Layer> &layers);
  std::vector<float> feedForward(const std::vector<float> &input);
  std::vector<Layer>& getLayers() { return layers; }
  void printWeights();
  void train(const std::vector<std::vector<float>> &inputs,
             const std::vector<int> &targets, int epochs, float learningRate);
};

#endif // NEURAL_NETWORK
