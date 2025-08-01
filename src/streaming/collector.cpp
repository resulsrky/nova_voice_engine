#include "streaming/collector.hpp"
#include <iostream>
#include <algorithm>
#include <map>

namespace streaming {

class Collector::Impl {
public:
    struct PacketBuffer {
        std::map<uint32_t, core::Packet> packets;
        uint32_t expected_sequence = 0;
        bool is_collecting = false;
        size_t total_data_size = 0;
        
        void reset() {
            packets.clear();
            expected_sequence = 0;
            is_collecting = false;
            total_data_size = 0;
        }
    };
    
    PacketBuffer buffer_;
    static constexpr size_t MAX_PACKET_BUFFER = 100;
    static constexpr uint32_t MAX_SEQUENCE_GAP = 10;
    
    void collect(const core::Packet& packet, const OnDataCollected& callback) {
        if (!callback) return;
        
        // İlk paket geldiğinde buffer'ı başlat
        if (!buffer_.is_collecting) {
            buffer_.expected_sequence = packet.sequence_number;
            buffer_.is_collecting = true;
        }
        
        // Paket buffer'a ekle
        buffer_.packets[packet.sequence_number] = packet;
        buffer_.total_data_size += packet.data.size();
        
        // Buffer overflow kontrolü
        if (buffer_.packets.size() > MAX_PACKET_BUFFER) {
            std::cout << "UYARI: Packet buffer overflow, buffer temizleniyor." << std::endl;
            buffer_.reset();
            return;
        }
        
        // Sıralı paketleri işle
        process_sequential_packets(callback);
        
        // Eski paketleri temizle
        cleanup_old_packets();
    }
    
private:
    void process_sequential_packets(const OnDataCollected& callback) {
        while (buffer_.packets.find(buffer_.expected_sequence) != buffer_.packets.end()) {
            const auto& packet = buffer_.packets[buffer_.expected_sequence];
            
            // Tek paket halinde data'yı gönder (basit approach)
            if (!packet.data.empty()) {
                callback(packet.data);
            }
            
            // İşlenen paketi buffer'dan kaldır
            buffer_.packets.erase(buffer_.expected_sequence);
            buffer_.expected_sequence++;
        }
    }
    
    void cleanup_old_packets() {
        // Çok eski paketleri temizle (sequence gap çok büyükse)
        auto it = buffer_.packets.begin();
        while (it != buffer_.packets.end()) {
            if (it->first < buffer_.expected_sequence - MAX_SEQUENCE_GAP) {
                std::cout << "UYARI: Eski paket temizleniyor: " << it->first << std::endl;
                it = buffer_.packets.erase(it);
            } else {
                ++it;
            }
        }
    }
};

Collector::Collector() : impl_(std::make_unique<Impl>()) {}

Collector::~Collector() = default;

void Collector::collect(const core::Packet& packet, const OnDataCollected& callback) {
    impl_->collect(packet, callback);
}

void Collector::reset() {
    impl_->buffer_.reset();
}

}
