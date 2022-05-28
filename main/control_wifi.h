/**
 * @file control_wifi.h
 * 
 * @brief Initializes Wi-Fi and connects to predefined AP
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef CONTROL_WIFI_H_
#define CONTROL_WIFI_H_

#define MAXIMUM_RETRY   5

/** DEFINES **/
#define WIFI_SUCCESS 1 << 0
#define WIFI_FAILURE 1 << 1
#define TCP_SUCCESS 1 << 0
#define TCP_FAILURE 1 << 1

// add functions

void connect_wifi();

#endif /* CONTROL_WIFI_H */

/*** end of file ***/