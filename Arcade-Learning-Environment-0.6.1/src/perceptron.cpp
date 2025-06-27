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
  this->target = "";
}

std::pair<std::vector<std::vector<float>>, std::vector<int>>
Perceptron::load_dataset(const std::string &filename, const std::string& target_action) {
  this-> target = target_action;
  std::vector<std::vector<float>> X;
  std::vector<int> y;

  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "Error opening file: " << filename << std::endl;
    return {X, y}; 
  }

  std::string line;
  while (std::getline(file, line)) {
    std::stringstream ss(line);
    std::string cell;
    std::vector<float> features;

    while(std::getline(ss, cell, ',')) {
      if (ss.peek() == '\n' || ss.eof()) {
        break;
      }
      features.push_back(std::stof(cell));
    }

    std::getline(ss, cell, ',');
    std::string action = cell;

    if(action == target_action) {
      X.push_back(features);
      y.push_back(1);
    } else if (action == "PLAYER_A_NOOP"){
      X.push_back(features);
      y.push_back(0);
    }
  }

  file.close();
  return {X, y};
}

std::pair<std::vector<std::vector<float>>, std::vector<int>>
Perceptron::balance_dataset(const std::vector<std::vector<float>> &X, const std::vector<int> &y) {
  std::vector<std::vector<float>> class0_X;
  std::vector<std::vector<float>> class1_X;

  for(std::size_t i = 0; i < y.size(); i++){
    if(y[i] == 0){
      class0_X.push_back(X[i]);
    }
    else{
      class1_X.push_back(X[i]);
    }
  }

  std::size_t count0 = class0_X.size();
  std::size_t count1 = class1_X.size();

  std::cout << "Original number of samples per action: class NOOP = " << count0 << ", class 1 = " << count1 << std::endl;

  std::vector<std::vector<float>> balanced_X;
  std::vector<int> balanced_y;

  if (count0 > count1) {
    std::shuffle(class0_X.begin(), class0_X.end(), rng);
    for (std::size_t i = 0; i < count1; ++i) {
      balanced_X.push_back(class0_X[i]);
      balanced_y.push_back(0);
    }
    for (const auto& sample : class1_X) {
      balanced_X.push_back(sample);
      balanced_y.push_back(1);
    }
  } else {
    std::shuffle(class1_X.begin(), class1_X.end(), rng);
    for (std::size_t i = 0; i < count0; ++i) {
      balanced_X.push_back(class1_X[i]);
      balanced_y.push_back(1);
    }
    for (const auto& sample : class0_X) {
      balanced_X.push_back(sample);
      balanced_y.push_back(0);
    }
  }

  std::cout << "Number of samples per action after reduction: " << std::min(count0, count1) << std::endl;

  return {balanced_X, balanced_y};
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
  std::cout << "Perceptron " << this->target << "; ";
  std::cout << "Best weights after training: [";

  for (std::size_t i = 0; i < bestWeights.size(); ++i) {
    std::cout << " " << bestWeights[i];
  }

  std::cout << " ] with accuracy: " << bestAcc << std::endl;

  this->weights = bestWeights;
}
