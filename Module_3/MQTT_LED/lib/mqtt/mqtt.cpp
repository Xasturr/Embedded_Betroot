#include "esp_log.h"

#include "mqtt.h"

esp_mqtt_client_handle_t mqttClient = NULL;

static const char TAG[] = "mqtt";

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
	ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32, base, event_id);

	esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;

	esp_mqtt_client_handle_t client = event->client;

	switch ((esp_mqtt_event_id_t)event_id)
	{
	case MQTT_EVENT_CONNECTED:
		ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");

		esp_mqtt_client_subscribe(client, MQTT_COMMANDS, 0);

		esp_mqtt_client_publish(client, MQTT_STATUS, getled_state() ? "ON" : "OFF", 0, 0, 0);
		break;

	case MQTT_EVENT_DISCONNECTED:
		ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
		break;

	case MQTT_EVENT_SUBSCRIBED:
		ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d, return code=0x%02x ", event->msg_id, (uint8_t)*event->data);
		break;

	case MQTT_EVENT_UNSUBSCRIBED:
		ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
		break;

	case MQTT_EVENT_PUBLISHED:
		ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
		break;

	case MQTT_EVENT_DATA:
	{
		ESP_LOGI(TAG, "MQTT_EVENT_DATA");
		printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
		printf("DATA=%.*s\r\n", event->data_len, event->data);

		char topic[event->topic_len + 1];
		memcpy(topic, event->topic, event->topic_len);
		topic[event->topic_len] = '\0';

		char data[event->data_len + 1];
		memcpy(data, event->data, event->data_len);
		data[event->data_len] = '\0';

		handle_mqtt_message(topic, data);

		break;
	}
	case MQTT_EVENT_ERROR:
		ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
		if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT)
		{
			ESP_LOGI(TAG, "Last error code reported from esp-tls: 0x%x", event->error_handle->esp_tls_last_esp_err);
			ESP_LOGI(TAG, "Last tls stack error number: 0x%x", event->error_handle->esp_tls_stack_err);
			ESP_LOGI(TAG, "Last captured errno : %d (%s)", event->error_handle->esp_transport_sock_errno,
					 strerror(event->error_handle->esp_transport_sock_errno));
		}
		else if (event->error_handle->error_type == MQTT_ERROR_TYPE_CONNECTION_REFUSED)
		{
			ESP_LOGI(TAG, "Connection refused error: 0x%x", event->error_handle->connect_return_code);
		}
		else
		{
			ESP_LOGW(TAG, "Unknown error type: 0x%x", event->error_handle->error_type);
		}
		break;

	default:
		ESP_LOGI(TAG, "Other event id:%d", event->event_id);
		break;
	}
}

void mqtt_start(void)
{
	esp_mqtt_client_config_t mqtt_cfg = {};

	mqtt_cfg.broker.address.uri = "mqtt://broker.hivemq.com";
	mqtt_cfg.broker.address.port = 1883;

	esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
	mqttClient = client;
	/* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
	esp_mqtt_client_register_event(client, (esp_mqtt_event_id_t)ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
	esp_mqtt_client_start(client);
}

esp_mqtt_client_handle_t get_mqtt_client()
{
	return mqttClient;
}