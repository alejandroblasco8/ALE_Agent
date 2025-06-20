#ifndef STEP_H
#define STEP_H

#include "activation_function.h"

class StepFunction : public ActivationFunction {
public:
  float activate(float input) const override;
};

#endif // STEP_H
