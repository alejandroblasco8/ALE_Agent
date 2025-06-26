#ifndef ACTIVATION_FUNCTION_H
#define ACTIVATION_FUNCTION_H

#include <memory>
#include <vector>

class ActivationFunction {
public:
  virtual ~ActivationFunction() = default;
  virtual void activate(std::vector<float> &) const = 0;
  virtual float derivative(float) const = 0;
  virtual std::unique_ptr<ActivationFunction> clone() const = 0;
};

#endif
