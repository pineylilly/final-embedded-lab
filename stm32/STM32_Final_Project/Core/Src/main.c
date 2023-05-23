/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include <string.h>
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
ADC_HandleTypeDef hadc1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

osThreadId ControlHandle;
osThreadId Thermal_and_HumHandle;
osThreadId DustHandle;
osThreadId UltrasonicHandle;
osThreadId AverageHandle;
osThreadId nodeMCUsendHandle;
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM1_Init(void);
static void MX_USART1_UART_Init(void);
void StartControl(void const * argument);
void StartThermal_and_Humidity(void const * argument);
void StartDust(void const * argument);
void StartUltrasonic(void const * argument);
void StartAverage(void const * argument);
void StartnodeMCUsend(void const * argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//Control
	int autoMode = 1;
	int pwmFan = 0;
	int pwmWater = 0;
	int pwmTem = 0;
	int pwmDust = 0;
	int movement = 300; //no movement after 60s turn off fan and water check in range 50cm

//Average
	int aveCount = 0;
	float sumDust  = 0;
	float aveDust  = 0;

//Ultrasonic
	#define TRIG_PIN GPIO_PIN_13
	#define TRIG_PORT GPIOB
	#define ECHO_PIN GPIO_PIN_15
	#define ECHO_PORT GPIOB
	uint32_t pMillis_U;
	uint32_t Value1 = 0;
	uint32_t Value2 = 0;
	float Distance  = 0;  // cm

//Dust
	float voMeasured = 0;
	float calcVoltage = 0;
	float dustDensity = 0;

//Thermal_and_Humidity
	#define DHT11_PORT GPIOC
	#define DHT11_PIN GPIO_PIN_10
	uint8_t RHI, RHD, TCI, TCD, SUM;
	uint32_t pMillis_T, cMillis_T;
	float tCelsius = 0;
	float tFahrenheit = 0;
	float RH = 0;

	void microDelay (uint16_t delay)
	{
	  __HAL_TIM_SET_COUNTER(&htim1, 0);
	  while (__HAL_TIM_GET_COUNTER(&htim1) < delay);
	}

	uint8_t DHT11_Start (void)
	{
	  uint8_t Response = 0;
	  GPIO_InitTypeDef GPIO_InitStructPrivate = {0};
	  GPIO_InitStructPrivate.Pin = DHT11_PIN;
	  GPIO_InitStructPrivate.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStructPrivate.Speed = GPIO_SPEED_FREQ_LOW;
	  GPIO_InitStructPrivate.Pull = GPIO_NOPULL;
	  HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStructPrivate); // set the pin as output
	  HAL_GPIO_WritePin (DHT11_PORT, DHT11_PIN, 0);   // pull the pin low
	  HAL_Delay(20);   // wait for 20ms
	  HAL_GPIO_WritePin (DHT11_PORT, DHT11_PIN, 1);   // pull the pin high
	  microDelay (30);   // wait for 30us
	  GPIO_InitStructPrivate.Mode = GPIO_MODE_INPUT;
	  GPIO_InitStructPrivate.Pull = GPIO_PULLUP;
	  HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStructPrivate); // set the pin as input
	  microDelay (40);
	  if (!(HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)))
	  {
	    microDelay (80);
	    if ((HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN))) Response = 1;
	  }
	  pMillis_T = HAL_GetTick();
	  cMillis_T = HAL_GetTick();
	  while ((HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)) && pMillis_T + 2 > cMillis_T)
	  {
	    cMillis_T = HAL_GetTick();
	  }
	  return Response;
	}

	uint8_t DHT11_Read (void)
	{
	  uint8_t a,b;
	  for (a=0;a<8;a++)
	  {
	    pMillis_T = HAL_GetTick();
	    cMillis_T = HAL_GetTick();
	    while (!(HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)) && pMillis_T + 2 > cMillis_T)
	    {  // wait for the pin to go high
	      cMillis_T = HAL_GetTick();
	    }
	    microDelay (40);   // wait for 40 us
	    if (!(HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)))   // if the pin is low
	      b&= ~(1<<(7-a));
	    else
	      b|= (1<<(7-a));
	    pMillis_T = HAL_GetTick();
	    cMillis_T = HAL_GetTick();
	    while ((HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)) && pMillis_T + 2 > cMillis_T)
	    {  // wait for the pin to go low
	      cMillis_T = HAL_GetTick();
	    }
	  }
	  return b;
	}


	//Node MCU
		char c;
		char receiveString[256] = "";
		char tempFilling[2] = {' ', '\0'};
		char sendingString[256] = "";
		int receiveIdx = 0;
		bool validInitial = false;

		void sendData() {
			strcpy(sendingString, "");
			sprintf(sendingString, "{\"data\":{\"dust\":%.1f,\"temperature\":%.1f,\"humidity\":%.1f}}", aveDust, tCelsius, RH);
			HAL_UART_Transmit_IT(&huart1, sendingString, sizeof(sendingString));
		}



		void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
			if (huart->Instance == USART1) {
				// HAL_UART_Transmit(&huart2, &c, sizeof(c), 1000);
				if (receiveIdx == 0) {
					// autoMode
					autoMode = (int)c;
				}
				else if (receiveIdx == 1) {
					// pwmFan
					pwmFan = (int)c;
				}
				else {
					// pwmWater
					pwmWater = (int)c;
					validInitial = true;
				}
				receiveIdx = (receiveIdx+1)%3;

				HAL_UART_Receive_IT(&huart1, &c, sizeof(c));
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
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  // Start receiving data from nodemcu
  HAL_UART_Receive_IT(&huart1, &c, sizeof(c));

  // Wait for initializing mode
  while (!validInitial) {
	  HAL_UART_Transmit(&huart1, "request", sizeof("request"), 2000);
	  HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
	  int cnt = 0;
	  while (++cnt < 2000) {
		  HAL_Delay(5);
		  if (validInitial) break;
	  }
  }

  HAL_TIM_Base_Start(&htim1);

  //Ultrasonic
  HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET);  // pull the TRIG pin low

  //PWM
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1); //PA6
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1); //PB6
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, 1);


  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of Control */
  osThreadDef(Control, StartControl, osPriorityNormal, 0, 128);
  ControlHandle = osThreadCreate(osThread(Control), NULL);

  /* definition and creation of Thermal_and_Hum */
  osThreadDef(Thermal_and_Hum, StartThermal_and_Humidity, osPriorityNormal, 0, 128);
  Thermal_and_HumHandle = osThreadCreate(osThread(Thermal_and_Hum), NULL);

  /* definition and creation of Dust */
  osThreadDef(Dust, StartDust, osPriorityNormal, 0, 128);
  DustHandle = osThreadCreate(osThread(Dust), NULL);

  /* definition and creation of Ultrasonic */
  osThreadDef(Ultrasonic, StartUltrasonic, osPriorityNormal, 0, 128);
  UltrasonicHandle = osThreadCreate(osThread(Ultrasonic), NULL);

  /* definition and creation of Average */
  osThreadDef(Average, StartAverage, osPriorityNormal, 0, 128);
  AverageHandle = osThreadCreate(osThread(Average), NULL);

  /* definition and creation of nodeMCUsend */
  osThreadDef(nodeMCUsend, StartnodeMCUsend, osPriorityNormal, 0, 1024);
  nodeMCUsendHandle = osThreadCreate(osThread(nodeMCUsend), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  osKernelStart();
  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 83;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 0;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 65535;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */
  HAL_TIM_MspPostInit(&htim4);

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
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
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
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);

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

  /*Configure GPIO pin : PB13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PC10 */
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PD2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartControl */
/**
  * @brief  Function implementing the Control thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartControl */
