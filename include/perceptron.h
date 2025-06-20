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
  Perceptron(size_t input_dim, float learning_rate = 0.1,
             size_t randomSeed = 1);

  int predict(const std::vector<float> &input) const;

  void fit(const std::vector<std::vector<float>> &x, const std::vector<int> &y,
           size_t epochs);
};

#endif // PERCEPTRON_H
