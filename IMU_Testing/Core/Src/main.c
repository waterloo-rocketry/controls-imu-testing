/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "printf.h"
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
I2C_HandleTypeDef hi2c1;
UART_HandleTypeDef huart2;
/* USER CODE BEGIN PV */
uint8_t temp_data = 0;
char uart_buffer_accel[64];
char uart_buffer_gyro[64];
/* USER CODE END PV */
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);
//static void MX_USART1_UART_Init(void);
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
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  uint8_t INT1_CTRL = 0x01;	// Acc data-ready interrupt on INT1
  uint8_t CTRL1_XL = 0x60;	// ODR = 416 Hz, FS = ±2g
  uint8_t CTRL2_G = 0x60;	// ODR = 416 Hz, FS = ±250 dps
  //set the data-ready interrupt on INT1 and ODR/FS on CTRL1_XL
  HAL_I2C_Mem_Write(&hi2c1, (0b01101010 << 1), 0x10, 1, &CTRL1_XL, 1, 100);	//accel
  HAL_I2C_Mem_Write(&hi2c1, (0b01101010 << 1), 0x11, 1, &CTRL2_G, 1, 100);	//gyro
  HAL_I2C_Mem_Write(&hi2c1, (0b01101010 << 1), 0x0, 1, &INT1_CTRL, 1, 100);	//data-ready interrupt pin
  HAL_I2C_IsDeviceReady(&hi2c1, (0b01101010 << 1) , 1, 100);
  //sending a "0" on I2C just in case
  HAL_I2C_Master_Transmit(&hi2c1, (0b01101010 << 1), &temp_data, 1, 100);
  //reading the WHO_AM_I register
  HAL_I2C_Mem_Read(&hi2c1, (0b01101010 << 1) + 1, 0x0F, 1, &temp_data, 1, 100);
  /* USER CODE END 2 */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
	  if (temp_data == 0x6C) {
		  // Turn the LED on if the WHO_AM_I register is fine
	      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
	  }
	  /* Accelerometer data read*/
	  uint8_t accel_data[6]; // Array to store X, Y, Z values
	  HAL_I2C_Mem_Read(&hi2c1, (0b01101010 << 1), 0x28, 1, accel_data, 6, 100);
	  int16_t accel_x = (int16_t)(accel_data[1] << 8 | accel_data[0]);
	  int16_t accel_y = (int16_t)(accel_data[3] << 8 | accel_data[2]);
	  int16_t accel_z = (int16_t)(accel_data[5] << 8 | accel_data[4]);
	  //convert to g based on a sensitivity of 0.061 mg/LSB for FS = ±2g
	  float accel_x_g = accel_x * (0.061 / 1000)*9.8;
	  float accel_y_g = accel_y * (0.061 / 1000)*9.8;
	  float accel_z_g = accel_z * (0.061 / 1000)*9.8;
	  /* Gyroscope data read*/
	  uint8_t gyro_data[6];  // Array to store X, Y, Z values
	  HAL_I2C_Mem_Read(&hi2c1, (0b01101010 << 1), 0x22, 1, gyro_data, 6, 100);
	  int16_t gyro_x = (int16_t)(gyro_data[1] << 8 | gyro_data[0]);
	  int16_t gyro_y = (int16_t)(gyro_data[3] << 8 | gyro_data[2]);
	  int16_t gyro_z = (int16_t)(gyro_data[5] << 8 | gyro_data[4]);
	  // Convert to dps (degrees per second) based on sensitivity 8.75 mdps/LSB for ±250 dps
	  float gyro_x_dps = gyro_x * 8.75 / 1000;
	  float gyro_y_dps = gyro_y * 8.75 / 1000;
	  float gyro_z_dps = gyro_z * 8.75 / 1000;
	  /*Data transfer to terminal*/
	  //set all the data in the buffer to send it all at once to the USB-TTL converter
	  snprintf_(uart_buffer_accel, 100, "Accel - X: %.2f g, Y: %.2f g, Z: %.2f g\r\n", accel_x_g, accel_y_g, accel_z_g);
	  snprintf_(uart_buffer_gyro, 100, "Gyro - X: %.2f dps, Y: %.2f dps, Z: %.2f dps\r\n\n", gyro_x_dps, gyro_y_dps, gyro_z_dps);
	  HAL_UART_Transmit(&huart2, (uint8_t*)uart_buffer_accel, strlen(uart_buffer_accel), 100);
	  HAL_UART_Transmit(&huart2, (uint8_t*)uart_buffer_gyro, strlen(uart_buffer_gyro), 100);
	    // Send formatted data for Teleplot
//	    sprintf(uart_buffer_accel, ">accel_z:%.2f\n", accel_z_g);
//	    sprintf(uart_buffer_gyro, ">gyro_z:%.2f\n", gyro_z_dps);
//
//	    // Transmit data over UART
//	    HAL_UART_Transmit(&huart2, (uint8_t*)uart_buffer_accel, strlen(uart_buffer_accel), 100);
//	    HAL_Delay(200);  // Adjust delay for your data rate
//	    HAL_UART_Transmit(&huart2, (uint8_t*)uart_buffer_gyro, strlen(uart_buffer_gyro), 100);
//	  uint8_t CTRL3_C = 0x1;
//	  HAL_I2C_Mem_Write(&hi2c1,(0b01101010 << 1) , CTRL3_C, 1, &CTRL3_C, 1, 100);
	  HAL_Delay(200);
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_5;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART2
                              |RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
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
  /* USER CODE BEGIN I2C1_Init 0 */
  /* USER CODE END I2C1_Init 0 */
  /* USER CODE BEGIN I2C1_Init 1 */
  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00000608;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */
  /* USER CODE END I2C1_Init 2 */
}

static void MX_USART2_UART_Init(void)
{
  /* USER CODE BEGIN USART2_Init 0 */
  /* USER CODE END USART2_Init 0 */
  /* USER CODE BEGIN USART2_Init 1 */
  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */
  /* USER CODE END USART2_Init 2 */
}
/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);
  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);
/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
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
