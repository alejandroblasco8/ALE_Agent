#include <fstream>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

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
      } else {
        row.push_back(stof(column));
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
}
