#include "network/udp_receiver.hpp"
#include <iostream>
#include <stdexcept>
#include <vector>

namespace network {
UdpReceiver::UdpReceiver() {
#ifdef _WIN32
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data_) != 0) { throw std::runtime_error("WSAStartup basarisiz oldu."); }
#endif
}

UdpReceiver::~UdpReceiver() {
    stop();
#ifdef _WIN32
    WSACleanup();
#endif
}

bool UdpReceiver::start(int port, OnPacketReceived callback) {
    if (is_running_) { return true; }
    on_packet_received_ = std::move(callback);
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
    int recv_buffer_size = 65536;  // 64KB receive buffer
    if (setsockopt(socket_, SOL_SOCKET, SO_RCVBUF, &recv_buffer_size, sizeof(recv_buffer_size)) < 0) {
        std::cerr << "UYARI: Receive buffer size ayarlanamadi." << std::endl;
    }
    
    // Socket timeout ayarları
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 100000; // 100ms timeout
    if (setsockopt(socket_, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        std::cerr << "UYARI: Socket timeout ayarlanamadi." << std::endl;
    }
#endif

    sockaddr_in server_address{};
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port);
    if (bind(socket_, (const sockaddr*)&server_address, sizeof(server_address)) < 0) {
        std::cerr << "HATA: Socket " << port << " portuna bind edilemedi." << std::endl;
        return false;
    }
    is_running_ = true;
    receiver_thread_ = std::thread(&UdpReceiver::receive_loop, this);
    std::cout << "Receiver " << port << " portunu dinlemeye basladi (Optimized)." << std::endl;
    return true;
}

void UdpReceiver::stop() {
    is_running_ = false;
    if (socket_ != -1) {
#ifdef _WIN32
        closesocket(socket_);
        socket_ = INVALID_SOCKET;
#else
        shutdown(socket_, SHUT_RDWR);
        close(socket_);
        socket_ = -1;
#endif
    }
    if (receiver_thread_.joinable()) {
        receiver_thread_.join();
    }
}

void UdpReceiver::receive_loop() {
    std::vector<uint8_t> buffer(2048);
    sockaddr_in client_address{};
    socklen_t client_len = sizeof(client_address);
    while (is_running_) {
        int bytes_received = recvfrom(socket_, reinterpret_cast<char*>(buffer.data()), buffer.size(), 0, (sockaddr*)&client_address, &client_len);
        if (bytes_received > 0) {
            std::vector<uint8_t> received_data(buffer.begin(), buffer.begin() + bytes_received);
            if (on_packet_received_) {
                on_packet_received_(core::Packet::from_bytes(received_data));
            }
        }
    }
    std::cout << "Receiver dongusu sonlandi." << std::endl;
}
}