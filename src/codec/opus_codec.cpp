#include "codec/opus_codec.hpp"
#include <iostream>
#include <stdexcept>

namespace codec {
    OpusCodec::OpusCodec(int sample_rate, int channels)
        : sample_rate_(sample_rate), channels_(channels), frame_size_(sample_rate / 100) {
        int error;
        encoder_ = opus_encoder_create(sample_rate_, channels_, OPUS_APPLICATION_VOIP, &error);
        if (error != OPUS_OK) { throw std::runtime_error("Opus encoder oluşturulamadı: " + std::string(opus_strerror(error))); }
        
        decoder_ = opus_decoder_create(sample_rate_, channels_, &error);
        if (error != OPUS_OK) { opus_encoder_destroy(encoder_); throw std::runtime_error("Opus decoder oluşturulamadı: " + std::string(opus_strerror(error))); }
        
        // Encoder optimizasyonları
        opus_encoder_ctl(encoder_, OPUS_SET_BITRATE(64000));  // 64kbps optimal voice için
        opus_encoder_ctl(encoder_, OPUS_SET_VBR(1));         // Variable bitrate
        opus_encoder_ctl(encoder_, OPUS_SET_VBR_CONSTRAINT(1)); // Constrained VBR
        opus_encoder_ctl(encoder_, OPUS_SET_COMPLEXITY(5));   // Orta seviye complexity
        opus_encoder_ctl(encoder_, OPUS_SET_SIGNAL(OPUS_SIGNAL_VOICE)); // Voice optimizasyonu
        opus_encoder_ctl(encoder_, OPUS_SET_DTX(1));         // Discontinuous transmission
        opus_encoder_ctl(encoder_, OPUS_SET_PACKET_LOSS_PERC(5)); // %5 packet loss tolerance
        
        std::cout << "Opus codec başarıyla başlatıldı (Optimized)." << std::endl;
    }

    OpusCodec::~OpusCodec() {
        if (encoder_) { opus_encoder_destroy(encoder_); }
        if (decoder_) { opus_decoder_destroy(decoder_); }
        std::cout << "Opus codec temizlendi." << std::endl;
    }

    std::vector<uint8_t> OpusCodec::encode(const std::vector<int16_t>& pcm_data) {
        if (!encoder_ || pcm_data.empty()) { return {}; }
        
        // Frame size kontrolü - Opus 10ms frameler bekler
        if (static_cast<int>(pcm_data.size()) != frame_size_ * channels_) {
            std::cerr << "UYARI: PCM data boyutu beklenen frame size ile uyuşmuyor. "
                      << "Beklenen: " << (frame_size_ * channels_) 
                      << ", Gelen: " << pcm_data.size() << std::endl;
            return {};
        }
        
        std::vector<uint8_t> compressed_data(4000); // Max Opus packet size
        opus_int32 result = opus_encode(encoder_, pcm_data.data(), frame_size_, 
                                       compressed_data.data(), compressed_data.size());
        
        if (result < 0) { 
            std::cerr << "Opus encode hatası: " << opus_strerror(result) << std::endl; 
            return {}; 
        }
        
        compressed_data.resize(result);
        return compressed_data;
    }

    std::vector<int16_t> OpusCodec::decode(const std::vector<uint8_t>& encoded_data) {
        if (!decoder_ || encoded_data.empty()) { return {}; }
        std::vector<int16_t> decoded_data(frame_size_ * channels_ * 6);
        int decoded_samples = opus_decode(decoder_, encoded_data.data(), encoded_data.size(), decoded_data.data(), frame_size_ * 6, 0);
        if (decoded_samples < 0) { std::cerr << "Opus decode hatası: " << opus_strerror(decoded_samples) << std::endl; return {}; }
        decoded_data.resize(decoded_samples * channels_);
        return decoded_data;
    }
}