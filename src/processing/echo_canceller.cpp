#include "processing/echo_canceller.hpp"
#include <algorithm>
#include <cmath>

namespace processing {
EchoCanceller::EchoCanceller(size_t max_delay_samples)
    : max_delay_(max_delay_samples) {}

void EchoCanceller::on_playback(const std::vector<int16_t>& samples) {
    echo_buffer_.insert(echo_buffer_.end(), samples.begin(), samples.end());
    if (echo_buffer_.size() > max_delay_) {
        echo_buffer_.erase(echo_buffer_.begin(), echo_buffer_.end() - max_delay_);
    }
}

void EchoCanceller::process(std::vector<int16_t>& capture) {
    size_t n = std::min(capture.size(), echo_buffer_.size());
    for (size_t i = 0; i < n; ++i) {
        int val = capture[i] - echo_buffer_[i];
        val = std::clamp(val, -32768, 32767);
        capture[i] = static_cast<int16_t>(val);
    }
    if (n > 0) {
        echo_buffer_.erase(echo_buffer_.begin(), echo_buffer_.begin() + n);
    }
}
}
