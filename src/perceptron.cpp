#include "../include/perceptron.h"
#include "../include/activation_functions/step.h"

#include <cassert>
#include <iostream>

using std::cout;
using std::endl;
using std::make_unique;
using std::mt19937;
using std::size_t;
using std::uniform_int_distribution;
using std::vector;

Perceptron::Perceptron(size_t input_dim, float learning_rate,
                       size_t randomSeed) {
  this->input_dim = input_dim;
  this->learning_rate = learning_rate;

  this->weights = vector<float>(input_dim + 1, 0.0);
  this->afunc = make_unique<StepFunction>();

  this->rng = mt19937(randomSeed);
}

int Perceptron::predict(const vector<float> &input) const {
  float sum = this->weights[0];

  for (size_t i = 0; i < input.size(); i++) {
    sum += input[i] * this->weights[i + 1];
  }

  float output = this->afunc->activate(sum);
  return static_cast<int>(output);
}

void Perceptron::fit(const vector<vector<float>> &x, const vector<int> &y,
                     size_t epochs) {
  // Ensure X and Y are the same size
  assert(x.size() == y.size());

  vector<float> bestWeights = this->weights;
  float bestAcc = 0.0;

  cout << "Starting training..." << endl;

  // Loop over the specified epochs
  for (size_t e = 0; e < epochs + 1; ++e) {
    if (e != epochs) {
      cout << "Epoch " << e << "/" << epochs - 1 << endl;
    }

    vector<size_t> errors;

    // Collect all the misclassified samples
    for (size_t i = 0; i < x.size(); ++i) {
      const vector<float> &input = x.at(i);
      int target = y[i];

      int pred = this->predict(input);

      if (pred != target) {
        errors.push_back(i);
      }
    }

    float accuracy = static_cast<float>(x.size() - errors.size()) / x.size();

    if (e != epochs) {
      cout << errors.size() << " samples misclassified in epoch " << e
           << " with " << accuracy << " accuracy" << endl;
    }

    // Save the new weights and accuracy if it has improved
    if (accuracy > bestAcc) {
      cout << "Best accuracy and weights updated" << endl;

      bestWeights = this->weights;
      bestAcc = accuracy;
    }

    // Exit early if there are no errors
    if (errors.empty() && !(e == epochs)) {
      cout << "Exiting early with perfect accuracy" << endl;
      bestWeights = this->weights;
      break;
    }

    // Use the extra epoch just to check the new weights accuracy
    if (e == epochs) {
      break;
    }

    // Pick a random error
    uniform_int_distribution<size_t> uni(0, errors.size() - 1);
    size_t index = errors[uni(this->rng)];

    cout << "Error " << index << " (base 0) selected to update the weights"
         << endl;

    const vector<float> &errX = x[index];
    int errY = (y[index] == 0) ? -1 : 1;

    // Update the learning rate
    this->weights[0] += learning_rate * errY;

    // Update the weights
    for (size_t j = 0; j < errX.size(); ++j) {
      this->weights[j + 1] += this->learning_rate * errY * errX[j];
    }

    cout << "Updated weights:";

    for (size_t j = 0; j < this->weights.size(); ++j) {
      cout << " " << this->weights[j];
    }

    cout << endl;
  }

  cout << "Training completed" << endl;
  cout << "Best weights after training: [";

  for (size_t i = 0; i < bestWeights.size(); ++i) {
    cout << " " << bestWeights[i];
  }

  cout << " ] with accuracy: " << bestAcc << endl;

  this->weights = bestWeights;
}