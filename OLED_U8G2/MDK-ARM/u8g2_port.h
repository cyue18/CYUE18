#ifndef U8G2_PORT_H
#define U8G2_PORT_H

#include "stm32f1xx_hal.h" // ȷ������HAL��ͷ�ļ�
#include "u8g2.h"         // ����U8g2��ͷ�ļ�

// ����һ���ⲿ����������I2C��������ǽ���main.c�ж�����
extern I2C_HandleTypeDef hi2c1;

// ����U8g2�ĳ�ʼ������
uint8_t u8x8_stm32_gpio_and_delay(U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr);
uint8_t u8x8_byte_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

#endif
