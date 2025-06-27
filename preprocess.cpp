#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <vector>

void shuffleData(std::vector<std::vector<float>> &, std::vector<int> &,
                 unsigned);

std::vector<std::vector<float>>
filterColumns(const std::vector<std::vector<float>> &data,
              const std::vector<std::size_t> &indices) {

  std::vector<std::vector<float>> filteredData;
  filteredData.reserve(data.size());

  for (const auto &row : data) {
    std::vector<float> filteredRow;
    filteredRow.reserve(indices.size());

    for (std::size_t idx : indices) {
      filteredRow.push_back(row[idx]);
    }

    filteredData.push_back(std::move(filteredRow));
  }

  return filteredData;
}

std::vector<float> min_max(const std::vector<float> &data) {
  float min_val = *std::min_element(data.begin(), data.end());
  float max_val = *std::max_element(data.begin(), data.end());

  std::vector<float> scaled;
  for (float x : data) {
    scaled.push_back((x - min_val) / (max_val - min_val));
  }

  return scaled;
}

void z_score(std::vector<float> &data) {
  if (data.empty()) {
    return;
  }

  float sum = std::accumulate(data.begin(), data.end(), 0.0);
  float mean = sum / data.size();

  float sum_sq_diff = 0.0;

  for (float x : data) {
    sum_sq_diff += (x - mean) * (x - mean);
  }

  float stddev = std::sqrt(sum_sq_diff / data.size());

  if (stddev == 0.0) {
    stddev = 1.0;
  }

  for (float &x : data) {
    x = (x - mean) / stddev;
  }
}

void min_max(std::vector<float> &data) {
  if (data.empty()) {
    return;
  }

  auto [min_it, max_it] = std::minmax_element(data.begin(), data.end());

  float min_val = *min_it;
  float max_val = *max_it;

  float range = max_val - min_val;

  if (range == 0.0) {
    range = 1.0;
  }

  for (float &x : data) {
    x = (x - min_val) / range;
  }
}

std::tuple<std::vector<std::vector<float>>, std::vector<int>,
           std::vector<std::vector<float>>, std::vector<int>,
           std::vector<std::vector<float>>, std::vector<int>>
splitDataset(const std::vector<std::vector<float>> &inputs,
             const std::vector<int> &targets, float trainRatio = 0.7,
             float valRatio = 0.15, float = 0.15, unsigned int seed = 1) {
  std::unordered_map<int, std::vector<std::size_t>> classIndices;

  for (std::size_t i = 0; i < targets.size(); ++i) {
    classIndices[targets[i]].push_back(i);
  }

  std::vector<std::vector<float>> xTrain;
  std::vector<int> yTrain;
  std::vector<std::vector<float>> xVal;
  std::vector<int> yVal;
  std::vector<std::vector<float>> xTest;
  std::vector<int> yTest;

  std::mt19937 rng(seed);

  for (auto &entry : classIndices) {
    std::vector<std::size_t> &idxVec = entry.second;
    std::shuffle(idxVec.begin(), idxVec.end(), rng);

    std::size_t m = idxVec.size();
    std::size_t mTrain = m * trainRatio;
    std::size_t mVal = m * valRatio;

    for (std::size_t k = 0; k < mTrain; ++k) {
      xTrain.push_back(inputs[idxVec[k]]);
      yTrain.push_back(targets[idxVec[k]]);
    }

    for (std::size_t k = mTrain; k < mTrain + mVal; ++k) {
      xVal.push_back(inputs[idxVec[k]]);
      yVal.push_back(targets[idxVec[k]]);
    }

    for (std::size_t k = mTrain + mVal; k < m; ++k) {
      xTest.push_back(inputs[idxVec[k]]);
      yTest.push_back(targets[idxVec[k]]);
    }
  }

  shuffleData(xTrain, yTrain, seed + 1);
  shuffleData(xVal, yVal, seed + 2);
  shuffleData(xTest, yTest, seed + 3);

  return {xTrain, yTrain, xVal, yVal, xTest, yTest};
}
