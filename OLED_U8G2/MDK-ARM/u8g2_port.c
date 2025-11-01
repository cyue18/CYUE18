#include "u8g2_port.h"

// GPIO����ʱ����
uint8_t u8x8_stm32_gpio_and_delay(U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr)
{
  switch (msg)
  {
  case U8X8_MSG_GPIO_AND_DELAY_INIT: // ��ʼ������u8g2.begin()����
    // ������Է���һЩ��ʼ���Ĵ��룬��HAL���Ѿ�ͨ��CubeMX��ʼ�����ˣ�ͨ��Ϊ�ա�
    break;
  case U8X8_MSG_DELAY_MILLI:         // ��ʱ��������
    HAL_Delay(arg_int);               // ����HAL�����ʱ����
    break;
  case U8X8_MSG_DELAY_10MICRO:       // ��ʱ10΢������ (����I2Cͨ������Ҫ)
    for (uint16_t n = 0; n < 80; n++) // ���Ե�10us��ʱѭ��������CPUƵ�ʵ���
    {
      __NOP();
    }
    break;
  case U8X8_MSG_DELAY_100NANO:       // ��ʱ100�������� (����I2Cͨ������Ҫ)
    __NOP(); // ִ��һ���ղ�������Լ��ʮ����
    break;
  default:
    return 0; // ��Ϣδ֪������0��ʾʧ��
  }
  return 1; // ��Ϣ����ɹ�
}

// I2C�ֽڴ��亯��
uint8_t u8x8_byte_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  static uint8_t buffer[32]; // U8g2����ֿ鴫�����ݣ�32�ֽ��ǳ����Ŀ��С
  static uint8_t buf_idx;
  uint8_t *data;

  switch (msg)
  {
  case U8X8_MSG_BYTE_SEND: // �������ݣ�arg_ptrָ��Ҫ���͵����ݣ�arg_int�����ݳ���
    data = (uint8_t *)arg_ptr;
    while (arg_int > 0)
    {
      buffer[buf_idx++] = *data;
      data++;
      arg_int--;
    }
    break;
  case U8X8_MSG_BYTE_INIT: // ��ʼ�������ﲻ��Ҫ��ʲô
    break;
  case U8X8_MSG_BYTE_SET_DC: // ��������/�������ţ�����Ӳ��I2C�������Ϣͨ�������ڵ�ַ�У���������ʲô��������
    break;
  case U8X8_MSG_BYTE_START_TRANSFER: // ��ʼ����
    buf_idx = 0;
    break;
  case U8X8_MSG_BYTE_END_TRANSFER: // �������䣬��������������ͨ��HAL_I2C_Master_Transmit���ͳ�ȥ
    // 0x78 ��SSD1306��I2C��ַ��7λ��ַ����һλ��Ϊ0x3C << 1 = 0x78��
    if (HAL_I2C_Master_Transmit(&hi2c1, 0x78, buffer, buf_idx, HAL_MAX_DELAY) != HAL_OK)
    {
      return 0; // ����ʧ��
    }
    break;
  default:
    return 0;
  }
  return 1;
}
