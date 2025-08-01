#ifndef VOICE_ENGINE_UDP_SENDER_HPP
#define VOICE_ENGINE_UDP_SENDER_HPP

#include "core/non_copyable.hpp"
#include "core/packet.hpp"
#include <string>
#include <vector>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <cstring>
#endif

namespace network {
    class UdpSender : private core::NonCopyable {
    public:
        UdpSender();
        ~UdpSender();
        bool connect(const std::string& ip_address, int port);
        void send(const core::Packet& packet);
        void send(const std::vector<core::Packet>& packets);
    private:
#ifdef _WIN32
        SOCKET socket_ = INVALID_SOCKET;
        WSADATA wsa_data_{};
#else
        int socket_ = -1;
#endif
        sockaddr_in server_address_{};
    };
}

#endif