#include "processing/noise_suppressor.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>

namespace processing {
NoiseSuppressor::NoiseSuppressor(int16_t initial_threshold, float alpha)
    : threshold_(static_cast<float>(initial_threshold * 3)), // 3x daha agresif
      noise_level_(static_cast<float>(initial_threshold)),
      alpha_(alpha),
      silence_counter_(0),
      noise_gate_threshold_(1000.0f), // Çok daha yüksek threshold
      noise_reduction_factor_(0.1f) { // %90 gürültü azaltma
    
    std::cout << "Agresif Noise Suppressor başlatıldı - Threshold: " << threshold_ << std::endl;
}

void NoiseSuppressor::process(std::vector<int16_t>& samples) {
    // RMS (Root Mean Square) hesapla - daha doğru ses seviyesi
    float rms = 0.0f;
    for (const auto& sample : samples) {
        rms += static_cast<float>(sample * sample);
    }
    rms = std::sqrt(rms / samples.size());
    
    // Adaptif noise level güncelle
    if (rms < noise_gate_threshold_) {
        // Sessizlik/gürültü bölgesi
        noise_level_ = alpha_ * noise_level_ + (1.0f - alpha_) * rms;
        silence_counter_++;
    } else {
        // Aktif konuşma
        silence_counter_ = 0;
    }
    
    // Dinamik threshold hesapla
    float dynamic_threshold = std::max(noise_level_ * 2.5f, noise_gate_threshold_);
    
    // Agresif noise suppression uygula
    for (auto& sample : samples) {
        float abs_sample = std::abs(sample);
        
        if (abs_sample < dynamic_threshold) {
            // Gürültü - büyük oranda azalt veya sıfırla
            if (silence_counter_ > 10) { // 100ms sessizlik sonrası tamamen sıfırla
                sample = 0;
            } else {
                sample = static_cast<int16_t>(sample * noise_reduction_factor_);
            }
        } else if (abs_sample < dynamic_threshold * 1.5f) {
            // Geçiş bölgesi - kademeli azaltma
            float reduction = 0.3f + 0.7f * ((abs_sample - dynamic_threshold) / (dynamic_threshold * 0.5f));
            sample = static_cast<int16_t>(sample * reduction);
        }
        // Yüksek seviye - dokunma (konuşma)
    }
}
}
