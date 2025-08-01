#include "network/udp_sender.hpp"
#include <iostream>
#include <stdexcept>

namespace network {
    UdpSender::UdpSender() {
#ifdef _WIN32
        if (WSAStartup(MAKEWORD(2, 2), &wsa_data_) != 0) { throw std::runtime_error("WSAStartup basarisiz oldu."); }
#endif
    }

    UdpSender::~UdpSender() {
        if (socket_ != -1) {
#ifdef _WIN32
            closesocket(socket_);
            WSACleanup();
#else
            close(socket_);
#endif
        }
    }

    bool UdpSender::connect(const std::string& ip_address, int port) {
        socket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
#ifdef _WIN32
        if (socket_ == INVALID_SOCKET) {
#else
        if (socket_ < 0) {
#endif
            std::cerr << "HATA: Socket olusturulamadi." << std::endl;
            return false;
        }

        // Socket optimizasyonları
#ifndef _WIN32
        int send_buffer_size = 65536;  // 64KB send buffer
        if (setsockopt(socket_, SOL_SOCKET, SO_SNDBUF, &send_buffer_size, sizeof(send_buffer_size)) < 0) {
            std::cerr << "UYARI: Send buffer size ayarlanamadi." << std::endl;
        }
        
        // Non-blocking socket
        int flags = fcntl(socket_, F_GETFL, 0);
        if (flags >= 0) {
            fcntl(socket_, F_SETFL, flags | O_NONBLOCK);
        }
#endif

        server_address_.sin_family = AF_INET;
        server_address_.sin_port = htons(port);
        int ip_result = inet_pton(AF_INET, ip_address.c_str(), &server_address_.sin_addr);
        if (ip_result <= 0) {
            std::cerr << "HATA: Gecersiz IP adresi: " << ip_address << std::endl;
            
#ifdef _WIN32
            closesocket(socket_);
            socket_ = INVALID_SOCKET;
#else
            close(socket_);
            socket_ = -1;
#endif
            return false;
        }
        std::cout << "Sender " << ip_address << ":" << port << " adresine baglanmaya hazir (Optimized)." << std::endl;
        return true;
    }

    void UdpSender::send(const core::Packet& packet) {
        auto bytes = packet.to_bytes();
        ssize_t result = sendto(socket_, reinterpret_cast<const char*>(bytes.data()), bytes.size(), 
                               0, (const sockaddr*)&server_address_, sizeof(server_address_));
        
        if (result < 0) {
#ifdef _WIN32
            int error = WSAGetLastError();
            if (error != WSAEWOULDBLOCK) {
                std::cerr << "UDP Send hatası: " << error << std::endl;
            }
#else
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                std::cerr << "UDP Send hatası: " << strerror(errno) << std::endl;
            }
#endif
        }
    }

    void UdpSender::send(const std::vector<core::Packet>& packets) {
        for (const auto& packet : packets) { send(packet); }
    }
}