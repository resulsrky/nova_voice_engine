#ifndef VOICE_ENGINE_NOISE_SUPPRESSOR_HPP
#define VOICE_ENGINE_NOISE_SUPPRESSOR_HPP

#include <vector>
#include <cstdint>

namespace processing {
    class NoiseSuppressor {
    public:
        explicit NoiseSuppressor(int16_t initial_threshold = 500, float alpha = 0.95f);
        void process(std::vector<int16_t>& samples);
    private:
        float threshold_;
        float noise_level_;
        float alpha_;
        int silence_counter_;
        float noise_gate_threshold_;
        float noise_reduction_factor_;
    };
}

#endif
