#include "playback/audio_player.hpp"
#include <iostream>
#include <stdexcept>
#include <cstring>


namespace playback {

AudioPlayer::AudioPlayer() = default;
AudioPlayer::~AudioPlayer() { stop(); }

bool AudioPlayer::start() {
    if (is_playing_) { return true; }

    PaStreamParameters output_parameters;
    output_parameters.device = Pa_GetDefaultOutputDevice();
    if (output_parameters.device == paNoDevice) { std::cerr << "HATA: Varsayılan çıkış aygıtı bulunamadı." << std::endl; return false; }
    output_parameters.channelCount = NUM_CHANNELS;
    output_parameters.sampleFormat = FORMAT;
    output_parameters.suggestedLatency = Pa_GetDeviceInfo(output_parameters.device)->defaultLowOutputLatency;
    output_parameters.hostApiSpecificStreamInfo = nullptr;

    PaError err = Pa_OpenStream(&stream_, nullptr, &output_parameters, SAMPLE_RATE, FRAMES_PER_BUFFER, paClipOff, &AudioPlayer::pa_callback, this);
    if (err != paNoError) { std::cerr << "PortAudio HATA: Pa_OpenStream() - " << Pa_GetErrorText(err) << std::endl; return false; }

    err = Pa_StartStream(stream_);
    if (err != paNoError) { std::cerr << "PortAudio HATA: Pa_StartStream() - " << Pa_GetErrorText(err) << std::endl; Pa_CloseStream(stream_); stream_ = nullptr; return false; }

    is_playing_ = true;
    std::cout << "Ses oynatıcı başlatıldı." << std::endl;
    return true;
}

void AudioPlayer::stop() {
    if (!is_playing_ || !stream_) { return; }
    Pa_StopStream(stream_);
    Pa_CloseStream(stream_);
    stream_ = nullptr;
    is_playing_ = false;
    std::cout << "Ses oynatıcı durduruldu." << std::endl;
}

void AudioPlayer::submit_audio_data(const std::vector<int16_t>& audio_data) {
    std::lock_guard<std::mutex> lock(buffer_mutex_);
    audio_buffer_.insert(audio_buffer_.end(), audio_data.begin(), audio_data.end());
}

bool AudioPlayer::is_playing() const {
    return is_playing_;
}

void AudioPlayer::set_playback_callback(PlaybackCallback cb) {
    playback_callback_ = std::move(cb);
}

int AudioPlayer::pa_callback(const void*, void* o, unsigned long f, const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void* u) {
    return static_cast<AudioPlayer*>(u)->process(static_cast<int16_t*>(o), f);
}

int AudioPlayer::process(int16_t* outputBuffer, unsigned long framesPerBuffer) {
    std::lock_guard<std::mutex> lock(buffer_mutex_);
    const size_t samples_needed = framesPerBuffer * NUM_CHANNELS;
    if (audio_buffer_.size() >= samples_needed) {
        std::memcpy(outputBuffer, audio_buffer_.data(), samples_needed * sizeof(int16_t));
        audio_buffer_.erase(audio_buffer_.begin(), audio_buffer_.begin() + samples_needed);
    } else {
        std::memset(outputBuffer, 0, samples_needed * sizeof(int16_t));
    }
    if (playback_callback_) {
        playback_callback_(std::vector<int16_t>(outputBuffer, outputBuffer + samples_needed));
    }
    return paContinue;
}
}