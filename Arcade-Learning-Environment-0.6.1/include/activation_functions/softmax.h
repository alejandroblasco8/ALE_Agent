#ifndef SOFTMAX_H
#define SOFTMAX_H

#include "activation_function.h"

class Softmax : public ActivationFunction {
public:
  void activate(std::vector<float> &values) const override;
  float derivative(float activated) const override;
  std::unique_ptr<ActivationFunction> clone() const override;
};

#endif // SOFTMAX_H
