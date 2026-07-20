#include <stdio.h>
#include <time.h>
#include <algorithm>
#include "driver/gpio.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/ledc.h"
#include "mqtt.h"
#include "wifi_setup.h"

static const char TAG[] = "main";
static bool led_state = false;
static int32_t current_brightness = 0;
constexpr int32_t min_brightness = 0;
constexpr int32_t max_brightness = 255;
constexpr int32_t brightness_delta = 64;
constexpr gpio_num_t LED_GPIO = GPIO_NUM_16;

void led_set_brightness(uint8_t brightness)
{
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, brightness);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);

	current_brightness = std::clamp(brightness, min_brightness, max_brightness)
}

void handle_mqtt_message(const char *topic, const char *data)
{
	if (topic == NULL || data == NULL)
	{
		return;
	}

	if (strcmp(topic, MQTT_COMMANDS) != 0)
	{
		return;
	}

	esp_mqtt_client_handle_t client = get_mqtt_client();

	if (client == NULL)
	{
		ESP_LOGE(TAG, "MQTT client is NULL");
		return;
	}

	if (strcmp(data, "ON") == 0)
	{
		ESP_LOGI(TAG, "Command: LED ON");

		if (led_state)
		{
			return;
		}

		led_set_brightness(max_brightness);
		led_state = true;
	}
	else if (strcmp(data, "OFF") == 0)
	{
		ESP_LOGI(TAG, "Command: LED OFF");

		led_set_brightness(min_brightness);
		led_state = false;
	}
	else if (strcmp(data, "STATUS") == 0)
	{
		if (esp_mqtt_client_publish(client, MQTT_STATUS, led_state ? "ON" : "OFF", 0, 0, 0) < 0)
		{
			ESP_LOGE(TAG, "Failed to publish status");
		}
		else
		{
			ESP_LOGI(TAG, "Status sent");
		}
	}
	else if (strcmp(data, "BRIGHT_UP") == 0)
	{
		ESP_LOGW(TAG, "BRIGHT_UP");

		if (led_state)
		{
			led_set_brightness(current_brightness + brightness_delta);
		}
	}
	else if (strcmp(data, "BRIGHT_DOWN") == 0)
	{
		ESP_LOGW(TAG, "BRIGHT_DOWN");

		if (led_state)
		{
			led_set_brightness(current_brightness - brightness_delta);
		}
	}
	else
	{
		ESP_LOGW(TAG, "Unknown command: %s", data);
	}
}

extern "C" void app_main(void)
{
	// Initialize NVS
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);
	ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");

	ledc_timer_config_t timer = {};

	timer.speed_mode = LEDC_LOW_SPEED_MODE;
	timer.timer_num = LEDC_TIMER_0;
	timer.duty_resolution = LEDC_TIMER_8_BIT;
	timer.freq_hz = 5000;
	timer.clk_cfg = LEDC_AUTO_CLK;

	ESP_ERROR_CHECK(ledc_timer_config(&timer));

    ledc_channel_config_t channel = {
        .gpio_num = LED_GPIO,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0,
    };
    ledc_channel_config(&channel);

	wifi_init_sta();

	if (get_isConnected())
	{
		mqtt_start();
	}

	while (1)
	{
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}
