#ifndef MQTT_NETWORK_POSIX_H
#define MQTT_NETWORK_POSIX_H

#include "network_impl.h"

namespace MqttPlatform {
namespace detail {

class PosixNetworkImpl : public NetworkImpl {
   public:
    PosixNetworkImpl();
    ~PosixNetworkImpl() override;

    bool connect(const std::string& host, int port) override;
    void disconnect() override;
    mqtt_pal_socket_handle get_socket_handle() const override;

   private:
    int sockfd;
};

}  // namespace detail
}  // namespace MqttPlatform

#endif  // MQTT_NETWORK_POSIX_H