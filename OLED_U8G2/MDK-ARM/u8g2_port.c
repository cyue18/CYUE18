#include "u8g2_port.h"

// GPIO和延时函数
uint8_t u8x8_stm32_gpio_and_delay(U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr)
{
  switch (msg)
  {
  case U8X8_MSG_GPIO_AND_DELAY_INIT: // 初始化，被u8g2.begin()调用
    // 这里可以放置一些初始化的代码，但HAL库已经通过CubeMX初始化好了，通常为空。
    break;
  case U8X8_MSG_DELAY_MILLI:         // 延时毫秒命令
    HAL_Delay(arg_int);               // 调用HAL库的延时函数
    break;
  case U8X8_MSG_DELAY_10MICRO:       // 延时10微秒命令 (对于I2C通常不需要)
    for (uint16_t n = 0; n < 80; n++) // 粗略的10us延时循环，根据CPU频率调整
    {
      __NOP();
    }
    break;
  case U8X8_MSG_DELAY_100NANO:       // 延时100纳秒命令 (对于I2C通常不需要)
    __NOP(); // 执行一个空操作，大约几十纳秒
    break;
  default:
    return 0; // 消息未知，返回0表示失败
  }
  return 1; // 消息处理成功
}

// I2C字节传输函数
uint8_t u8x8_byte_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  static uint8_t buffer[32]; // U8g2将会分块传输数据，32字节是常见的块大小
  static uint8_t buf_idx;
  uint8_t *data;

  switch (msg)
  {
  case U8X8_MSG_BYTE_SEND: // 发送数据，arg_ptr指向要发送的数据，arg_int是数据长度
    data = (uint8_t *)arg_ptr;
    while (arg_int > 0)
    {
      buffer[buf_idx++] = *data;
      data++;
      arg_int--;
    }
    break;
  case U8X8_MSG_BYTE_INIT: // 初始化，这里不需要做什么
    break;
  case U8X8_MSG_BYTE_SET_DC: // 设置数据/命令引脚，对于硬件I2C，这个信息通常包含在地址中，所以这里什么都不做。
    break;
  case U8X8_MSG_BYTE_START_TRANSFER: // 开始传输
    buf_idx = 0;
    break;
  case U8X8_MSG_BYTE_END_TRANSFER: // 结束传输，将缓冲区的数据通过HAL_I2C_Master_Transmit发送出去
    // 0x78 是SSD1306的I2C地址（7位地址左移一位后为0x3C << 1 = 0x78）
    if (HAL_I2C_Master_Transmit(&hi2c1, 0x78, buffer, buf_idx, HAL_MAX_DELAY) != HAL_OK)
    {
      return 0; // 传输失败
    }
    break;
  default:
    return 0;
  }
  return 1;
}
