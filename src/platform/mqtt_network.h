#ifndef MQTT_NETWORK_H
#define MQTT_NETWORK_H

#include <memory>
#include <string>
extern "C" {
#include "mqtt_pal.h"
}

namespace MqttPlatform {
namespace detail {
class NetworkImpl;
}

class MqttNetwork {
   public:
    MqttNetwork();
    ~MqttNetwork();

    MqttNetwork(MqttNetwork&&) noexcept;
    MqttNetwork& operator=(MqttNetwork&&) noexcept;
    MqttNetwork(const MqttNetwork&) = delete;
    MqttNetwork& operator=(const MqttNetwork&) = delete;

    bool connect(const std::string& host, int port);
    void disconnect();
    mqtt_pal_socket_handle get_socket_handle() const;

   private:
    std::unique_ptr<detail::NetworkImpl> pimpl;
};

}  // namespace MqttPlatform
#endif  // MQTT_NETWORK_H