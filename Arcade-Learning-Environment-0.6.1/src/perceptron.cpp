#include "../include/perceptron.h"
#include "../include/activation_functions/step.h"

#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

Perceptron::Perceptron(std::size_t input_dim, std::size_t randomSeed) {
  this->input_dim = input_dim;

  this->weights = std::vector<float>(input_dim + 1, 0.0);
  this->afunc = std::make_unique<StepFunction>();

  this->rng = std::mt19937(randomSeed);
}

int Perceptron::predict(const std::vector<float> &input) const {
  float sum = this->weights[0];

  for (std::size_t i = 0; i < input.size(); i++) {
    sum += input[i] * this->weights[i + 1];
  }

  std::vector<float> tmp = {sum};
  this->afunc->activate(tmp);
  float output = tmp[0];

  return static_cast<int>(output);
}

void Perceptron::fit(const std::vector<std::vector<float>> &x,
                     const std::vector<int> &y, std::size_t epochs,
                     float learning_rate) {
  // Ensure X and Y are the same size
  assert(x.size() == y.size());

  std::vector<float> bestWeights = this->weights;
  float bestAcc = 0.0;

  std::cout << "Starting training..." << std::endl;

  // Loop over the specified epochs
  for (std::size_t e = 0; e < epochs + 1; ++e) {
    if (e != epochs) {
      std::cout << "Epoch " << e << "/" << epochs - 1 << std::endl;
    }

    std::vector<std::size_t> errors;

    // Collect all the misclassified samples
    for (std::size_t i = 0; i < x.size(); ++i) {
      const std::vector<float> &input = x.at(i);
      int target = y[i];

      int pred = this->predict(input);

      if (pred != target) {
        errors.push_back(i);
      }
    }

    float accuracy = static_cast<float>(x.size() - errors.size()) / x.size();

    if (e != epochs) {
      std::cout << errors.size() << " samples misclassified in epoch " << e
                << " with " << accuracy << " accuracy" << std::endl;
    }

    // Save the new weights and accuracy if it has improved
    if (accuracy > bestAcc) {
      std::cout << "Best accuracy and weights updated" << std::endl;

      bestWeights = this->weights;
      bestAcc = accuracy;
    }

    // Exit early if there are no errors
    if (errors.empty() && !(e == epochs)) {
      std::cout << "Exiting early with perfect accuracy" << std::endl;
      bestWeights = this->weights;
      break;
    }

    // Use the extra epoch just to check the new weights accuracy
    if (e == epochs) {
      break;
    }

    // Pick a random error
    std::uniform_int_distribution<std::size_t> uni(0, errors.size() - 1);
    std::size_t index = errors[uni(this->rng)];

    std::cout << "Error " << index << " (base 0) selected to update the weights"
              << std::endl;

    const std::vector<float> &errX = x[index];
    int errY = (y[index] == 0) ? -1 : 1;

    // Update the learning rate
    this->weights[0] += learning_rate * errY;

    // Update the weights
    for (std::size_t j = 0; j < errX.size(); ++j) {
      this->weights[j + 1] += learning_rate * errY * errX[j];
    }

    std::cout << "Updated weights:";

    for (std::size_t j = 0; j < this->weights.size(); ++j) {
      std::cout << " " << this->weights[j];
    }

    std::cout << std::endl;
  }

  std::cout << "Training completed" << std::endl;
  std::cout << "Best weights after training: [";

  for (std::size_t i = 0; i < bestWeights.size(); ++i) {
    std::cout << " " << bestWeights[i];
  }

  std::cout << " ] with accuracy: " << bestAcc << std::endl;

  this->weights = bestWeights;
}
