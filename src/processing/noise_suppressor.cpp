#include "processing/noise_suppressor.hpp"
#include <cmath>

namespace processing {
NoiseSuppressor::NoiseSuppressor(int16_t threshold) : threshold_(threshold) {}

void NoiseSuppressor::process(std::vector<int16_t>& samples) const {
    for (auto& s : samples) {
        if (std::abs(s) < threshold_) s = 0;
    }
}
}
