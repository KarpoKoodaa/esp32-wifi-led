#include <stdio.h>
#include "freertos/freeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

#include "esp_log.h"

#include "driver/gpio.h"
#include "led.h"

static const char* TAG = "LED";

void initialize_leds(void)
{
    //int i = 0;

    // for (i = LED_1_GPIO; i < NUM_OF_LEDS; i++) {
    //     gpio_reset_pin(i);
    //     gpio_set_direction(i, GPIO_MODE_INPUT_OUTPUT);
    //     ESP_LOGI(TAG, "LED: %d initialized", i);

    // }
    gpio_reset_pin(LED_1_GPIO);
    gpio_set_direction(LED_1_GPIO, GPIO_MODE_INPUT_OUTPUT);
    ESP_LOGI(TAG, "LED: %d initialized", LED_1_GPIO);

    gpio_reset_pin(LED_2_GPIO);
    gpio_set_direction(LED_2_GPIO, GPIO_MODE_INPUT_OUTPUT);
    ESP_LOGI(TAG, "LED: %d initialized", LED_2_GPIO);

    gpio_reset_pin(LED_3_GPIO);
    gpio_set_direction(LED_3_GPIO, GPIO_MODE_INPUT_OUTPUT);
    ESP_LOGI(TAG, "LED: %d initialized", LED_3_GPIO);

    gpio_reset_pin(LED_4_GPIO);
    gpio_set_direction(LED_4_GPIO, GPIO_MODE_INPUT_OUTPUT);
    ESP_LOGI(TAG, "LED: %d initialized", LED_4_GPIO);

}

void toggle_led_task( void * pvParameters)
{
    int led_number = (int)pvParameters;

    while(1) {

        gpio_set_level(led_number, !(gpio_get_level(led_number))); 
        ESP_LOGI(TAG, "%d state %d", led_number, gpio_get_level(led_number));
        vTaskDelay(100 / portTICK_PERIOD_MS);
        vTaskDelete(NULL);
    }
}

void led_on_task (void * pvParameters)
{
    int led_number = (int)pvParameters;
    
    while(1) {
        gpio_set_level(led_number, 1); 
        ESP_LOGI(TAG, "%d state %d", led_number, gpio_get_level(led_number));
        vTaskDelay(100 / portTICK_PERIOD_MS);
        vTaskDelete(NULL);
    }
}

void led_off_task (void * pvParameters) 
{
    int led_number = (int)pvParameters;

    while(1) {
        gpio_set_level(led_number, 0); 
        ESP_LOGI(TAG, "%d state %d", led_number, gpio_get_level(led_number));
        vTaskDelay(100 / portTICK_PERIOD_MS);
        vTaskDelete(NULL);
    }
}