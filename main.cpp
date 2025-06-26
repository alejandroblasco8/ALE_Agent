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

void shuffleData(vector<vector<float>> &inputs, vector<int> &targets,
                 unsigned seed = random_device{}()) {
  if (inputs.size() != targets.size()) {
    throw runtime_error("size mismatch");
  }

  mt19937 rng(seed);

  for (size_t i = inputs.size(); i-- > 1;) {
    uniform_int_distribution<size_t> dist(0, i);
    size_t j = dist(rng);

    swap(inputs[i], inputs[j]);
    swap(targets[i], targets[j]);
  }
}

void loadAndData(const string &filename, vector<vector<float>> &inputs,
                 vector<int> &targets) {
  ifstream file(filename);

  if (!file) {
    throw runtime_error("Could not open " + filename);
  }

  string line;
  bool firstLine = true;

  while (getline(file, line)) {
    if (firstLine) {
      firstLine = false;
      continue;
    }

    stringstream ss(line);
    string column;
    vector<float> row;

    if (!getline(ss, column, ',')) {
      throw runtime_error("missing Value_A in: " + line);
    }
    row.push_back(stof(column));

    if (!getline(ss, column, ',')) {
      throw runtime_error("missing Value_B in: " + line);
    }
    row.push_back(stof(column));

    if (!getline(ss, column, ',')) {
      throw runtime_error("missing AND_Gate in: " + line);
    }

    int label;
    if (column == "zero") {
      label = 0;
    } else if (column == "one") {
      label = 1;
    } else {
      continue;
    }

    inputs.push_back(move(row));
    targets.push_back(label);
  }
}

void loadIrisData(const string &filename, vector<vector<float>> &inputs,
                  vector<int> &targets) {
  ifstream file(filename);
  if (!file) {
    throw runtime_error("Could not open " + filename);
  }

  unordered_map<string, int> label2id;
  int nextId = 0;
  string line;
  bool firstLine = true;

  while (getline(file, line)) {
    if (firstLine) {
      firstLine = false;
      continue;
    }

    stringstream ss(line);
    string column;

    getline(ss, column, ',');
    vector<float> row;

    for (int i = 0; i < 4; ++i) {
      if (!getline(ss, column, ',')) {
        throw runtime_error("missing features in: " + line);
      }

      row.push_back(stof(column));
    }

    if (!getline(ss, column, ',')) {
      throw runtime_error("missing label in: " + line);
    }

    int label = label2id.try_emplace(column, nextId).first->second;

    if (label == nextId) {
      nextId++;
    }

    inputs.push_back(move(row));
    targets.push_back(label);
  }
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

  const float lr = 0.1;
  const size_t epochs = 10;
  Perceptron p(inputs[0].size());

  testPerceptron(p, inputs, targets, epochs, lr);
}

void neuralNetwork() {
  vector<vector<float>> inputs;
  vector<int> targets;

  try {
    loadIrisData("iris.csv", inputs, targets);
    shuffleData(inputs, targets, 1);

    cout << "Loaded " << inputs.size() << " samples with " << inputs[0].size()
         << " features." << endl;
  } catch (const exception &e) {
    cerr << "Error: " << e.what() << endl;
    exit(1);
  }

  vector<Layer> layers = {
      Layer(4, 12, make_unique<ReLU>()),
      Layer(12, 6, make_unique<ReLU>()),
      Layer(6, 3, make_unique<Softmax>()),
  };

  const int epochs = 100;
  const float lr = 0.01;

  NeuralNetwork nn(layers);
  testNeuralNetork(nn, inputs, targets, epochs, lr);
}

int main() {
  perceptron();
  // neuralNetwork();

  return 0;
}