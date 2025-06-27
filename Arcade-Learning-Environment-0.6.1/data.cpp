#include <fstream>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>

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

std::pair<std::vector<std::vector<float>>, std::vector<int>>
loadDataPerceptron(const std::string &filename, const std::string& target_action) {
  std::string target = target_action;

  std::vector<std::vector<float>> X;
  std::vector<int> y;

  std::ifstream file(filename);
  if (!file.is_open()) {
    throw std::runtime_error("Error opening file: " + filename);
  }

  std::string line;
  bool firstLine = true;

  while (std::getline(file, line)) {
    if (firstLine) {
      firstLine = false;
      continue;
    }

    std::stringstream ss(line);
    std::string column;
    std::vector<std::string> tokens;

    while (std::getline(ss, column, ',')) {
      tokens.push_back(column);
    }

    if (tokens.size() < 2) {
      throw std::runtime_error("Invalid line (too few columns): " + line);
    }

    std::vector<float> features;
    for (size_t i = 0; i < tokens.size() - 1; ++i) {
      try {
        features.push_back(std::stof(tokens[i]));
      } catch (const std::invalid_argument&) {
        throw std::runtime_error("Invalid float: " + tokens[i] + " in line: " + line);
      }
    }

    if (features.empty()) continue;

    const std::string& action = tokens.back();
    if (action == target_action) {
      X.push_back(std::move(features));
      y.push_back(1);
    } else if (action == "PLAYER_A_NOOP") {
      X.push_back(std::move(features));
      y.push_back(0);
    }
  }

  file.close();
  return {X, y};
}


std::pair<std::vector<std::vector<float>>, std::vector<int>>
shuffleDataPerceptron(const std::vector<std::vector<float>> &X, const std::vector<int> &y, unsigned seed = 1) {
  std::vector<std::vector<float>> class0_X;
  std::vector<std::vector<float>> class1_X;

  for(std::size_t i = 0; i < y.size(); i++){
    if(y[i] == 0){
      class0_X.push_back(X[i]);
    }
    else{
      class1_X.push_back(X[i]);
    }
  }

  std::size_t count0 = class0_X.size();
  std::size_t count1 = class1_X.size();
  std::vector<std::vector<float>> balanced_X;
  std::vector<int> balanced_y;

  std::mt19937 rng(seed);

  if (count0 > count1) {
    std::shuffle(class0_X.begin(), class0_X.end(), rng);
    for (std::size_t i = 0; i < count1; ++i) {
      balanced_X.push_back(class0_X[i]);
      balanced_y.push_back(0);
    }
    for (const auto& sample : class1_X) {
      balanced_X.push_back(sample);
      balanced_y.push_back(1);
    }
  } else {
    std::shuffle(class1_X.begin(), class1_X.end(), rng);
    for (std::size_t i = 0; i < count0; ++i) {
      balanced_X.push_back(class1_X[i]);
      balanced_y.push_back(1);
    }
    for (const auto& sample : class0_X) {
      balanced_X.push_back(sample);
      balanced_y.push_back(0);
    }
  }
  
  return {balanced_X, balanced_y};
}
