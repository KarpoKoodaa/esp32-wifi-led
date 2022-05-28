/**
 * @file control_wifi.c
 *
 * @brief Initilizes Wi-Fi and connects to AP. 
 * 
 * 
 * @copyright Copyright (c) 2022
 * 
 */

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

#include "control_wifi.h"
#include "secrets.h"

// TAG for logging
static const char* TAG = "WIFI";

// event group to contain status information
static EventGroupHandle_t wifi_event_group;

static int s_retry_num = 0;

/**
 * @brief Wi-Fi event handler
 * 
 * @param handler_arg 
 * @param event_base 
 * @param event_id 
 * @param event_data 
 */

static void wifi_event_handler(void* handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if(event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        ESP_LOGI(TAG, "Connecting to AP....");
        esp_wifi_connect();
    } else if(event_base == WIFI_EVENT && WIFI_EVENT_STA_DISCONNECTED) {
        if(s_retry_num < MAXIMUM_RETRY){
            ESP_LOGI(TAG, "Reconnecting to AP....");
            esp_wifi_connect();
            s_retry_num++;
        } else {
            xEventGroupSetBits(wifi_event_group, WIFI_FAILURE);
        }

    }
}


/**
 * @brief event handler for ip events
 * 
 * @param handler_args 
 * @param event_base 
 * @param event_id 
 * @param event_data 
 */

static void ip_event_handler(void* handler_args, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if(event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "STA IP: " IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(wifi_event_group, WIFI_SUCCESS);
    }
}

/**
 * @brief Connects to Wi-Fi AP
 * 
 */

void connect_wifi(void )
{
    int status = WIFI_FAILURE;

    /** INITIALIZE ALL THE THINGS **/
    // Initialize the ESP32 Netwok Interface
    ESP_ERROR_CHECK(esp_netif_init());

    // Initialize default ESP32 event loop
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Create Wi-Fi Station in the Wi-Fi driver
    esp_netif_create_default_wifi_sta();

    // Setup Wi-Fi station with the defaul Wi-Fi configuration
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // Create an event group
    wifi_event_group = xEventGroupCreate();

    esp_event_handler_instance_t wifi_handler_event_instance;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &wifi_handler_event_instance));

    esp_event_handler_instance_t got_ip_event_instance;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &ip_event_handler,
                                                        NULL,
                                                        &got_ip_event_instance));

    // Start the WI-FI driver
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .capable = true,
                .required = false
            },
        },
    };

    // set the wifi controller to be a station
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    // Set the Wi-Fi config
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

    // Start the Wi-Fi driver
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "STA initialization complete");

    EventBits_t bits = xEventGroupWaitBits(wifi_event_group,
                                            WIFI_SUCCESS | WIFI_FAILURE,
                                            pdFALSE,
                                            pdFALSE,
                                            portMAX_DELAY);
    if(bits & WIFI_SUCCESS) {
        ESP_LOGI(TAG, "Connected to AP");
        status = WIFI_SUCCESS;
    } else if (bits & WIFI_FAILURE) {
        ESP_LOGI(TAG, "Failed to connect to AP");
        status = WIFI_FAILURE;
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
        status = WIFI_FAILURE;
    }

    /* The event will no be processed after unregister */
    ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, got_ip_event_instance));
    ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_handler_event_instance));
    vEventGroupDelete(wifi_event_group);

}
