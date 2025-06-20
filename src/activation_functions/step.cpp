#include "../../include/activation_functions/step.h"

float StepFunction::activate(float input) const {
  return (input >= 0.0) ? 1.0 : 0.0;
}