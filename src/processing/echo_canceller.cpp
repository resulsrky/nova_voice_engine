#include "processing/echo_canceller.hpp"
#include <algorithm>
#include <cmath>
#include <mutex>
#include <iostream>

namespace processing {
EchoCanceller::EchoCanceller(size_t max_delay_samples)
    : max_delay_(max_delay_samples),
      echo_suppression_factor_(0.8f), // %80 echo bastırma
      adaptive_threshold_(1000.0f),
      learning_rate_(0.01f) {
    
    std::cout << "Agresif Echo Canceller başlatıldı - Max Delay: " << max_delay_samples << std::endl;
}

void EchoCanceller::on_playback(const std::vector<int16_t>& samples) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Playback buffer'a ekle
    echo_buffer_.insert(echo_buffer_.end(), samples.begin(), samples.end());
    
    // Buffer boyutunu kontrol et
    if (echo_buffer_.size() > max_delay_) {
        echo_buffer_.erase(echo_buffer_.begin(), echo_buffer_.end() - max_delay_);
    }
    
    // RMS hesapla playback için
    float rms = 0.0f;
    for (const auto& sample : samples) {
        rms += static_cast<float>(sample * sample);
    }
    rms = std::sqrt(rms / samples.size());
    
    // Adaptif threshold güncelle
    if (rms > 100.0f) { // Aktif playback varsa
        adaptive_threshold_ = learning_rate_ * rms + (1.0f - learning_rate_) * adaptive_threshold_;
    }
}

void EchoCanceller::process(std::vector<int16_t>& capture) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (echo_buffer_.empty()) {
        return; // Echo data yok
    }
    
    // Capture RMS hesapla
    float capture_rms = 0.0f;
    for (const auto& sample : capture) {
        capture_rms += static_cast<float>(sample * sample);
    }
    capture_rms = std::sqrt(capture_rms / capture.size());
    
    // Echo var mı kontrol et
    if (capture_rms < adaptive_threshold_ * 0.5f) {
        // Çok düşük ses - muhtemelen sadece echo
        for (auto& sample : capture) {
            sample = static_cast<int16_t>(sample * 0.1f); // %90 azalt
        }
        return;
    }
    
    size_t n = std::min(capture.size(), echo_buffer_.size());
    
    for (size_t i = 0; i < n; ++i) {
        float capture_f = static_cast<float>(capture[i]);
        float echo_f = static_cast<float>(echo_buffer_[i]);
        
        // Agresif echo cancellation
        float suppressed = capture_f - (echo_f * echo_suppression_factor_);
        
        // Aşırı suppression önle
        if (std::abs(capture_f) > std::abs(echo_f) * 1.5f) {
            // Gerçek konuşma var - daha az suppression
            suppressed = capture_f - (echo_f * 0.3f);
        }
        
        // Clamp ve set
        int val = static_cast<int>(suppressed);
        val = std::clamp(val, -32768, 32767);
        capture[i] = static_cast<int16_t>(val);
    }
    
    // İşlenen echo data'yı temizle
    if (n > 0) {
        echo_buffer_.erase(echo_buffer_.begin(), echo_buffer_.begin() + n);
    }
}
}
