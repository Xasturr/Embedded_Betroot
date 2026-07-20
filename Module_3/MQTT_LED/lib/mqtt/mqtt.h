#pragma once

#include "mqtt_client.h"

#define MQTT_BROKER_URI "mqtt://broker.hivemq.com:1883"
#define MQTT_TOPIC "esp32s3/test"

inline constexpr char MQTT_COMMANDS[] = "esp32s3/commands";
inline constexpr char MQTT_STATUS[] = "esp32s3/status";

bool getled_state();

void mqtt_start();

typedef void (*mqtt_message_handler_t)(const char *topic, const char *data);

void mqtt_app_start(void);

esp_mqtt_client_handle_t mqtt_get_client(void);

void handle_mqtt_message(const char *topic, const char *data);

esp_mqtt_client_handle_t get_mqtt_client();
