#ifndef PERCEPTRON_H
#define PERCEPTRON_H

#include "activation_functions/activation_function.h"

#include <cstddef>
#include <memory>
#include <random>
#include <vector>

class Perceptron {
private:
  std::size_t input_dim;
  float learning_rate;

  std::vector<float> weights;
  std::unique_ptr<ActivationFunction> afunc;

  std::mt19937 rng;

public:
  Perceptron(size_t input_dim, size_t randomSeed = 1);

  int predict(const std::vector<float> &input) const;
  std::pair<std::vector<std::vector<float>>, std::vector<int>>load_dataset(const std::string &filename, const std::string &target_action);
  std::pair<std::vector<std::vector<float>>, std::vector<int>>balance_dataset(const std::vector<std::vector<float>> &X, const std::vector<int> &y);

  void fit(const std::vector<std::vector<float>> &inputs,
           const std::vector<int> &targets, size_t epochs, float learning_rate);
};

#endif // PERCEPTRON_H
