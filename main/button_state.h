/**
 * @file button_state.h
 * 
 * @brief Initializes buttons and handles button presses 
 * 
 * @date 2022-05-28
 * 
 * @copyright Copyright (c) 2022
 * 
 */


#ifndef BUTTON_STATE_H_
#define BUTTON_STATE_H_

#define BUTTON_1 12
#define BUTTON_2 13
#define BUTTON_3 27
#define BUTTON_4 26

#define ESP_INTR_FLAG_DEFAULT 0

void initialize_buttons (void);
void press_button_task(void *pvParameter);

#endif /* BUTTON_STATE_H */

/*** end of file ***/