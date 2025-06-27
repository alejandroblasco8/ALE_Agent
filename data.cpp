#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

std::vector<double> accuracy(const std::vector<int> &preds,
                             const std::vector<int> &targets, int nClasses) {
  if (preds.size() != targets.size()) {
    throw std::runtime_error("size mismatch");
  }

  std::vector<int> correct(nClasses, 0);
  std::vector<int> total(nClasses, 0);

  std::size_t globalHits = 0;

  for (std::size_t i = 0; i < preds.size(); ++i) {
    total[targets[i]]++;

    if (preds[i] == targets[i]) {
      correct[targets[i]]++;
      globalHits++;
    }
  }

  std::vector<double> acc(nClasses + 1, 0.0);
  acc[0] = 100.0 * globalHits / static_cast<double>(preds.size());

  for (int c = 0; c < nClasses; ++c) {
    acc[c + 1] = total[c] ? 100.0 * correct[c] / total[c] : 0.0;
  }

  return acc;
}

void balanceToMin(std::vector<std::vector<float>> &X, std::vector<int> &y,
                  unsigned seed = 1) {
  std::unordered_map<int, std::vector<std::size_t>> idxByClass;
  for (std::size_t i = 0; i < y.size(); ++i) {
    idxByClass[y[i]].push_back(i);
  }

  std::size_t minCount = std::numeric_limits<std::size_t>::max();
  for (const auto &kv : idxByClass) {
    minCount = std::min(minCount, kv.second.size());
  }

  std::mt19937 rng{seed};
  std::vector<std::size_t> keep;
  keep.reserve(minCount * idxByClass.size());

  for (auto &kv : idxByClass) {
    auto &vec = kv.second;
    std::shuffle(vec.begin(), vec.end(), rng);
    keep.insert(keep.end(), vec.begin(), vec.begin() + minCount);
  }

  std::vector<std::vector<float>> X_bal;
  std::vector<int> y_bal;
  X_bal.reserve(keep.size());
  y_bal.reserve(keep.size());

  for (std::size_t idx : keep) {
    X_bal.push_back(std::move(X[idx]));
    y_bal.push_back(y[idx]);
  }

  X.swap(X_bal);
  y.swap(y_bal);
}

void shuffleData(std::vector<std::vector<float>> &inputs,
                 std::vector<int> &targets, unsigned seed = 1) {
  if (inputs.size() != targets.size()) {
    throw std::runtime_error("size mismatch");
  }

  std::mt19937 rng(seed);

  for (std::size_t i = inputs.size(); i-- > 1;) {
    std::uniform_int_distribution<std::size_t> dist(0, i);
    std::size_t j = dist(rng);

    std::swap(inputs[i], inputs[j]);
    std::swap(targets[i], targets[j]);
  }
}

void loadAndData(const std::string &filename,
                 std::vector<std::vector<float>> &inputs,
                 std::vector<int> &targets) {
  std::ifstream file(filename);

  if (!file) {
    throw std::runtime_error("Could not open " + filename);
  }

  std::string line;
  bool firstLine = true;

  while (getline(file, line)) {
    if (firstLine) {
      firstLine = false;
      continue;
    }

    std::stringstream ss(line);
    std::string column;
    std::vector<float> row;

    if (!getline(ss, column, ',')) {
      throw std::runtime_error("missing Value_A in: " + line);
    }
    row.push_back(stof(column));

    if (!getline(ss, column, ',')) {
      throw std::runtime_error("missing Value_B in: " + line);
    }
    row.push_back(stof(column));

    if (!getline(ss, column, ',')) {
      throw std::runtime_error("missing AND_Gate in: " + line);
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

void loadData(const std::string &filename,
              std::vector<std::vector<float>> &inputs,
              std::vector<int> &targets, std::size_t nlabels) {
  std::ifstream file(filename);
  if (!file) {
    throw std::runtime_error("Could not open " + filename);
  }

  std::unordered_map<std::string, int> label2id;
  int nextId = 0;
  std::string line;
  bool firstLine = true;

  while (getline(file, line)) {
    if (firstLine) {
      firstLine = false;
      continue;
    }

    std::stringstream ss(line);
    std::string column;
    std::vector<float> row;

    for (std::size_t i = 0; i < nlabels; ++i) {
      if (!getline(ss, column, ',')) {
        throw std::runtime_error("Missing feature at column " +
                                 std::to_string(i) + " in line: " + line);
      }

      if (filename.rfind("ram", 0) == 0) {
        row.push_back(static_cast<float>(stoi(column)));
      } else if (filename.rfind("iris", 0) == 0) {
        if (i != 0) {
        	row.push_back(stof(column));
        } else {
        	continue;
        }
      } else {
     		std::runtime_error("Dataset loading not implemented");
      }
    }

    if (!getline(ss, column, ',')) {
      throw std::runtime_error("Missing label in: " + line);
    }

    int label = label2id.try_emplace(column, nextId).first->second;
    if (label == nextId) {
      nextId++;
    }

    inputs.push_back(std::move(row));
    targets.push_back(label);
  }

  for (const auto &kv : label2id) {
    std::cout << kv.first << " " << kv.second << std::endl;
  }
}
