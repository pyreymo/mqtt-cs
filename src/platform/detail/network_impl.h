#ifndef MQTT_NETWORK_IMPL_H
#define MQTT_NETWORK_IMPL_H

#include <string>
extern "C" {
#include "mqtt_pal.h"
}

namespace MqttPlatform {
namespace detail {

class NetworkImpl {
   public:
    virtual ~NetworkImpl() = default;

    virtual bool connect(const std::string& host, int port) = 0;
    virtual void disconnect() = 0;
    virtual mqtt_pal_socket_handle get_socket_handle() const = 0;
};

}  // namespace detail
}  // namespace MqttPlatform

#endif  // MQTT_NETWORK_IMPL_H