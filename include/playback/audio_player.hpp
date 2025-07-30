#ifndef VOICE_ENGINE_AUDIO_PLAYER_HPP
#define VOICE_ENGINE_AUDIO_PLAYER_HPP

#include "core/non_copyable.hpp"
#include <portaudio.h>
#include <vector>
#include <cstdint>
#include <mutex>
#include <functional>

namespace playback {
    class AudioPlayer : private core::NonCopyable {
    public:
        static constexpr int SAMPLE_RATE = 48000;
        static constexpr int NUM_CHANNELS = 1;
        static constexpr PaSampleFormat FORMAT = paInt16;
        static constexpr int FRAMES_PER_BUFFER = 480;

        AudioPlayer();
        ~AudioPlayer();

        using PlaybackCallback = std::function<void(const std::vector<int16_t>&)>;

        bool start();
        void stop();
        void submit_audio_data(const std::vector<int16_t>& audio_data);
        bool is_playing() const;
        void set_playback_callback(PlaybackCallback cb);

    private:
        static int pa_callback(const void*, void*, unsigned long, const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void*);
        int process(int16_t*, unsigned long);

        PaStream *stream_ = nullptr;
        bool is_playing_ = false;

        std::vector<int16_t> audio_buffer_;
        std::mutex buffer_mutex_;
        PlaybackCallback playback_callback_;
    };
}

#endif