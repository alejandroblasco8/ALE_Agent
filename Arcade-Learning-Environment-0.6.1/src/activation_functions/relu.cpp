#include "../include/activation_functions/relu.h"

#include <memory>

void ReLU::activate(std::vector<float> &values) const {
  for (float &v : values) {
    if (v < 0.0) {
      v = 0.0;
    }
  }
}

float ReLU::derivative(float activated) const {
  return activated > 0.0 ? 1.0 : 0.0;
}

std::unique_ptr<ActivationFunction> ReLU::clone() const {
  return std::make_unique<ReLU>();
}
