/**
 * @file httpd_server.h
 * 
 * @brief Start and stop Webserver. Handles also incoming requests
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef HTTPD_SERVER_H_
#define HTTPD_SERVER_H_

httpd_handle_t start_webserver(void);
esp_err_t stop_webserver(httpd_handle_t server);

#endif /* HTTPD_SERVER */

/*** end of file ***/