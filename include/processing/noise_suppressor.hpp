#ifndef VOICE_ENGINE_NOISE_SUPPRESSOR_HPP
#define VOICE_ENGINE_NOISE_SUPPRESSOR_HPP

#include <vector>
#include <cstdint>

namespace processing {
    class NoiseSuppressor {
    public:
        explicit NoiseSuppressor(int16_t threshold = 500);
        void process(std::vector<int16_t>& samples) const;
    private:
        int16_t threshold_;
    };
}

#endif
