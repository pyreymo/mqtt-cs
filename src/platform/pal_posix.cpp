#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#include <cerrno>

extern "C" {
#include "mqtt_pal.h"
#include "mqtt.h"
}

ssize_t mqtt_pal_sendall(mqtt_pal_socket_handle fd, const void* buf, size_t len, int flags) {
    size_t sent = 0;
    int sockfd = (int)(intptr_t)fd;
    while (sent < len) {
        ssize_t s = send(sockfd, (const char*)buf + sent, len - sent, flags);
        if (s < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) continue;
            return MQTT_ERROR_SOCKET_ERROR;
        }
        sent += s;
    }
    return sent;
}

ssize_t mqtt_pal_recvall(mqtt_pal_socket_handle fd, void* buf, size_t len, int flags) {
    int sockfd = (int)(intptr_t)fd;
    ssize_t n = recv(sockfd, buf, len, flags);
    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0;
        }
        return MQTT_ERROR_SOCKET_ERROR;
    }
    return n;
}

time_t mqtt_pal_get_time(void) { return time(NULL); }