#include "processing/noise_suppressor.hpp"
#include <cmath>

namespace processing {
NoiseSuppressor::NoiseSuppressor(int16_t initial_threshold, float alpha)
    : threshold_(static_cast<float>(initial_threshold)),
      noise_level_(static_cast<float>(initial_threshold)),
      alpha_(alpha) {}

void NoiseSuppressor::process(std::vector<int16_t>& samples) {
    for (auto& s : samples) {
        noise_level_ = alpha_ * noise_level_ + (1.0f - alpha_) * std::abs(s);
        float dynamic_threshold = std::max(noise_level_, threshold_);
        if (std::abs(s) < dynamic_threshold) {
            s = 0;
        }
    }
}
}
