
#ifndef APP_ERROR_CHECK_H_
#define APP_ERROR_CHECK_H_


#include "../arduino/Arduino.h"

void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name);


#endif

