/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include <stdlib.h>
#include <stdio.h>

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
ADC_HandleTypeDef hadc;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

uint16_t pot1;
uint16_t pot2;
int temp;
float vsense = 3.3/4095.0;

float arr[3];
#define maxsize 10
float values[maxsize];
int pot2_arr[5];
int temp_arr[5];

// queue node
struct node
{
	int data;
	struct node *next;
};

//front ve rear
struct node *front = NULL;
struct node *rear  = NULL;

//kuyruga veri ekleme enqueu
int enqueu(int val)
{
	//kuyrugun bo?? olma durumu
	if(front == NULL )
	{
		struct node *new = (struct node*)malloc(sizeof(struct node));
		new ->data = val;
		new ->next = NULL;

		// kuyrugun ba????da sonuda ayn?? eri g??stermeli
		front = rear = new;
	}
	//kuyruk bo?? de??ilse
	else {
		struct node *new = (struct node*)malloc(sizeof(struct node));
		new ->data = val;
		new ->next = NULL;

		rear->next = new;
		rear = new;

	}

}

//kuyrukdan veri c??karma
int dequeu()
{
	//ilk giren ilk c??kan FIFO olucak - parametre yok

	//kuyrugun bos olam durumu
	if(front == NULL)
	{
		printf('BOSSS CIKAMAZZ');
		return 1;
	}
	else {
		struct node* temp = front;
		front = front->next;
		free(temp);
	}
}


int display()
{
	int count = 0;
	if (front == NULL) {
		printf('BSS');
		return 0;
	}
	struct node* index = front;
	while(index!=NULL){
		printf('%d -- ', index->data);
		index  = index->next;
		count++;
	}
	return count;


}

float gettemp(float value)
{
	return (((value*vsense)-0.76)/0.0025)+25;
}

int movingAvg(float newVal, float* array)
{


}


float Read_ADC(float channel)
{
	  HAL_ADC_DeInit(&hadc);
	  ADC_ChannelConfTypeDef sConfig = {0};
	  hadc.Instance = ADC1;
	  hadc.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
	  hadc.Init.Resolution = ADC_RESOLUTION_12B;
	  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	  hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
	  hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	  hadc.Init.LowPowerAutoWait = DISABLE;
	  hadc.Init.LowPowerAutoPowerOff = DISABLE;
	  hadc.Init.ContinuousConvMode = ENABLE;
	  hadc.Init.DiscontinuousConvMode = DISABLE;
	  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	  hadc.Init.DMAContinuousRequests = DISABLE;
	  hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
	  if (HAL_ADC_Init(&hadc) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  sConfig.Channel = channel;
	  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
	  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
	  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  else
	  {
		  HAL_ADC_Start(&hadc);
		  HAL_ADC_PollForConversion(&hadc, 100);
		  HAL_Delay(100);
		  float val = HAL_ADC_GetValue(&hadc);
		  float sum = 0;
		  for (int i = 0; i < maxsize; i++)
		  {
			values[i] = val;
		  }

		  for(int i = 0; i < maxsize-1; i++)
		  {
			  values[i] = values[i+1];
			  sum +=values[i];
		  }
		  values[maxsize-1] = val;
		  return ((sum+values[maxsize-1])/10);
	  }
}



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
  MX_ADC_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  arr[0] = Read_ADC(ADC_CHANNEL_5);
	  arr[1] = Read_ADC(ADC_CHANNEL_6);
	  arr[2] = gettemp(Read_ADC(ADC_CHANNEL_TEMPSENSOR));

	  HAL_ADC_Stop(&hadc);

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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSI14;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSI14State = RCC_HSI14_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.HSI14CalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC_Init(void)
{

  /* USER CODE BEGIN ADC_Init 0 */

  /* USER CODE END ADC_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC_Init 1 */

  /* USER CODE END ADC_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc.Instance = ADC1;
  hadc.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
  hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc.Init.LowPowerAutoWait = DISABLE;
  hadc.Init.LowPowerAutoPowerOff = DISABLE;
  hadc.Init.ContinuousConvMode = ENABLE;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.DMAContinuousRequests = DISABLE;
  hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  if (HAL_ADC_Init(&hadc) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel to be converted.
  */


  /** Configure for the selected ADC regular channel to be converted.
  */

  /** Configure for the selected ADC regular channel to be converted.
  */

  /* USER CODE BEGIN ADC_Init 2 */

  /* USER CODE END ADC_Init 2 */

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
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, LD4_Pin|LD3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD4_Pin LD3_Pin */
  GPIO_InitStruct.Pin = LD4_Pin|LD3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

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
