// Synthetic point cloud generators for the desktop view template.

/**
 * @file synthetic.cpp
 *
 * @brief Synthetic point cloud generation
 *
 * Generates synthetic test point clouds:
 * - Random uniform sphere
 * - Gaussian cluster
 * - Grid with noise
 * - Parametric surfaces (plane, sphere, cylinder)
 *
 * Used for testing without requiring real scan data.
 *
 * @see synthetic.h
 */
#include "synthetic.h"

#include <cmath>
#include <random>

namespace synthetic {

namespace {
constexpr float kPi = 3.14159265358979323846f;
}

std::vector<float> spiral(size_t n) {
    std::vector<float> out;
    out.reserve(n * 3);
    for (size_t i = 0; i < n; ++i) {
        const float t = static_cast<float>(i) / static_cast<float>(n) * 6.0f * kPi;
        const float r = 0.05f + 0.5f * (1.0f - std::cos(t * 0.5f));
        out.push_back(r * std::cos(t));         // x
        out.push_back(r * std::sin(t));         // y
        out.push_back((static_cast<float>(i) / static_cast<float>(n)) - 0.5f);  // z
    }
    return out;
}

std::vector<float> gaussian_cloud(size_t n, float sigma, unsigned seed) {
    std::mt19937 rng(seed);
    std::normal_distribution<float> dist(0.0f, sigma);
    std::vector<float> out;
    out.reserve(n * 3);
    for (size_t i = 0; i < n; ++i) {
        out.push_back(dist(rng));
        out.push_back(dist(rng));
        out.push_back(dist(rng));
    }
    return out;
}

std::vector<float> helix(size_t n, float radius, float pitch) {
    std::vector<float> out;
    out.reserve(n * 3);
    for (size_t i = 0; i < n; ++i) {
        const float t = static_cast<float>(i) / static_cast<float>(n) * 4.0f * kPi;
        out.push_back(radius * std::cos(t));
        out.push_back(radius * std::sin(t));
        out.push_back(pitch * t);
    }
    return out;
}

}  // namespace synthetic
