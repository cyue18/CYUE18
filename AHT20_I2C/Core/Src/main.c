/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * AHT20温湿度传感器实验 - 完整正确版本
  * 
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdio.h>
#include <string.h>

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
UART_HandleTypeDef huart1;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);

/* AHT20相关定义和函数 */
#define AHT20_ADDRESS 0x38
#define AHT20_INIT_CMD 0xBE
#define AHT20_MEASURE_CMD 0xAC
#define AHT20_NORMAL_CMD 0xA8

// 串口发送函数 - 可靠版本
void UART_SendString(const char *str)
{
    if(str != NULL)
    {
        HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), 1000);
    }
}

// 重定向printf - 可选
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

PUTCHAR_PROTOTYPE
{
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}

/**
  * @brief  AHT20初始化
  * @retval 初始化状态：0-成功，1-失败
  */
uint8_t AHT20_Init(void)
{
    uint8_t cmd[3] = {AHT20_INIT_CMD, 0x08, 0x00};
    
    HAL_Delay(40); // 上电后等待至少40ms
    
    if(HAL_I2C_Master_Transmit(&hi2c1, AHT20_ADDRESS << 1, cmd, 3, 100) != HAL_OK)
    {
        return 1;
    }
    
    HAL_Delay(100); // 等待初始化完成
    
    // 检查初始化状态
    uint8_t status = 0;
    if(HAL_I2C_Master_Receive(&hi2c1, AHT20_ADDRESS << 1, &status, 1, 100) != HAL_OK)
    {
        return 1;
    }
    
    if((status & 0x18) != 0x18) // 检查校准位和就绪位
    {
        return 1;
    }
    
    return 0;
}

/**
  * @brief  读取AHT20状态
  * @retval 状态字节
  */
uint8_t AHT20_ReadStatus(void)
{
    uint8_t status = 0;
    if(HAL_I2C_Master_Receive(&hi2c1, AHT20_ADDRESS << 1, &status, 1, 100) == HAL_OK)
    {
        return status;
    }
    return 0xFF; // 读取失败
}

/**
  * @brief  触发AHT20测量
  * @retval 成功返回0，失败返回1
  */
uint8_t AHT20_StartMeasurement(void)
{
    uint8_t cmd[3] = {AHT20_MEASURE_CMD, 0x33, 0x00};
    
    if(HAL_I2C_Master_Transmit(&hi2c1, AHT20_ADDRESS << 1, cmd, 3, 100) != HAL_OK)
    {
        return 1;
    }
    
    return 0;
}

/**
  * @brief  读取温湿度数据
  * @param  temperature: 温度值指针(°C)
  * @param  humidity: 湿度值指针(%RH)
  * @retval 成功返回0，失败返回1
  */
uint8_t AHT20_ReadData(float *temperature, float *humidity)
{
    uint8_t data[6] = {0};
    uint32_t timeout = 1000; // 超时时间
    
    // 等待测量完成
    while(timeout--)
    {
        if(HAL_I2C_Master_Receive(&hi2c1, AHT20_ADDRESS << 1, data, 1, 10) == HAL_OK)
        {
            if(!(data[0] & 0x80)) // 检查忙标志位
            {
                break;
            }
        }
        HAL_Delay(1);
    }
    
    if(timeout == 0)
    {
        return 1; // 超时
    }
    
    // 读取全部6字节数据
    if(HAL_I2C_Master_Receive(&hi2c1, AHT20_ADDRESS << 1, data, 6, 100) != HAL_OK)
    {
        return 1;
    }
    
    // 解析湿度数据 (20bit)
    uint32_t hum_raw = ((uint32_t)data[1] << 12) | 
                      ((uint32_t)data[2] << 4) | 
                      ((uint32_t)data[3] >> 4);
    *humidity = (float)hum_raw * 100.0 / 1048576.0; // 2^20 = 1048576
    
    // 解析温度数据 (20bit)
    uint32_t temp_raw = (((uint32_t)data[3] & 0x0F) << 16) | 
                       ((uint32_t)data[4] << 8) | 
                       data[5];
    *temperature = (float)temp_raw * 200.0 / 1048576.0 - 50.0;
    
    return 0;
}

/**
  * @brief  系统诊断信息
  */
void System_Diagnostic(void)
{
    char buffer[100];
    
    UART_SendString("\r\n=== STM32F103 System Diagnostic ===\r\n");
    
    // 系统时钟信息
    sprintf(buffer, "System Clock: %lu Hz\r\n", HAL_RCC_GetSysClockFreq());
    UART_SendString(buffer);
    
    sprintf(buffer, "HCLK Frequency: %lu Hz\r\n", HAL_RCC_GetHCLKFreq());
    UART_SendString(buffer);
    
    sprintf(buffer, "PCLK1 Frequency: %lu Hz\r\n", HAL_RCC_GetPCLK1Freq());
    UART_SendString(buffer);
    
    sprintf(buffer, "PCLK2 Frequency: %lu Hz\r\n", HAL_RCC_GetPCLK2Freq());
    UART_SendString(buffer);
    
    UART_SendString("=== Diagnostic End ===\r\n\r\n");
}

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    /* MCU Configuration--------------------------------------------------------*/
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_I2C1_Init();
    MX_USART1_UART_Init();

    /* 启动指示 - LED闪烁3次 */
    for(int i = 0; i < 6; i++)
    {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        HAL_Delay(100);
    }
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
    
    /* 系统启动信息 */
    UART_SendString("\r\n");
    UART_SendString("***************************************\r\n");
    UART_SendString("* STM32F103 AHT20 Sensor Test Program *\r\n");
    UART_SendString("*        Version: 2.0 - Fixed        *\r\n");
    UART_SendString("***************************************\r\n\r\n");
    
    /* 系统诊断 */
    System_Diagnostic();
    
    /* AHT20初始化 */
    UART_SendString("Initializing AHT20 sensor...\r\n");
    
    if(AHT20_Init() != 0)
    {
        UART_SendString("ERROR: AHT20 initialization failed!\r\n");
        UART_SendString("Please check I2C connection.\r\n");
    }
    else
    {
        UART_SendString("SUCCESS: AHT20 initialized successfully!\r\n");
        
        // 读取状态
        uint8_t status = AHT20_ReadStatus();
        char status_msg[50];
        sprintf(status_msg, "AHT20 Status: 0x%02X\r\n", status);
        UART_SendString(status_msg);
    }
    
    UART_SendString("\r\nStarting temperature and humidity measurement...\r\n");
    UART_SendString("===========================================\r\n");
    
    float temperature, humidity;
    uint32_t measurement_count = 0;
    
    while (1)
    {
        measurement_count++;
        
        /* 触发测量 */
        if(AHT20_StartMeasurement() != 0)
        {
            UART_SendString("ERROR: Failed to start measurement\r\n");
        }
        else
        {
            /* 读取数据 */
            if(AHT20_ReadData(&temperature, &humidity) == 0)
            {
                char result[80];
                sprintf(result, "[%lu] Temperature: %6.2f C, Humidity: %6.2f %%\r\n", 
                       measurement_count, temperature, humidity);
                UART_SendString(result);
                
                // LED指示测量成功
                HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
                HAL_Delay(100);
                HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
            }
            else
            {
                UART_SendString("ERROR: Failed to read sensor data\r\n");
            }
        }
        
        HAL_Delay(2000); // 2秒间隔
    }
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

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 100000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

    /*Configure GPIO pin : PC13 */
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

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
        // 错误时快速闪烁LED
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        HAL_Delay(100);
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