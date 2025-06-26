#include "../../include/activation_functions/step.h"

#include <vector>

void StepFunction::activate(std::vector<float> &values) const {
  for (float &v : values) {
    v = (v >= 0.0) ? 1.0 : 0.0;
  }
}

float StepFunction::derivative(float) const { return 0.0; }

std::unique_ptr<ActivationFunction> StepFunction::clone() const {
  return std::make_unique<StepFunction>(*this);
}
