#ifndef VOICE_ENGINE_AUDIO_GAIN_CONTROLLER_HPP
#define VOICE_ENGINE_AUDIO_GAIN_CONTROLLER_HPP

#include <vector>
#include <cstdint>

namespace processing {
    class AudioGainController {
    public:
        explicit AudioGainController(
            float target_level = 8000.0f,
            float max_gain = 3.0f,
            float min_gain = 0.1f,
            float attack_rate = 0.1f,
            float release_rate = 0.01f
        );
        
        void process(std::vector<int16_t>& samples);
        void reset();
        float get_current_gain() const { return current_gain_; }
        
    private:
        float calculate_rms(const std::vector<int16_t>& samples);
        float target_level_;
        float max_gain_;
        float min_gain_;
        float attack_rate_;
        float release_rate_;
        float current_gain_;
        float current_level_;
    };
}

#endif