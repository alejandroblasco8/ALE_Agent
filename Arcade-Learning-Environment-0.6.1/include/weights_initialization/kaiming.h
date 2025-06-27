#ifndef KAIMING_H
#define KAIMING_H

#include "weights_initializer.h"

class Kaiming : public WeightsInitializer {
public:
  std::pair<float, float> getDistParams(int n_in) const override;
  std::unique_ptr<WeightsInitializer> clone() const override;
};

#endif // KAIMING_H
