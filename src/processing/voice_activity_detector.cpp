#include "processing/voice_activity_detector.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>

namespace processing {

VoiceActivityDetector::VoiceActivityDetector(
    float energy_threshold,
    float zero_crossing_threshold,
    int min_speech_frames,
    int min_silence_frames)
    : energy_threshold_(energy_threshold),
      zero_crossing_threshold_(zero_crossing_threshold),
      min_speech_frames_(min_speech_frames),
      min_silence_frames_(min_silence_frames),
      is_voice_active_(false),
      speech_frame_count_(0),
      silence_frame_count_(0),
      avg_energy_(0.0f),
      history_index_(0) {
    
    // Energy history'yi sıfırla
    for (int i = 0; i < 10; ++i) {
        energy_history_[i] = 0.0f;
    }
    
    std::cout << "Voice Activity Detector başlatıldı - Energy Threshold: " << energy_threshold_ << std::endl;
}

float VoiceActivityDetector::calculate_energy(const std::vector<int16_t>& samples) {
    float energy = 0.0f;
    for (const auto& sample : samples) {
        energy += static_cast<float>(sample * sample);
    }
    return energy / samples.size(); // Normalize
}

float VoiceActivityDetector::calculate_zero_crossing_rate(const std::vector<int16_t>& samples) {
    if (samples.size() < 2) return 0.0f;
    
    int zero_crossings = 0;
    for (size_t i = 1; i < samples.size(); ++i) {
        if ((samples[i-1] >= 0 && samples[i] < 0) || 
            (samples[i-1] < 0 && samples[i] >= 0)) {
            zero_crossings++;
        }
    }
    
    return static_cast<float>(zero_crossings) / (samples.size() - 1);
}

bool VoiceActivityDetector::detect_voice(const std::vector<int16_t>& samples) {
    if (samples.empty()) {
        return false;
    }
    
    // Energy ve zero crossing rate hesapla
    float current_energy = calculate_energy(samples);
    float zcr = calculate_zero_crossing_rate(samples);
    
    // Energy history güncelle
    energy_history_[history_index_] = current_energy;
    history_index_ = (history_index_ + 1) % 10;
    
    // Ortalama energy hesapla (son 10 frame)
    avg_energy_ = 0.0f;
    for (int i = 0; i < 10; ++i) {
        avg_energy_ += energy_history_[i];
    }
    avg_energy_ /= 10.0f;
    
    // Adaptif threshold - ortalamanın üstünde olmalı
    float adaptive_threshold = std::max(energy_threshold_, avg_energy_ * 1.5f);
    
    // Voice detection kriterleri
    bool energy_check = current_energy > adaptive_threshold;
    bool zcr_check = zcr > zero_crossing_threshold_ && zcr < 0.8f; // Çok yüksek ZCR = gürültü
    
    bool voice_detected = energy_check && zcr_check;
    
    // State machine - kararlı detection için
    if (voice_detected) {
        speech_frame_count_++;
        silence_frame_count_ = 0;
        
        if (speech_frame_count_ >= min_speech_frames_) {
            is_voice_active_ = true;
        }
    } else {
        silence_frame_count_++;
        speech_frame_count_ = 0;
        
        if (silence_frame_count_ >= min_silence_frames_) {
            is_voice_active_ = false;
        }
    }
    
    return is_voice_active_;
}

void VoiceActivityDetector::reset() {
    is_voice_active_ = false;
    speech_frame_count_ = 0;
    silence_frame_count_ = 0;
    avg_energy_ = 0.0f;
    history_index_ = 0;
    
    for (int i = 0; i < 10; ++i) {
        energy_history_[i] = 0.0f;
    }
}

}