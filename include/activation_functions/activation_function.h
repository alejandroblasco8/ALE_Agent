#ifndef ACTIVATION_FUNCTION_H
#define ACTIVATION_FUNCTION_H

class ActivationFunction {
public:
  virtual float activate(float input) const = 0;
  virtual ~ActivationFunction() = default;
};

#endif // ACTIVATION_FUNCTION_H