void StartControl(void const * argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
	  if(autoMode==1){
		  //fan tem=25 or dust=5
		  if(tCelsius>25 && pwmTem<50){
			  pwmTem=pwmTem+1;
		  }else if(tCelsius<=25){
			  pwmTem=0;
		  }
		  if(aveDust>5 && pwmDust<50){
			  pwmDust=pwmDust+1;
		  }else if(aveDust<=5){
			  pwmDust=0;
		  }
		  //pwmFan = max(pwmDust,pwmTem)
		  if(pwmDust>=pwmTem) pwmFan=50+pwmDust;
		  else pwmFan=50+pwmTem;

		  //water rh=80
		  if(RH<=75){
			  pwmWater=1;
		  }else if(RH>80){
			  pwmWater=0;
		  }

		  //check movement  จะsetค่าโชว์ดีมั้ย
		  if(movement>0){
			  __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,(htim3.Init.Period)*pwmFan/100);
			  __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_1,(htim4.Init.Period)*(pwmWater*5)/100);
		  }
		  else{
			  __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,(htim3.Init.Period)*0/100);
			  __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_1,(htim4.Init.Period)*0/100);
		  }

	  }
	  else{
		  __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,(htim3.Init.Period)*pwmFan/100);
		  __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_1,(htim4.Init.Period)*(pwmWater*5)/100);
	  }
	  osDelay(500);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartThermal_and_Humidity */
