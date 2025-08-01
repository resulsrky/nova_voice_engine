#include "processing/audio_gain_controller.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>

namespace processing {

AudioGainController::AudioGainController(
    float target_level,
    float max_gain,
    float min_gain,
    float attack_rate,
    float release_rate)
    : target_level_(target_level),
      max_gain_(max_gain),
      min_gain_(min_gain),
      attack_rate_(attack_rate),
      release_rate_(release_rate),
      current_gain_(1.0f),
      current_level_(0.0f) {
    
    std::cout << "Audio Gain Controller başlatıldı - Target Level: " << target_level_ << std::endl;
}

float AudioGainController::calculate_rms(const std::vector<int16_t>& samples) {
    if (samples.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (const auto& sample : samples) {
        sum += static_cast<float>(sample * sample);
    }
    
    return std::sqrt(sum / samples.size());
}

void AudioGainController::process(std::vector<int16_t>& samples) {
    if (samples.empty()) return;
    
    // Mevcut RMS seviyesini hesapla
    float rms = calculate_rms(samples);
    
    // Smooth level tracking
    if (rms > current_level_) {
        current_level_ = attack_rate_ * rms + (1.0f - attack_rate_) * current_level_;
    } else {
        current_level_ = release_rate_ * rms + (1.0f - release_rate_) * current_level_;
    }
    
    // Gain hesapla
    if (current_level_ > 10.0f) { // Minimum threshold
        float desired_gain = target_level_ / current_level_;
        desired_gain = std::clamp(desired_gain, min_gain_, max_gain_);
        
        // Smooth gain changes
        if (desired_gain > current_gain_) {
            current_gain_ = attack_rate_ * desired_gain + (1.0f - attack_rate_) * current_gain_;
        } else {
            current_gain_ = release_rate_ * desired_gain + (1.0f - release_rate_) * current_gain_;
        }
    }
    
    // Gain'i uygula
    for (auto& sample : samples) {
        float processed = static_cast<float>(sample) * current_gain_;
        
        // Clipping protection
        processed = std::clamp(processed, -32767.0f, 32767.0f);
        sample = static_cast<int16_t>(processed);
    }
}

void AudioGainController::reset() {
    current_gain_ = 1.0f;
    current_level_ = 0.0f;
}

}