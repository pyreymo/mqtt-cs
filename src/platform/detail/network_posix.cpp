#include "network_posix.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdint>
#include <cstring>
#include <iostream>

namespace MqttPlatform {
namespace detail {

PosixNetworkImpl::PosixNetworkImpl() : sockfd(-1) {}

PosixNetworkImpl::~PosixNetworkImpl() { disconnect(); }

bool PosixNetworkImpl::connect(const std::string& host, int port) {
    if (sockfd != -1) {
        return false;
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        return false;
    }

    struct sockaddr_in broker_addr;
    memset(&broker_addr, 0, sizeof(broker_addr));
    broker_addr.sin_family = AF_INET;
    broker_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, host.c_str(), &broker_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sockfd);
        sockfd = -1;
        return false;
    }

    if (::connect(sockfd, (struct sockaddr*)&broker_addr, sizeof(broker_addr)) < 0) {
        perror("connect");
        close(sockfd);
        sockfd = -1;
        return false;
    }

    if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL, 0) | O_NONBLOCK) == -1) {
        perror("fcntl");
        disconnect();
        return false;
    }

    std::cout << "[POSIX] Network connection established to " << host << ":" << port << std::endl;
    return true;
}

void PosixNetworkImpl::disconnect() {
    if (sockfd != -1) {
        close(sockfd);
        sockfd = -1;
    }
}

mqtt_pal_socket_handle PosixNetworkImpl::get_socket_handle() const { return sockfd; }

}  // namespace detail
}  // namespace MqttPlatform