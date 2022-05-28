/**
 * @file led.h
 * 
 * @brief  Initialized LEDs as input/output and handles LEDs toggling 
 * 
 * @par
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef LED_H_
#define LED_H_

// LED GPIOs
/* @todo
* Refactor NUM_OF_LEDS to more understandable
*/

//typedef enum LED_PIN_GPIO {LED_1_GPIO = 2, LED_2_GPIO = 3, LED_3_GPIO = 4, LED_4_GPIO = 5} LED_PIN;
#define NUM_OF_LEDS 6
#define LED_1_GPIO 2
#define LED_2_GPIO 15
#define LED_3_GPIO 5
#define LED_4_GPIO 18


void initialize_leds(void);
void toggle_led_task( void * pvParameters);
void led_on_task (void * pvParameters);
void led_off_task (void * pvParameters);

#endif /* LED_H */

/*** end of file ***/