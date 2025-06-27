#include "../include/weights_initialization/kaiming.h"

#include <cmath>
#include <stdexcept>

std::pair<float, float> Kaiming::getDistParams(int n_in) const {
  if (n_in <= 0) {
    throw std::invalid_argument("n_in debe ser mayor que 0");
  }

  float stddev = std::sqrt(2.0 / n_in);
  return {0.0f, stddev};
}

std::unique_ptr<WeightsInitializer> Kaiming::clone() const {
  return std::make_unique<Kaiming>(*this);
}