/**
* @brief Function implementing the Thermal_and_Hum thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartThermal_and_Humidity */
void StartThermal_and_Humidity(void const * argument)
{
  /* USER CODE BEGIN StartThermal_and_Humidity */
  /* Infinite loop */
  for(;;)
  {
	    if(DHT11_Start())
	    {
	      RHI = DHT11_Read(); // Relative humidity integral
	      RHD = DHT11_Read(); // Relative humidity decimal
	      TCI = DHT11_Read(); // Celsius integral
	      TCD = DHT11_Read(); // Celsius decimal
	      SUM = DHT11_Read(); // Check sum
	      if (RHI + RHD + TCI + TCD == SUM)
	      {
	        // Can use RHI and TCI for any purposes if whole number only needed
	        tCelsius = (float)TCI + (float)(TCD/10.0);
	        tFahrenheit = tCelsius * 9/5 + 32;
	        RH = (float)RHI + (float)(RHD/10.0);
	        // Can use tCelsius, tFahrenheit and RH for any purposes
	      }
	    }
	    osDelay(200);
  }
  /* USER CODE END StartThermal_and_Humidity */
}

/* USER CODE BEGIN Header_StartDust */
/**
* @brief Function implementing the Dust thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDust */
void StartDust(void const * argument)
{
  /* USER CODE BEGIN StartDust */
  /* Infinite loop */
  for(;;)
  {
		 HAL_ADC_Start(&hadc1);

		 if (HAL_ADC_PollForConversion(&hadc1, 1000) == HAL_OK) {
			voMeasured = HAL_ADC_GetValue(&hadc1);

			// 0 - 5V mapped to 0 - 1023 integer values
			// recover voltage
			calcVoltage = voMeasured * (5.0 / 1024.0);

			// linear eqaution taken from http://www.howmuchsnow.com/arduino/airquality/
			// Chris Nafis (c) 2012
			dustDensity = 170 * calcVoltage - 0.1;

		  	osDelay(100);
		 }
  }
  /* USER CODE END StartDust */
}

/* USER CODE BEGIN Header_StartUltrasonic */
/**
* @brief Function implementing the Ultrasonic thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartUltrasonic */
void StartUltrasonic(void const * argument)
{
  /* USER CODE BEGIN StartUltrasonic */
  /* Infinite loop */
  for(;;)
  {
	    HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_SET);  // pull the TRIG pin HIGH
	    __HAL_TIM_SET_COUNTER(&htim1, 0);
	    while (__HAL_TIM_GET_COUNTER (&htim1) < 10);  // wait for 10 us
	    HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET);  // pull the TRIG pin low

	    pMillis_U = HAL_GetTick(); // used this to avoid infinite while loop  (for timeout)
	    // wait for the echo pin to go high
	    while (!(HAL_GPIO_ReadPin (ECHO_PORT, ECHO_PIN)) && pMillis_U + 10 >  HAL_GetTick());
	    Value1 = __HAL_TIM_GET_COUNTER (&htim1);

	    pMillis_U = HAL_GetTick(); // used this to avoid infinite while loop (for timeout)
	    // wait for the echo pin to go low
	    while ((HAL_GPIO_ReadPin (ECHO_PORT, ECHO_PIN)) && pMillis_U + 50 > HAL_GetTick());
	    Value2 = __HAL_TIM_GET_COUNTER (&htim1);

	    Distance = (Value2-Value1)* 0.034/2;
	    osDelay(200);
  }
  /* USER CODE END StartUltrasonic */
}

/* USER CODE BEGIN Header_StartAverage */
/**
* @brief Function implementing the Average thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartAverage */
void StartAverage(void const * argument)
{
  /* USER CODE BEGIN StartAverage */

	aveDust=dustDensity;

  /* Infinite loop */
  for(;;)
  {
		if(aveCount<50){
			aveCount=aveCount+1;
			sumDust=sumDust+dustDensity;
		}
		else{
			aveCount = 0;
			aveDust  = sumDust/50.0;

			sumDust  = 0;
		}

	    //check movement
	    if(Distance <= 50) movement=300;
	    else movement=movement-1;
	    if(movement<0) movement=0;

	    osDelay(200);
  }
  /* USER CODE END StartAverage */
}

/* USER CODE BEGIN Header_StartnodeMCUsend */
/**
* @brief Function implementing the nodeMCUsend thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartnodeMCUsend */
void StartnodeMCUsend(void const * argument)
{
  /* USER CODE BEGIN StartnodeMCUsend */
  /* Infinite loop */
  for(;;)
  {
	  osDelay(30000);
	  sendData();
  }
  /* USER CODE END StartnodeMCUsend */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM11 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM11) {
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
