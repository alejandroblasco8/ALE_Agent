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

void shuffleData(vector<vector<float>> &, vector<int> &, unsigned);
void loadAndData(const std::string &, std::vector<std::vector<float>> &,
                 std::vector<int> &);
void loadData(const string &, vector<vector<float>> &, vector<int> &, size_t);
std::pair<std::vector<std::vector<float>>, std::vector<int>>
loadDataPerceptron(const std::string &, const std::string& ) ;
std::pair<std::vector<std::vector<float>>, std::vector<int>>
shuffleDataPerceptron(const std::vector<std::vector<float>> &, const std::vector<int> &, unsigned seed = 1);

size_t indexOfMax(const vector<float> &v) {
  if (v.empty()) {
    throw runtime_error("empty vector");
  }
  auto it = max_element(v.begin(), v.end());
  return static_cast<size_t>(it - v.begin());
}

vector<double> accuracy(const vector<int> &preds, const vector<int> &targets,
                        int nClasses) {
  if (preds.size() != targets.size()) {
    throw runtime_error("size mismatch");
  }

  vector<int> correct(nClasses, 0);
  vector<int> total(nClasses, 0);

  size_t globalHits = 0;

  for (size_t i = 0; i < preds.size(); ++i) {
    total[targets[i]]++;

    if (preds[i] == targets[i]) {
      correct[targets[i]]++;
      globalHits++;
    }
  }

  vector<double> acc(nClasses + 1, 0.0);
  acc[0] = 100.0 * globalHits / static_cast<double>(preds.size());

  for (int c = 0; c < nClasses; ++c) {
    acc[c + 1] = total[c] ? 100.0 * correct[c] / total[c] : 0.0;
  }

  return acc;
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
  auto predsBefore = predictAll(nn, inputs);
  auto accBefore = accuracy(predsBefore, targets, 3);

  nn.train(inputs, targets, epochs, lr);

  auto predsAfter = predictAll(nn, inputs);
  auto accAfter = accuracy(predsAfter, targets, 3);

  cout << "Accuracy before training:  " << accBefore[0] << ' ' << accBefore[1]
       << ' ' << accBefore[2] << ' ' << accBefore[3] << '\n';
  cout << "Accuracy after training:      " << accAfter[0] << ' ' << accAfter[1]
       << ' ' << accAfter[2] << ' ' << accAfter[3] << '\n';
}


void perceptron() {
  const vector<string> acciones = {
    "PLAYER_A_LEFT", "PLAYER_A_RIGHT", "PLAYER_A_FIRE", "PLAYER_A_RIGHTFIRE", "PLAYER_A_LEFTFIRE"
  };

  const size_t epochs = 10;
  const float lr = 0.1;

  for (const auto& accion : acciones) {
    cout << "\nEntrenando perceptrón para acción: " << accion << endl;

    Perceptron p(128);

    auto [X, y] = loadDataPerceptron("ram2.csv", accion);
    auto [balanced_X, balanced_y] = shuffleDataPerceptron(X, y);

    testPerceptron(p, balanced_X, balanced_y, epochs, lr);

    cout << "---------------------------------------------\n";
  }
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
  } catch (const exception &e) {
    cerr << "Error: " << e.what() << endl;
    exit(1);
  }

  vector<Layer> layers = {
    Layer(128, 6, make_unique<ReLU>()),
    Layer(6, 6, make_unique<Softmax>())
  };


  const int epochs = 1;
  const float lr = 0.001;

  NeuralNetwork nn(layers);
  testNeuralNetork(nn, inputs, targets, epochs, lr);
}

int main() {
  perceptron();
  //neuralNetwork();

  return 0;
}