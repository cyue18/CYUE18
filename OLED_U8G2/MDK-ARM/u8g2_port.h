#ifndef U8G2_PORT_H
#define U8G2_PORT_H

#include "stm32f1xx_hal.h" // 确保包含HAL库头文件
#include "u8g2.h"         // 包含U8g2库头文件

// 声明一个外部变量，用于I2C句柄，我们将在main.c中定义它
extern I2C_HandleTypeDef hi2c1;

// 声明U8g2的初始化函数
uint8_t u8x8_stm32_gpio_and_delay(U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr);
uint8_t u8x8_byte_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

#endif
