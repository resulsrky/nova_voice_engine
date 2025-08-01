#ifndef VOICE_ENGINE_ECHO_CANCELLER_HPP
#define VOICE_ENGINE_ECHO_CANCELLER_HPP

#include <vector>
#include <cstdint>
#include <cstddef>
#include <mutex>

namespace processing {
    class EchoCanceller {
    public:
        explicit EchoCanceller(size_t max_delay_samples = 48000);
        void on_playback(const std::vector<int16_t>& samples);
        void process(std::vector<int16_t>& capture);
    private:
        std::vector<int16_t> echo_buffer_;
        size_t max_delay_;
        std::mutex mutex_;
        float echo_suppression_factor_;
        float adaptive_threshold_;
        float learning_rate_;
    };
}

#endif
