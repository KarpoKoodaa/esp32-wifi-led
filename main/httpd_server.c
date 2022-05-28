/**
 * @file httpd_server.c
 *
 * @brief Initializes HTTP server, sets URI handlers and server webpages
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_eth.h"
#include "esp_tls_crypto.h"
#include <esp_http_server.h>


#include "led.h"

// TAG for logging
static const char *TAG = "HTTP_SERVER";

/**
 * @brief Handler allows the custom error handling functionality
 * 
 * @param req incoming request
 * @param err httpd error code
 * @return esp_err_t ESP error
 */
esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    /* For any other URI send 404 and close socket */
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Some 404 error message");
    return ESP_FAIL;
}


/**
 * @brief LED handler to handle single led, e.g. with CURL
 * 
 * @param req GPIO LED to be toggled
 * @return esp_err_t ESP error
 */
static esp_err_t led_handler(httpd_req_t *req)
{
    char buf;
    int ret;

    if ((ret = httpd_req_recv(req, &buf, 1)) <= 0) {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }

    switch (buf)
    {
    case '1':
        ESP_LOGI(TAG, "Changing LED %c", buf);
        xTaskCreate(&toggle_led_task, "Toggle LED task", 2048, ( void * ) LED_1_GPIO, 5, NULL);
        break;
    case '2':
        ESP_LOGI(TAG, "Changing LED %c", buf);
        xTaskCreate(&toggle_led_task, "Toggle LED task", 2048, ( void * ) LED_2_GPIO, 5, NULL);
        break;
    case '3':
        ESP_LOGI(TAG, "Changing LED %c", buf);
        xTaskCreate(&toggle_led_task, "Toggle LED task", 2048, ( void * ) LED_3_GPIO, 5, NULL);
        break;
    case '4':
        ESP_LOGI(TAG, "Changing LED %c", buf);
        xTaskCreate(&toggle_led_task, "Toggle LED task", 2048, ( void * ) LED_4_GPIO, 5, NULL);
        break;
    default:
        ESP_LOGE(TAG, "UNEXPECTED ERROR!");
        break;
    }

    /* Respond with empty body */
    httpd_resp_send(req, NULL, 0);
    return ESP_OK;
    
}

static const httpd_uri_t led = {
    .uri = "/led",
    .method = HTTP_PUT,
    .handler = led_handler,
    .user_ctx = NULL
};

/**
 * @brief LED OFF webpage handler
 * 
 * @param req GPIO LED to be turned OFF
 * @return esp_err_t ESP error
 */
static esp_err_t ledoff_handler(httpd_req_t *req)
{
    esp_err_t error;
    ESP_LOGI(TAG, "LED turned off");

    const char *response = (const char *) req->user_ctx;
    error = httpd_resp_send(req, response, strlen(response));
    
    if(error != ESP_OK) {
        ESP_LOGE(TAG, "Error %d while sending responce", error);
    }
    else {
        ESP_LOGI(TAG, "Response sent successfully");
    }

    xTaskCreate(&led_off_task, "LED OFF task", 2048, ( void * ) LED_1_GPIO, 5, NULL);

    return error;
}



static const httpd_uri_t led_off = {
    .uri = "/ledoff",
    .method = HTTP_GET,
    .handler = ledoff_handler,
    .user_ctx = 
    "<!DOCTYPE html>\
<html>\
<head>\
<style>\
.button {\
  border: none;\
  color: white;\
  padding: 15px 32px;\
  text-align: center;\
  text-decoration: none;\
  display: inline-block;\
  font-size: 16px;\
  margin: 4px 2px;\
  cursor: pointer;\
}\
\
.button1 {background-color: #4CAF50;} /* Green */\
</style>\
</head>\
<body>\
\    
<h1>ESP32 WebServer</h1>\
<p>Toggle LEDs</p>\
<h3> LED STATE: OFF </h3>\
\
<button class=\"button button1\" onclick=\"window.location.href='/ledon'\">LED ON</button>\
\
</body>\
</html>"
};

/**
 * @brief LED ON webpage handler
 * 
 * @param req GPIO LED to be turned ON
 * @return esp_err_t ESP error
 */
static esp_err_t ledon_handler(httpd_req_t *req)
{
    esp_err_t error;
    ESP_LOGI(TAG, "LED turned on");

    const char *response = (const char *) req->user_ctx;
    error = httpd_resp_send(req, response, strlen(response));
    if(error != ESP_OK) {
        ESP_LOGE(TAG, "Error %d while sending responce", error);
    }
    else {
        ESP_LOGI(TAG, "Response sent successfully");
    }
    xTaskCreate(&led_on_task, "LED ON task", 2048, ( void * ) LED_1_GPIO, 5, NULL);

    return error;
}



static const httpd_uri_t led_on = {
    .uri = "/ledon",
    .method = HTTP_GET,
    .handler = ledon_handler,
    .user_ctx = 
    "<!DOCTYPE html>\
<html>\
<head>\
<style>\
.button {\
  border: none;\
  color: white;\
  padding: 15px 32px;\
  text-align: center;\
  text-decoration: none;\
  display: inline-block;\
  font-size: 16px;\
  margin: 4px 2px;\
  cursor: pointer;\
}\
\
.button1 {background-color: #000000;} /* Black */\
</style>\
</head>\
<body>\
\    
<h1>ESP32 WebServer</h1>\
<p>Toggle LEDs</p>\
<h3> LED STATE: ON </h3>\
\
<button class=\"button button1\" onclick=\"window.location.href='/ledoff'\">LED OFF</button>\
\
</body>\
</html>"
};

/**
 * @brief Start the Webserver
 * 
 * @return httpd_handle_t 
 */

httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: %d", config.server_port);
    if(httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &led);
        httpd_register_uri_handler(server, &led_off);
        httpd_register_uri_handler(server, &led_on);

            return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;

}

/**
 * @brief Stops running Webserver
 * 
 * @param server server to be stopped
 * @return esp_err_t ESP error
 */
esp_err_t stop_webserver(httpd_handle_t server)
{
    // stop the httpd server
    return httpd_stop(server);
}

/**
 * @brief Handler for disconnecting HTTP server
 * 
 * @param arg 
 * @param event_base 
 * @param event_id 
 * @param event_data 
 */
static void disconnect_handler (void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server) {
        ESP_LOGI(TAG, "Stopping webserver");
        if (stop_webserver(*server) == ESP_OK) {
            *server = NULL;
        } else {
            ESP_LOGE(TAG, "Failed to stop http server");
        }
    }
}

/**
 * @brief Handler for connecting the HTTP server
 * 
 * @param arg 
 * @param event_base 
 * @param event_id 
 * @param event_data 
 */
static void connect_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server == NULL) {
        ESP_LOGI(TAG, "Starting webserver");
        *server = start_webserver();
    }
}

