/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "u8g2.h"
#include "u8g2_port.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
u8g2_t u8g2; // ����һ��U8g2�ṹ��
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
// ��ʼ��U8g2�⣬����ΪSSD1306������128x64�ֱ��ʣ�Ӳ��I2C
u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2, U8G2_R0, u8x8_byte_hw_i2c, u8x8_stm32_gpio_and_delay);
u8g2_InitDisplay(&u8g2); // ���ͳ�ʼ�����е���ʾ��
u8g2_SetPowerSave(&u8g2, 0); // ������ʾ��
u8g2_ClearBuffer(&u8g2);     // ����ڲ�������

// ��ʾ��ӭ��Ϣ�����ͼ��
u8g2_SetFont(&u8g2, u8g2_font_ncenB08_tr); // ��������
u8g2_DrawStr(&u8g2, 0, 20, "U8g2 Init OK!"); // ������(0,20)�����ַ���
u8g2_SendBuffer(&u8g2); // �����������ݷ��͵���ʾ��

HAL_Delay(2000); // ��ʱ2��
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		// ����1����ʾѧ�ź�����
u8g2_ClearBuffer(&u8g2); // ���������
u8g2_SetFont(&u8g2, u8g2_font_unifont_t_chinese2);
		u8g2_DrawUTF8(&u8g2, 0, 20, "QQ:3614789717");
		u8g2_DrawUTF8(&u8g2, 0, 40, "ID:CYUE18");
u8g2_SendBuffer(&u8g2);
HAL_Delay(3000); // ��ʾ3��
		// ����2�����»�����ʾ
u8g2_SetFont(&u8g2, u8g2_font_6x10_tf);
for (int pos = 64; pos >= -64; pos--) { // �������ϻ���
  u8g2_ClearBuffer(&u8g2);
  u8g2_DrawStr(&u8g2, 0, pos, "QQ:3614789717");
  u8g2_DrawStr(&u8g2, 0, pos+20, "ID:CYUE18");
  u8g2_SendBuffer(&u8g2);
  HAL_Delay(50); // ���ƻ����ٶ�
}
HAL_Delay(1000);
// ����3����ʾ��̬ͼ��
// ����3����ʾ�Ľ��Ķ�̬ͼ��
int ball_x = 20;  // ��ʼλ�ò�Ҫ�����߽�
int ball_y = 20;
int ball_vx = 2;
int ball_vy = 2;
int ball_radius = 5;

for (int i = 0; i < 300; i++) { // ����֡����������
  u8g2_ClearBuffer(&u8g2);

  // ���Ʊ߿�ͱ��������û�����ḻ��
  u8g2_DrawFrame(&u8g2, 0, 0, 128, 64);
  
  // ���Ʊ���������
  for (int x = 10; x < 128; x += 10) {
    u8g2_DrawVLine(&u8g2, x, 0, 64);
  }
  for (int y = 10; y < 64; y += 10) {
    u8g2_DrawHLine(&u8g2, 0, y, 128);
  }
  
  // �����ƶ���С��ʵ��Բ��
  u8g2_DrawDisc(&u8g2, ball_x, ball_y, ball_radius, U8G2_DRAW_ALL);
  
  // ��С�����Ļ����㣬����۲�
  u8g2_DrawPixel(&u8g2, ball_x, ball_y);

  // ����С��λ��
  ball_x += ball_vx;
  ball_y += ball_vy;

  // �Ľ��ı߽���ײ��⣨����С��뾶��
  if (ball_x <= ball_radius || ball_x >= 128 - ball_radius) {
    ball_vx = -ball_vx;
    // ��ֹ���ڱ߽�
    if (ball_x < ball_radius) ball_x = ball_radius;
    if (ball_x > 128 - ball_radius) ball_x = 128 - ball_radius;
  }
  
  if (ball_y <= ball_radius || ball_y >= 64 - ball_radius) {
    ball_vy = -ball_vy;
    // ��ֹ���ڱ߽�
    if (ball_y < ball_radius) ball_y = ball_radius;
    if (ball_y > 64 - ball_radius) ball_y = 64 - ball_radius;
  }

  // ��ʾ֡����λ����Ϣ�������ã�
  u8g2_SetFont(&u8g2, u8g2_font_5x7_tf);
  char info[20];
  snprintf(info, sizeof(info), "Frame:%d", i);
  u8g2_DrawStr(&u8g2, 70, 10, info);
  
  snprintf(info, sizeof(info), "Pos:%d,%d", ball_x, ball_y);
  u8g2_DrawStr(&u8g2, 70, 20, info);

  u8g2_SendBuffer(&u8g2);
  HAL_Delay(30); // ���ƶ���֡�ʣ���΢��һ��
}
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
