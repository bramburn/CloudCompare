#pragma once

#include <vector>
#include <cstddef>

namespace synthetic {

/// Generate a spiral point cloud of N points in a unit-ish box.
/// X, Y, Z in approximately [-1, 1]. Used as default test data.
std::vector<float> spiral(size_t n);

/// Generate a random Gaussian point cloud with `n` points.
std::vector<float> gaussian_cloud(size_t n, float sigma = 0.4f, unsigned seed = 42);

/// Generate a noise-free helical spring shape.
std::vector<float> helix(size_t n, float radius = 0.7f, float pitch = 0.3f);

}  // namespace synthetic
