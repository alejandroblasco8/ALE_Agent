#ifndef RELU_H
#define RELU_H

#include "activation_function.h"

class ReLU : public ActivationFunction {
public:
  void activate(std::vector<float> &values) const override;
  float derivative(float activated) const override;

  std::unique_ptr<ActivationFunction> clone() const override;
};

#endif // RELU_H
