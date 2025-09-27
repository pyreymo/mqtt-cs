#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include "platform/mqtt_network.h"

extern "C" {
#include "mqtt.h"
#include "mqtt_pal.h"
}

const char* broker_address = "127.0.0.1";
const int broker_port = 1883;
const char* topic = "/test/topic";
const char* client_id = "cpp-publisher-123";

struct mqtt_client client;
uint8_t sendbuf[2048];
uint8_t recvbuf[2048];

void publish_callback(void** unused, struct mqtt_response_publish* published) {}

int main() {
    MqttPlatform::MqttNetwork network;
    if (!network.connect(broker_address, broker_port)) {
        std::cerr << "Failed to establish network connection." << std::endl;
        return 1;
    }

    mqtt_init(&client, network.get_socket_handle(), sendbuf, sizeof(sendbuf), recvbuf, sizeof(recvbuf),
              publish_callback);

    mqtt_connect(&client, client_id, NULL, NULL, 0, NULL, NULL, 0, 400);

    auto start_time = std::chrono::steady_clock::now();
    while (client.error != MQTT_OK) {
        mqtt_sync(&client);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - start_time).count() > 5) {
            std::cerr << "Error: Connection timeout." << std::endl;
            return 1;
        }
    }
    std::cout << "Successfully connected to MQTT broker." << std::endl;

    for (int i = 0; i < 5; ++i) {
        if (client.error != MQTT_OK) {
            std::cerr << "Client disconnected unexpectedly." << std::endl;
            break;
        }

        std::string message = "Hello from C++ publisher! Count: " + std::to_string(i);

        mqtt_publish(&client, topic, message.c_str(), message.length(), MQTT_PUBLISH_QOS_0);

        if (mqtt_sync(&client) != MQTT_OK) {
            std::cerr << "Error during publish: " << mqtt_error_str(client.error) << std::endl;
            break;
        }

        std::cout << "Published: \"" << message << "\"" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    if (client.error == MQTT_OK) {
        std::cout << "Disconnecting..." << std::endl;
        mqtt_disconnect(&client);
        mqtt_sync(&client);
    }

    std::cout << "Publisher finished." << std::endl;
    return 0;
}