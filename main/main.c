#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include <esp_http_server.h>

#include "secrets.h"
#include "control_wifi.h"
#include "led.h"
#include "button_state.h"
#include "httpd_server.h"

static const char* TAG = "MAIN";

void app_main(void)
{
    static httpd_handle_t server = NULL;

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_OK){
        ESP_LOGI(TAG, "Storage succesfully initialized");
    } else if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGI(TAG, "ERROR receive, erasing flash...");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    
    connect_wifi();
    server = start_webserver();
    initialize_leds();
    initialize_buttons();

    
    xTaskCreate(&press_button_task, "Button Press", 2048, NULL, 5, NULL);
}