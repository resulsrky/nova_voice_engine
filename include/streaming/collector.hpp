#ifndef VOICE_ENGINE_COLLECTOR_HPP
#define VOICE_ENGINE_COLLECTOR_HPP

#include "core/packet.hpp"
#include <vector>
#include <functional>
#include <memory>

namespace streaming {
    class Collector {
    public:
        using OnDataCollected = std::function<void(const std::vector<uint8_t>&)>;

        Collector();
        ~Collector();
        
        void collect(const core::Packet& packet, const OnDataCollected& callback);
        void reset();
        
    private:
        class Impl;
        std::unique_ptr<Impl> impl_;
    };
}

#endif