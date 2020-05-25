/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
	
uint8_t hexToAscii(uint8_t n)//4-bit hex value converted to an ascii character
{
if (n>=0 && n<=9) n = n + '0';
else n = n - 10 + 'A';
return n;
}
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
  MX_USART1_UART_Init();

	int READY = 1; //
	char Request='\0'; //based on the request either DATE/TIME , TURN LED ON , TURN LED OFF

 if (HAL_I2C_IsDeviceReady(&hi2c1, 0xD0, 10, HAL_MAX_DELAY) == HAL_OK)
	{
		for (int i = 1; i<=10;i++) // indicator of ready device
		{
			HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
			HAL_Delay(250);
		}
	}
	uint8_t secbuffer [2], minbuffer [2], hourbuffer [2];
	
	secbuffer[0] = 0x00; //register address
	secbuffer[1] = 0x00; //data to put in register 0 SEC
	HAL_I2C_Master_Transmit(&hi2c1, 0xD0, secbuffer, 2, 10);
	
	minbuffer[0] = 0x01; //register address
	minbuffer[1] = 0x00; //data to put in register  O MIN 
	HAL_I2C_Master_Transmit(&hi2c1, 0xD0, minbuffer, 2, 10);
	
	hourbuffer[0] = 0x02; //register address
	hourbuffer[1] = 0x09; //data to put in register  00001001
	HAL_I2C_Master_Transmit(&hi2c1, 0xD0, hourbuffer, 2, 10);
	
	
    
	  uint8_t DAY [2];
    DAY[0] = 0x03; //register address
    DAY[1] = 0x03; //data to put in register tuesday=3
    HAL_I2C_Master_Transmit(&hi2c1, 0xD0, DAY, 2, 10);
    
		uint8_t DATE[2];
    DATE[0] = 0x04; //register address
    DATE[1] = 0x26; //data to put in register date=26 so 00(10)0110    10=20days so remaining 6
    HAL_I2C_Master_Transmit(&hi2c1, 0xD0, DATE, 2, 10);
    
		uint8_t MONTH_CENTURY [2];
    MONTH_CENTURY[0] = 0x05; //register address
    MONTH_CENTURY[1] = 0x05; //data to put in register MAY= 5
    HAL_I2C_Master_Transmit(&hi2c1, 0xD0, MONTH_CENTURY, 2, 10);
    
		uint8_t  YEAR [2];
		YEAR[0] = 0x06; //register address
    YEAR[1] = 0x20; //data to put in register 00100000 
    HAL_I2C_Master_Transmit(&hi2c1, 0xD0, YEAR, 2, 10);
	
	
	//Receive via I2C and forward to UART  
	uint8_t out[] = {'D','A','Y','=',0,0,',',0,0,'-',0,0,'-',0,0,'\r','\n','T','I','M','E','=',0,0,':',0,0,':',0,0,'\r','\n'};
   while (1)
   {
    
		HAL_I2C_Master_Transmit(&hi2c1, 0xD0, secbuffer, 1, 10);
		HAL_I2C_Master_Receive(&hi2c1, 0xD1, secbuffer+1, 1, 10);
		out[28] = hexToAscii(secbuffer[1] >> 4 );
		out[29] = hexToAscii(secbuffer[1] & 0x0F );
        
		HAL_I2C_Master_Transmit(&hi2c1, 0xD0, minbuffer, 1, 10);
		HAL_I2C_Master_Receive(&hi2c1, 0xD1, minbuffer+1, 1, 10);
		out[25] = hexToAscii(minbuffer[1] >> 4 );
		out[26] = hexToAscii(minbuffer[1] & 0x0F );

        
		HAL_I2C_Master_Transmit(&hi2c1, 0xD0, hourbuffer, 1, 10);
		HAL_I2C_Master_Receive(&hi2c1, 0xD1, hourbuffer+1, 1, 10);
		out[22] = hexToAscii(hourbuffer[1] >> 4 );
		out[23] = hexToAscii(hourbuffer[1] & 0x0F);
         
    HAL_I2C_Master_Transmit(&hi2c1, 0xD0, YEAR, 1, 10);
    HAL_I2C_Master_Receive(&hi2c1, 0xD1, YEAR+1, 1, 10);
    out[13] = hexToAscii(YEAR[1] >> 4 );
    out[14] = hexToAscii(YEAR[1] & 0x0F);
        
    HAL_I2C_Master_Transmit(&hi2c1, 0xD0, MONTH_CENTURY, 1, 10);
    HAL_I2C_Master_Receive(&hi2c1, 0xD1, MONTH_CENTURY+1, 1, 10);
    out[10] = hexToAscii(MONTH_CENTURY[1] >> 4 );
    out[11] = hexToAscii(MONTH_CENTURY[1] & 0x0F);
        
    HAL_I2C_Master_Transmit(&hi2c1, 0xD0, DATE, 1, 10);
    HAL_I2C_Master_Receive(&hi2c1, 0xD1, DATE+1, 1, 10);
    out[7] = hexToAscii(DATE[1] >> 4 );
    out[8] = hexToAscii(DATE[1] & 0x0F);    
        
    HAL_I2C_Master_Transmit(&hi2c1, 0xD0, DAY, 1, 10);
    HAL_I2C_Master_Receive(&hi2c1, 0xD1, DAY+1, 1, 10);
    out[4] = hexToAscii(DAY[1] >> 4 );
    out[5] = hexToAscii(DAY[1] & 0x0F);
        

HAL_UART_Receive(&huart1,&Request, sizeof(Request),50); //receiving the request 
		
		if (Request == 'D')
		{
			HAL_UART_Transmit(&huart1,out, sizeof(out),500); //display the date/time
			Request = '\0';
		}
		
		else if (Request == 'O')			
		{
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13,0); //on
			Request = '\0';
		}
		
		else if (Request == 'F')				
		{
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13,1); //off
			Request = '\0';
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

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
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
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
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
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

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

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

 /**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
