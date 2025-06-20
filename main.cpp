#include "perceptron.h"
#include <iostream>
#include <vector>

int main() {
  std::vector<std::vector<float>> x = {{0.6, 0.8}, {-0.8, 0.4}, {0.2, -0.9}};
  std::vector<int> y = {1, 0, 0};

  Perceptron p(2, 0.5);

  p.fit(x, y, 2);

  std::vector<float> x_predict = {3.0, -1.0};
  int pred = p.predict(x_predict);

  std::cout << "Prediction for input [";

  for (std::size_t i = 0; i < x_predict.size(); ++i) {
    std::cout << " " << x_predict[i];
  }

  std::cout << " ]: " << pred << std::endl;
}
