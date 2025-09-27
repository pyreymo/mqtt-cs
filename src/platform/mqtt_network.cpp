#include "mqtt_network.h"

#if defined(__linux__) || defined(__APPLE__)
#include "detail/network_posix.h"
#elif defined(_WIN32)
#include "detail/network_win.h"
#else
#error "Unsupported platform"
#endif

namespace MqttPlatform {

MqttNetwork::MqttNetwork() {
#if defined(__linux__) || defined(__APPLE__)
    pimpl = std::make_unique<detail::PosixNetworkImpl>();
#elif defined(_WIN32)
    pimpl = std::make_unique<detail::WinsockNetworkImpl>();
#endif
}

MqttNetwork::~MqttNetwork() = default;

MqttNetwork::MqttNetwork(MqttNetwork&&) noexcept = default;
MqttNetwork& MqttNetwork::operator=(MqttNetwork&&) noexcept = default;

bool MqttNetwork::connect(const std::string& host, int port) { return pimpl->connect(host, port); }

void MqttNetwork::disconnect() { pimpl->disconnect(); }

mqtt_pal_socket_handle MqttNetwork::get_socket_handle() const { return pimpl->get_socket_handle(); }

}  // namespace MqttPlatform