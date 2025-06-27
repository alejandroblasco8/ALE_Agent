#include "activation_functions/relu.h"
#include "activation_functions/softmax.h"
#include "neural_network.h"
#include "perceptron.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <vector>

using namespace std;

void balanceToMin(std::vector<std::vector<float>> &X, std::vector<int> &y,
                  unsigned seed = 1);

std::vector<double> accuracy(const std::vector<int> &preds,
                             const std::vector<int> &targets, int nClasses);

std::tuple<std::vector<std::vector<float>>, std::vector<int>,
           std::vector<std::vector<float>>, std::vector<int>,
           std::vector<std::vector<float>>, std::vector<int>>
splitDataset(const std::vector<std::vector<float>> &inputs,
             const std::vector<int> &targets, float trainRatio = 0.7,
             float valRatio = 0.15, float testRatio = 0.15,
             unsigned int seed = 1);

void shuffleData(vector<vector<float>> &, vector<int> &, unsigned);
void loadAndData(const std::string &, std::vector<std::vector<float>> &,
                 std::vector<int> &);
void loadData(const string &, vector<vector<float>> &, vector<int> &, size_t);

std::vector<std::vector<float>>
filterColumns(const std::vector<std::vector<float>> &,
              const std::vector<std::size_t> &);

void z_score(vector<float> &);
void min_max(vector<float> &);

size_t indexOfMax(const vector<float> &v) {
  if (v.empty()) {
    throw runtime_error("empty vector");
  }
  auto it = max_element(v.begin(), v.end());
  return static_cast<size_t>(it - v.begin());
}

vector<int> predictAll(NeuralNetwork &net,
                       const vector<vector<float>> &inputs) {
  vector<int> preds;
  preds.reserve(inputs.size());

  for (const auto &x : inputs) {
    auto out = net.feedForward(x);
    preds.push_back(indexOfMax(out));
  }

  return preds;
}

void testPerceptron(Perceptron &p, vector<vector<float>> &inputs,
                    vector<int> &targets, const size_t epochs, float lr) {
  vector<int> predsBefore;
  predsBefore.reserve(inputs.size());

  for (const auto &x : inputs) {
    predsBefore.push_back(p.predict(x));
  }

  auto accBefore = accuracy(predsBefore, targets, 2);

  p.fit(inputs, targets, epochs, lr);

  vector<int> predsAfter;
  predsAfter.reserve(inputs.size());

  for (const auto &x : inputs) {
    predsAfter.push_back(p.predict(x));
  }

  auto accAfter = accuracy(predsAfter, targets, 2);

  cout << "Accuracy before training:  " << accBefore[0] << ' ' << accBefore[1]
       << ' ' << accBefore[2] << '\n';

  cout << "Accuracy after training:   " << accAfter[0] << ' ' << accAfter[1]
       << ' ' << accAfter[2] << '\n';
}

void testNeuralNetork(NeuralNetwork &nn, vector<vector<float>> &inputs,
                      vector<int> &targets, const int epochs, const float lr) {
  auto [xTrain, yTrain, xVal, yVal, xTest, yTest] =
      splitDataset(inputs, targets);

  auto predsBefore = predictAll(nn, xTest);
  auto accBefore = accuracy(predsBefore, yTest, 6);

  nn.train(xTrain, yTrain, xVal, yVal, epochs, lr);

  auto predsAfter = predictAll(nn, xTest);
  auto accAfter = accuracy(predsAfter, yTest, 6);

  cout << "Accuracy before training: ";
  for (unsigned i = 0; i < accBefore.size(); ++i) {
    cout << accBefore[i] << ' ';
  }
  cout << '\n';

  cout << "Accuracy after training: ";
  for (unsigned i = 0; i < accAfter.size(); ++i) {
    cout << accAfter[i] << ' ';
  }
  cout << '\n';
}

void perceptron() {
  vector<vector<float>> inputs;
  vector<int> targets;

  try {
    loadAndData("and.csv", inputs, targets);
    shuffleData(inputs, targets, 1);

    cout << "Loaded " << inputs.size() << " samples with " << inputs[0].size()
         << " features." << endl;
  } catch (const exception &e) {
    cerr << "Error: " << e.what() << endl;
    exit(1);
  }

  const float lr = 0.001;
  const size_t epochs = 10;
  Perceptron p(inputs[0].size());

  testPerceptron(p, inputs, targets, epochs, lr);
}

void neuralNetwork() {
  vector<vector<float>> inputs;
  vector<int> targets;

  try {
    loadData("ram2.csv", inputs, targets, 128);
    shuffleData(inputs, targets, 1);

    cout << "Loaded " << inputs.size() << " samples with " << inputs[0].size()
         << " features." << endl;
    cout << "Number of unique targets: "
         << *std::max_element(targets.begin(), targets.end()) + 1 << '\n';

    for (auto &t : targets) {
      if (t == 5) {
        t = 2;
      } else if (t == 4) {
        t = 2;
      }
    }

    std::unordered_map<int, std::size_t> classCounts;
    for (int t : targets) {
      ++classCounts[t];
    }

    std::cout << "Samples per class (original):\n";
    for (auto &kv : classCounts) {
      std::cout << "  class " << kv.first << ": " << kv.second << '\n';
    }

    balanceToMin(inputs, targets);

    classCounts.clear();
    for (int t : targets) {
      ++classCounts[t];
    }
    std::cout << "Samples per class (balanced):\n";
    for (auto &kv : classCounts) {
      std::cout << "  class " << kv.first << ": " << kv.second << '\n';
    }

    std::vector<size_t> indices = {16, 17, 18, 19, 21, 37, 38, 39,
                                   40, 41, 42, 43, 44, 45, 46};

    inputs = filterColumns(inputs, indices);
    cout << "Sample before normalization: " << inputs[0][0] << '\n';

    for (auto &d : inputs) {
      // z_score(d);
      min_max(d);
    }

    cout << "Sample after normalization: " << inputs[0][0] << '\n';
  } catch (const exception &e) {
    cerr << "Error: " << e.what() << endl;
    exit(1);
  }

  vector<Layer> layers = {
      Layer(15, 64, make_unique<ReLU>()),
      Layer(64, 128, make_unique<ReLU>()),
      Layer(128, 64, make_unique<ReLU>()),
      Layer(64, 4, make_unique<Softmax>()),
  };

  const int epochs = 10;
  const float lr = 0.001;

  NeuralNetwork nn(layers);
  testNeuralNetork(nn, inputs, targets, epochs, lr);
}

int main() {
  // perceptron();
  neuralNetwork();

  return 0;
}