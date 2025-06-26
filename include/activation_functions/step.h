#ifndef STEP_H
#define STEP_H

#include "activation_function.h"
#include <memory>
#include <vector>

class StepFunction : public ActivationFunction {
public:
  void activate(std::vector<float> &values) const override;
  float derivative(float activated) const override;

  std::unique_ptr<ActivationFunction> clone() const override;
};

#endif // STEP_H
