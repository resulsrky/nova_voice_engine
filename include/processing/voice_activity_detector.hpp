#ifndef VOICE_ENGINE_VOICE_ACTIVITY_DETECTOR_HPP
#define VOICE_ENGINE_VOICE_ACTIVITY_DETECTOR_HPP

#include <vector>
#include <cstdint>

namespace processing {
    class VoiceActivityDetector {
    public:
        explicit VoiceActivityDetector(
            float energy_threshold = 2000.0f,
            float zero_crossing_threshold = 0.3f,
            int min_speech_frames = 3,
            int min_silence_frames = 5
        );
        
        bool detect_voice(const std::vector<int16_t>& samples);
        bool is_voice_active() const { return is_voice_active_; }
        void reset();
        
    private:
        float calculate_energy(const std::vector<int16_t>& samples);
        float calculate_zero_crossing_rate(const std::vector<int16_t>& samples);
        
        float energy_threshold_;
        float zero_crossing_threshold_;
        int min_speech_frames_;
        int min_silence_frames_;
        
        bool is_voice_active_;
        int speech_frame_count_;
        int silence_frame_count_;
        float avg_energy_;
        float energy_history_[10];
        int history_index_;
    };
}

#endif