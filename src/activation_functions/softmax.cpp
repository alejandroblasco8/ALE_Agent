#include "../include/activation_functions/softmax.h"

#include <algorithm>
#include <cmath>
#include <memory>

void Softmax::activate(std::vector<float> &values) const {
  float maxVal = *std::max_element(values.begin(), values.end());
  float sum = 0.0;

  for (float &v : values) {
    v = std::exp(v - maxVal);
    sum += v;
  }

  for (float &v : values) {
    v /= sum;
  }
}

float Softmax::derivative(float activated) const {
  return activated * (1.0 - activated);
}

std::unique_ptr<ActivationFunction> Softmax::clone() const {
  return std::make_unique<Softmax>();
}
