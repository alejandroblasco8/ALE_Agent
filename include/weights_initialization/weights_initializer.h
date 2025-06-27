#ifndef WEIGHTS_INITIALIZER_H
#define WEIGHTS_INITIALIZER_H

#include <memory>
#include <utility>

class WeightsInitializer {
public:
  virtual std::pair<float, float> getDistParams(int n_in) const = 0;
  virtual ~WeightsInitializer() = default;
  virtual std::unique_ptr<WeightsInitializer> clone() const = 0;
};

#endif // WEIGHTS_INITIALIZER_H
