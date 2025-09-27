#include <chrono>
#include <csignal>
#include <iostream>
#include <thread>

#include "platform/mqtt_network.h"

extern "C" {
#include "mqtt.h"
#include "mqtt_pal.h"
}

const char* broker_address = "127.0.0.1";
const int broker_port = 1883;
const char* topic = "/test/topic";
const char* client_id = "cpp-subscriber-456";

struct mqtt_client client;
uint8_t sendbuf[2048];
uint8_t recvbuf[2048];

volatile bool running = true;

void handle_signal(int s) {
    std::cout << "\nCaught signal " << s << ", shutting down." << std::endl;
    running = false;
}

void publish_callback(void** unused, struct mqtt_response_publish* published) {
    printf("Received publish:\n");
    printf("  Topic: %.*s\n", (int)published->topic_name_size, (const char*)published->topic_name);
    printf("  Payload: %.*s\n\n", (int)published->application_message_size,
           (const char*)published->application_message);
}

int main() {
    signal(SIGINT, handle_signal);

    MqttPlatform::MqttNetwork network;
    if (!network.connect(broker_address, broker_port)) {
        std::cerr << "Failed to establish network connection." << std::endl;
        return 1;
    }

    mqtt_init(&client, network.get_socket_handle(), sendbuf, sizeof(sendbuf), recvbuf, sizeof(recvbuf),
              publish_callback);

    mqtt_connect(&client, client_id, NULL, NULL, 0, NULL, NULL, 0, 400);

    auto start_time = std::chrono::steady_clock::now();
    while (client.error != MQTT_OK && running) {
        mqtt_sync(&client);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - start_time).count() > 5) {
            std::cerr << "Error: Connection timeout." << std::endl;
            return 1;
        }
    }
    if (!running) return 1;
    std::cout << "Successfully connected to MQTT broker." << std::endl;

    mqtt_subscribe(&client, topic, 0);
    std::cout << "Subscribed to topic: " << topic << std::endl;
    std::cout << "Waiting for messages... (Press Ctrl+C to exit)" << std::endl;

    while (running) {
        MQTTErrors err = mqtt_sync(&client);
        if (err != MQTT_OK) {
            std::cerr << "Error in sync: " << mqtt_error_str(err) << std::endl;
            break;
        }

        if (client.error != MQTT_OK) {
            std::cout << "Disconnected from broker." << std::endl;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if (client.error == MQTT_OK) {
        std::cout << "Disconnecting..." << std::endl;
        mqtt_disconnect(&client);
        mqtt_sync(&client);
    }

    network.disconnect();
    std::cout << "Subscriber finished." << std::endl;
    return 0;
}