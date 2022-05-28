/**
 * @file button_state.c
 * @brief Initializes buttons and handles button presses
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdio.h>
#include "freertos/freeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "driver/gpio.h"

#include "esp_log.h"

#include "button_state.h"
#include "led.h"

// TAG for logging
static const char* TAG = "BUTTON";

// Initialize queue to handle interrupts
static QueueHandle_t gpio_evt_queue = NULL;

/*!
 * @brief Interrupt Service Routine for button press
 * 
 * @param arg button GPIO that was pressed
 */

void static IRAM_ATTR gpio_isr_handler(void * arg) {
    
    u_int32_t gpio_num = (u_int32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);

}

/**
 * @brief Initializes GPIOs as INPUTs and adds ISRs 
 * 
 */

void initialize_buttons(void)
{
    // Enable buttons
    gpio_reset_pin(BUTTON_1);
    gpio_set_direction(BUTTON_1, GPIO_MODE_INPUT);
    gpio_set_intr_type(BUTTON_1, GPIO_INTR_POSEDGE);

    gpio_reset_pin(BUTTON_2);
    gpio_set_direction(BUTTON_2, GPIO_MODE_INPUT);
    gpio_set_intr_type(BUTTON_2, GPIO_INTR_POSEDGE);

    gpio_reset_pin(BUTTON_3);
    gpio_set_direction(BUTTON_3, GPIO_MODE_INPUT);
    gpio_set_intr_type(BUTTON_3, GPIO_INTR_POSEDGE);

    gpio_reset_pin(BUTTON_4);
    gpio_set_direction(BUTTON_4, GPIO_MODE_INPUT);
    gpio_set_intr_type(BUTTON_4, GPIO_INTR_POSEDGE);


    gpio_evt_queue = xQueueCreate(10, sizeof(u_int32_t));

    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);

    // add ISRs
    gpio_isr_handler_add(BUTTON_1, gpio_isr_handler, (void *) BUTTON_1);
    gpio_isr_handler_add(BUTTON_2, gpio_isr_handler, (void *) BUTTON_2);
    gpio_isr_handler_add(BUTTON_3, gpio_isr_handler, (void *) BUTTON_3);
    gpio_isr_handler_add(BUTTON_4, gpio_isr_handler, (void *) BUTTON_4);
    ESP_LOGI(TAG, "Buttons initialized");
    
}

/**
 * @brief Task for handling button presses
 * 
 * @param pvParameters GPIO (button) that was pressed
 */

void press_button_task( void *pvParameters )
{
    uint32_t button_state = 0;

    while (1)
    {
        
        if(xQueueReceive(gpio_evt_queue, &button_state, portMAX_DELAY)) {
            ESP_LOGI(TAG, "Button %d pressed", button_state);
            switch (button_state)
            {
            case 12:
                xTaskCreate(&toggle_led_task, "Toggle LED task", 2048, ( void * ) LED_1_GPIO, 5, NULL);
                vTaskDelay(200 / portTICK_PERIOD_MS);
                break;
            case 13:
                xTaskCreate(&toggle_led_task, "Toggle LED task", 2048, ( void * ) LED_2_GPIO, 5, NULL);
                vTaskDelay(200 / portTICK_PERIOD_MS);
                break;
            case 27:
                xTaskCreate(&toggle_led_task, "Toggle LED task", 2048, ( void * ) LED_3_GPIO, 5, NULL);
                vTaskDelay(200 / portTICK_PERIOD_MS);
                break;
            case 26:
                xTaskCreate(&toggle_led_task, "Toggle LED task", 2048, ( void * ) LED_4_GPIO, 5, NULL);
                vTaskDelay(200 / portTICK_PERIOD_MS);
                break;
            default:
                ESP_LOGE(TAG, "UNEXPECTED ERROR");
                break;
            }
            
        }
        vTaskDelay(1);
    }
}