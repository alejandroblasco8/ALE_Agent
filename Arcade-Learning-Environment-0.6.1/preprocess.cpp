#include <algorithm>
#include <cmath>
#include <numeric>
#include <vector>

std::vector<float> min_max(const std::vector<float> &data) {
  float min_val = *std::min_element(data.begin(), data.end());
  float max_val = *std::max_element(data.begin(), data.end());

  std::vector<float> scaled;
  for (float x : data) {
    scaled.push_back((x - min_val) / (max_val - min_val));
  }

  return scaled;
}

std::vector<float> z_score(const std::vector<float> &data) {
  float sum = std::accumulate(data.begin(), data.end(), 0.0);
  float mean = sum / data.size();

  float sum_sq_diff = 0.0;

  for (float x : data) {
    sum_sq_diff += (x - mean) * (x - mean);
  }

  float stddev = std::sqrt(sum_sq_diff / data.size());

  std::vector<float> standardized;

  for (float x : data) {
    standardized.push_back((x - mean) / stddev);
  }

  return standardized;
}