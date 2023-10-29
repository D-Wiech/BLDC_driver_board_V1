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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "STM32_Cordic.h"
#include "math.h"
#include "string.h"
#include "stdio.h"
#include "PID.h"
#include "hardware_Parameter.h"
#include "angleinterpolation.h"

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
ADC_HandleTypeDef hadc2;
ADC_HandleTypeDef hadc3;
DMA_HandleTypeDef hdma_adc1;
DMA_HandleTypeDef hdma_adc2;
DMA_HandleTypeDef hdma_adc3;

COMP_HandleTypeDef hcomp1;

DAC_HandleTypeDef hdac1;

DTS_HandleTypeDef hdts;

FDCAN_HandleTypeDef hfdcan1;

UART_HandleTypeDef hlpuart1;
UART_HandleTypeDef huart4;

SPI_HandleTypeDef hspi2;
SPI_HandleTypeDef hspi3;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim8;

PCD_HandleTypeDef hpcd_USB_OTG_HS;

/* USER CODE BEGIN PV */

static uint16_t ADC1_result[3] = {0};
static uint16_t ADC2_result[3] = {0};
static uint16_t ADC3_result[3] = {0};
static const float Bit16_to_Volt = 5.0354772259098191805905241474022e-5f;
static const float Bit12_to_Volt = 8.0586080586080586080586080586081e-4f;

static uint16_t SENSOR_HALL_DATA = 0;
uint16_t NEW_SENSOR_DATA = 0;//Indicates new sensor data

typedef struct
{
	float electrical_angle;// Electrical angle of the rotor
	float Id_target;
	float Iq_target;
	float IP_U;//Phase current U
	float IP_V;//Phase current V
	float IP_W;//Phase current W
	float PWM_U_Dutycycle;
	float PWM_V_Dutycycle;
	float PWM_W_Dutycycle;
}FOC_Values;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_CORDIC_Init(void);
static void MX_DAC1_Init(void);
static void MX_FDCAN1_Init(void);
static void MX_LPUART1_UART_Init(void);
static void MX_UART4_Init(void);
static void MX_SPI2_Init(void);
static void MX_SPI3_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM8_Init(void);
static void MX_USB_OTG_HS_PCD_Init(void);
static void MX_ADC1_Init(void);
static void MX_ADC3_Init(void);
static void MX_COMP1_Init(void);
static void MX_ADC2_Init(void);
static void MX_DTS_Init(void);
/* USER CODE BEGIN PFP */

HAL_StatusTypeDef debug_uart4_write_text(uint8_t *pData);
HAL_StatusTypeDef debug_uart4_write_float(float number);
HAL_StatusTypeDef Send_Command_Driver(uint16_t TX, uint16_t *RX, uint16_t length);
HAL_StatusTypeDef Driver_set_up(void);
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc);
void HAL_ADCEx_EndOfSamplingCallback(ADC_HandleTypeDef *hadc);
float ShuntVolts_to_Amps(float VMeas);
float Volts_to_HalfbridgeVolts(float VMeas);
HAL_StatusTypeDef Send_Command_Sensor(uint16_t TX, uint16_t *RX, uint16_t length);
float Convert_HALL_Sensor_Data(uint16_t Data);
void Clark_Transform(float I_U, float I_V, float I_W, float* I_Alpha, float* I_Beta);
void Inv_Park_Transform(float V_D, float V_Q, float Phi, float* V_Alpha, float* V_Beta);
void Park_Transform(float I_Alpha, float I_Beta, float Phi, float* I_Q, float* I_D);
void Inv_Clark_Transform(float V_Alpha, float V_Beta, float* V_R1, float* V_R2, float* V_R3);
void Calc_SVPWM_amp_angle(float Angle, float Amplitude, float Supply_voltage, float* PWM_U, float* PWM_V, float* PWM_W);
void Calc_SVPWM_Alpha_Beta(float V_Alpha, float V_Beta, float* PWM_U, float* PWM_V, float* PWM_W);
float float_low_pass_filter(float Alpha, float Val, float Val_prev);
HAL_StatusTypeDef Currentmode_FOC(FOC_Values *Values, PIDController *pid_I_D, PIDController *pid_I_Q );
void Current_Mode_PID_Controller_set_up(PIDController *pid_I_D, PIDController *pid_I_Q);
void Hall_sensor_calibration_val_gen(Motor_Parameter *MParam);
uint16_t proportinal_PWM_to_act_val(float PWM_val);
void Hall_sensor_calibration_look_up_table_gen(Motor_Parameter *MParam, float* Look_up_table);
float Convert_HALL_Sensor_Data_to_Electrical_angle(uint16_t Data, Motor_Parameter *MParam);
void Controll_Data_preparation(FOC_Values *Controll_Values,Motor_Parameter *MParam, Controller_Hardware_Parameter *CAL_Values
		);
void set_PWM_Half_bridges(FOC_Values *Controll_Values);
void Current_sensor_calibration(Controller_Hardware_Parameter *CAL_Values);


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

/* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_CORDIC_Init();
  MX_DAC1_Init();
  MX_FDCAN1_Init();
  MX_LPUART1_UART_Init();
  MX_UART4_Init();
  MX_SPI2_Init();
  MX_SPI3_Init();
  MX_TIM1_Init();
  MX_TIM4_Init();
  MX_TIM8_Init();
  MX_USB_OTG_HS_PCD_Init();
  MX_ADC1_Init();
  MX_ADC3_Init();
  MX_COMP1_Init();
  MX_ADC2_Init();
  MX_DTS_Init();
  /* USER CODE BEGIN 2 */

  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1); //Start USER LED

  Driver_set_up();

  // calibrate the ADCs for better accuracy and start it w/ interrupt
  HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
  HAL_ADCEx_Calibration_Start(&hadc2, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
  HAL_ADCEx_Calibration_Start(&hadc3, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);

  PIDController pid_I_D;
  PIDController pid_I_Q;
  Current_Mode_PID_Controller_set_up(&pid_I_D, &pid_I_Q);

  HAL_ADC_Start_DMA(&hadc1, ADC1_result, 3);
  HAL_ADC_Start_DMA(&hadc2, ADC2_result, 3);
  HAL_ADC_Start_DMA(&hadc3, ADC3_result, 3);

  //Starten der Motor PWM erzeugung
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1); //U-Positiv
  HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1); //U-Negativ
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2); //V-Positiv
  HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2); //V-Negativ
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3); //W-Positiv
  HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3); //W-Negativ

  //CCR Max value = 3999
  TIM1->CCR1 = 2000;
  TIM1->CCR2 = 2000;
  TIM1->CCR3 = 2000;

  __HAL_TIM_ENABLE_IT(&htim1, TIM_IT_BREAK);


  //Starten des digitalen Temperatursensors
  HAL_DTS_Start(&hdts);

  static int32_t Temperature;

  HAL_Delay(1);

  Motor_Parameter Test_Motor;
  Test_Motor.Pole_pair_count = Pole_pair_count;
  FOC_Values Controll_data;

  Controller_Hardware_Parameter HW_Param;
  Current_sensor_calibration(&HW_Param);

  //Hall_sensor_calibration_val_gen(&Test_Motor);
  //float LUT_Angle_sensor[365] = {0};
  //Hall_sensor_calibration_look_up_table_gen(&Test_Motor,LUT_Angle_sensor);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  /*
	  //Gets the die Temperature and sends it over UART
	  HAL_DTS_GetTemperature(&hdts, &Temperature);

	  debug_uart4_write_text("Der STM32H7 hat folgende Temperatur: ");
	  debug_uart4_write_float((float)Temperature);
	  debug_uart4_write_text("°\n");

	  debug_uart4_write_text("Strom in Phase U: ");
	  debug_uart4_write_float(ShuntVolts_to_Amps((float)ADC1_result[1] * Bit16_to_Volt));
	  debug_uart4_write_text("A; Phase V: ");
	  debug_uart4_write_float(ShuntVolts_to_Amps((float)ADC1_result[0] * Bit16_to_Volt));
	  debug_uart4_write_text("A; Phase W: ");
	  debug_uart4_write_float(ShuntVolts_to_Amps((float)ADC2_result[0] * Bit16_to_Volt));
	  debug_uart4_write_text("A; NTC Spannung: ");
	  debug_uart4_write_float((float)ADC3_result[1] * Bit12_to_Volt);
	  debug_uart4_write_text("V \n");

	  debug_uart4_write_text("Spannung: Phase U: ");
	  debug_uart4_write_float(Volts_to_HalfbridgeVolts((float)ADC2_result[1] * Bit16_to_Volt));
	  debug_uart4_write_text("V; Phase V: ");
	  debug_uart4_write_float(Volts_to_HalfbridgeVolts((float)ADC1_result[2] * Bit16_to_Volt));
	  debug_uart4_write_text("V; Phase W: ");
	  debug_uart4_write_float(Volts_to_HalfbridgeVolts((float)ADC2_result[2] * Bit16_to_Volt));
	  debug_uart4_write_text("V; Versorgungsspannung: ");
	  debug_uart4_write_float(Volts_to_HalfbridgeVolts((float)ADC3_result[0] * Bit12_to_Volt));
	  debug_uart4_write_text("V \n");

	  debug_uart4_write_text("HALL Sensor Daten: ");
	  debug_uart4_write_float(Convert_HALL_Sensor_Data(SENSOR_HALL_DATA));
	  debug_uart4_write_text("\n");

		const uint16_t WRITE_BIT = 0;
		const uint16_t READ_BIT = (1<<15);
		const uint16_t FAULT_STATUS1_REG = 0;
		const uint16_t FAULT_STATUS2_REG = 0x0800;

		uint16_t RECEIVE_MSG = 0;

		//Check whether the drive is connected
		Send_Command_Driver((READ_BIT | FAULT_STATUS1_REG), &RECEIVE_MSG, 1);
		debug_uart4_write_text("Driver FAULT1: ");
		debug_uart4_write_float((float)RECEIVE_MSG);
		Send_Command_Driver((READ_BIT | FAULT_STATUS2_REG), &RECEIVE_MSG, 1);
		debug_uart4_write_text(" FAULT2: ");
		debug_uart4_write_float((float)RECEIVE_MSG);
		debug_uart4_write_text(" \n");
		*/

	  if(NEW_SENSOR_DATA == 1)
	  {
		  Controll_Data_preparation(&Controll_data, &Test_Motor, &HW_Param);
		  Currentmode_FOC(&Controll_data, &pid_I_D, &pid_I_Q);
		  set_PWM_Half_bridges(&Controll_data);
	  }

	  HAL_Delay(1000);

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

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 50;
  RCC_OscInitStruct.PLL.PLLP = 1;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInitStruct.PLL2.PLL2M = 1;
  PeriphClkInitStruct.PLL2.PLL2N = 24;
  PeriphClkInitStruct.PLL2.PLL2P = 2;
  PeriphClkInitStruct.PLL2.PLL2Q = 2;
  PeriphClkInitStruct.PLL2.PLL2R = 2;
  PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_3;
  PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
  PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
  PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_PLL2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
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

  ADC_MultiModeTypeDef multimode = {0};
  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc1.Init.Resolution = ADC_RESOLUTION_16B;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 3;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIG_T1_TRGO;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
  hadc1.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
  hadc1.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the ADC multi-mode
  */
  multimode.Mode = ADC_MODE_INDEPENDENT;
  if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_16;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_8CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  sConfig.OffsetSignedSaturation = DISABLE;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_17;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_15;
  sConfig.Rank = ADC_REGULAR_RANK_3;
  sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief ADC2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC2_Init(void)
{

  /* USER CODE BEGIN ADC2_Init 0 */

  /* USER CODE END ADC2_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC2_Init 1 */

  /* USER CODE END ADC2_Init 1 */

  /** Common config
  */
  hadc2.Instance = ADC2;
  hadc2.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc2.Init.Resolution = ADC_RESOLUTION_16B;
  hadc2.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc2.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  hadc2.Init.LowPowerAutoWait = DISABLE;
  hadc2.Init.ContinuousConvMode = DISABLE;
  hadc2.Init.NbrOfConversion = 3;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConv = ADC_EXTERNALTRIG_T1_TRGO;
  hadc2.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
  hadc2.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR;
  hadc2.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc2.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
  hadc2.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_9;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_8CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  sConfig.OffsetSignedSaturation = DISABLE;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_11;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_14;
  sConfig.Rank = ADC_REGULAR_RANK_3;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC2_Init 2 */

  /* USER CODE END ADC2_Init 2 */

}

/**
  * @brief ADC3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC3_Init(void)
{

  /* USER CODE BEGIN ADC3_Init 0 */

  /* USER CODE END ADC3_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC3_Init 1 */

  /* USER CODE END ADC3_Init 1 */

  /** Common config
  */
  hadc3.Instance = ADC3;
  hadc3.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc3.Init.Resolution = ADC_RESOLUTION_12B;
  hadc3.Init.DataAlign = ADC3_DATAALIGN_RIGHT;
  hadc3.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc3.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  hadc3.Init.LowPowerAutoWait = DISABLE;
  hadc3.Init.ContinuousConvMode = DISABLE;
  hadc3.Init.NbrOfConversion = 3;
  hadc3.Init.DiscontinuousConvMode = DISABLE;
  hadc3.Init.ExternalTrigConv = ADC_EXTERNALTRIG_T1_TRGO;
  hadc3.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
  hadc3.Init.DMAContinuousRequests = DISABLE;
  hadc3.Init.SamplingMode = ADC_SAMPLING_MODE_NORMAL;
  hadc3.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR;
  hadc3.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc3.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
  hadc3.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc3) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_10;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC3_SAMPLETIME_6CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  sConfig.OffsetSign = ADC3_OFFSET_SIGN_NEGATIVE;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
  sConfig.Rank = ADC_REGULAR_RANK_3;
  sConfig.SamplingTime = ADC3_SAMPLETIME_2CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC3_Init 2 */

  /* USER CODE END ADC3_Init 2 */

}

/**
  * @brief COMP1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_COMP1_Init(void)
{

  /* USER CODE BEGIN COMP1_Init 0 */

  /* USER CODE END COMP1_Init 0 */

  /* USER CODE BEGIN COMP1_Init 1 */

  /* USER CODE END COMP1_Init 1 */
  hcomp1.Instance = COMP1;
  hcomp1.Init.InvertingInput = COMP_INPUT_MINUS_IO2;
  hcomp1.Init.NonInvertingInput = COMP_INPUT_PLUS_IO2;
  hcomp1.Init.OutputPol = COMP_OUTPUTPOL_NONINVERTED;
  hcomp1.Init.Hysteresis = COMP_HYSTERESIS_NONE;
  hcomp1.Init.BlankingSrce = COMP_BLANKINGSRC_NONE;
  hcomp1.Init.Mode = COMP_POWERMODE_HIGHSPEED;
  hcomp1.Init.WindowMode = COMP_WINDOWMODE_DISABLE;
  hcomp1.Init.TriggerMode = COMP_TRIGGERMODE_NONE;
  if (HAL_COMP_Init(&hcomp1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN COMP1_Init 2 */

  /* USER CODE END COMP1_Init 2 */

}

/**
  * @brief CORDIC Initialization Function
  * @param None
  * @retval None
  */
static void MX_CORDIC_Init(void)
{

  /* USER CODE BEGIN CORDIC_Init 0 */

  /* USER CODE END CORDIC_Init 0 */

  /* Peripheral clock enable */
  __HAL_RCC_CORDIC_CLK_ENABLE();

  /* USER CODE BEGIN CORDIC_Init 1 */

  /* USER CODE END CORDIC_Init 1 */

  /* nothing else to be configured */

  /* USER CODE BEGIN CORDIC_Init 2 */

  /* USER CODE END CORDIC_Init 2 */

}

/**
  * @brief DAC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_DAC1_Init(void)
{

  /* USER CODE BEGIN DAC1_Init 0 */

  /* USER CODE END DAC1_Init 0 */

  DAC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN DAC1_Init 1 */

  /* USER CODE END DAC1_Init 1 */

  /** DAC Initialization
  */
  hdac1.Instance = DAC1;
  if (HAL_DAC_Init(&hdac1) != HAL_OK)
  {
    Error_Handler();
  }

  /** DAC channel OUT1 config
  */
  sConfig.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE;
  sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  sConfig.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_DISABLE;
  sConfig.DAC_UserTrimming = DAC_TRIMMING_FACTORY;
  if (HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DAC1_Init 2 */

  /* USER CODE END DAC1_Init 2 */

}

/**
  * @brief DTS Initialization Function
  * @param None
  * @retval None
  */
static void MX_DTS_Init(void)
{

  /* USER CODE BEGIN DTS_Init 0 */

  /* USER CODE END DTS_Init 0 */

  /* USER CODE BEGIN DTS_Init 1 */

  /* USER CODE END DTS_Init 1 */
  hdts.Instance = DTS;
  hdts.Init.QuickMeasure = DTS_QUICKMEAS_DISABLE;
  hdts.Init.RefClock = DTS_REFCLKSEL_PCLK;
  hdts.Init.TriggerInput = DTS_TRIGGER_HW_NONE;
  hdts.Init.SamplingTime = DTS_SMP_TIME_15_CYCLE;
  hdts.Init.Divider = 0;
  hdts.Init.HighThreshold = 0x0;
  hdts.Init.LowThreshold = 0x0;
  if (HAL_DTS_Init(&hdts) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DTS_Init 2 */

  /* USER CODE END DTS_Init 2 */

}

/**
  * @brief FDCAN1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_FDCAN1_Init(void)
{

  /* USER CODE BEGIN FDCAN1_Init 0 */

  /* USER CODE END FDCAN1_Init 0 */

  /* USER CODE BEGIN FDCAN1_Init 1 */

  /* USER CODE END FDCAN1_Init 1 */
  hfdcan1.Instance = FDCAN1;
  hfdcan1.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
  hfdcan1.Init.Mode = FDCAN_MODE_NORMAL;
  hfdcan1.Init.AutoRetransmission = DISABLE;
  hfdcan1.Init.TransmitPause = DISABLE;
  hfdcan1.Init.ProtocolException = DISABLE;
  hfdcan1.Init.NominalPrescaler = 16;
  hfdcan1.Init.NominalSyncJumpWidth = 1;
  hfdcan1.Init.NominalTimeSeg1 = 2;
  hfdcan1.Init.NominalTimeSeg2 = 2;
  hfdcan1.Init.DataPrescaler = 1;
  hfdcan1.Init.DataSyncJumpWidth = 1;
  hfdcan1.Init.DataTimeSeg1 = 1;
  hfdcan1.Init.DataTimeSeg2 = 1;
  hfdcan1.Init.MessageRAMOffset = 0;
  hfdcan1.Init.StdFiltersNbr = 0;
  hfdcan1.Init.ExtFiltersNbr = 0;
  hfdcan1.Init.RxFifo0ElmtsNbr = 0;
  hfdcan1.Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_8;
  hfdcan1.Init.RxFifo1ElmtsNbr = 0;
  hfdcan1.Init.RxFifo1ElmtSize = FDCAN_DATA_BYTES_8;
  hfdcan1.Init.RxBuffersNbr = 0;
  hfdcan1.Init.RxBufferSize = FDCAN_DATA_BYTES_8;
  hfdcan1.Init.TxEventsNbr = 0;
  hfdcan1.Init.TxBuffersNbr = 0;
  hfdcan1.Init.TxFifoQueueElmtsNbr = 0;
  hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
  hfdcan1.Init.TxElmtSize = FDCAN_DATA_BYTES_8;
  if (HAL_FDCAN_Init(&hfdcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN FDCAN1_Init 2 */

  /* USER CODE END FDCAN1_Init 2 */

}

/**
  * @brief LPUART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_LPUART1_UART_Init(void)
{

  /* USER CODE BEGIN LPUART1_Init 0 */

  /* USER CODE END LPUART1_Init 0 */

  /* USER CODE BEGIN LPUART1_Init 1 */

  /* USER CODE END LPUART1_Init 1 */
  hlpuart1.Instance = LPUART1;
  hlpuart1.Init.BaudRate = 209700;
  hlpuart1.Init.WordLength = UART_WORDLENGTH_8B;
  hlpuart1.Init.StopBits = UART_STOPBITS_1;
  hlpuart1.Init.Parity = UART_PARITY_NONE;
  hlpuart1.Init.Mode = UART_MODE_TX_RX;
  hlpuart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  hlpuart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  hlpuart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  hlpuart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  hlpuart1.FifoMode = UART_FIFOMODE_DISABLE;
  if (HAL_UART_Init(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&hlpuart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&hlpuart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LPUART1_Init 2 */

  /* USER CODE END LPUART1_Init 2 */

}

/**
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 500000;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  huart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart4.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart4, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart4, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */

  /* USER CODE END UART4_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_16BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 0x0;
  hspi2.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  hspi2.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi2.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi2.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi2.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi2.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi2.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi2.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi2.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi2.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief SPI3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI3_Init(void)
{

  /* USER CODE BEGIN SPI3_Init 0 */

  /* USER CODE END SPI3_Init 0 */

  /* USER CODE BEGIN SPI3_Init 1 */

  /* USER CODE END SPI3_Init 1 */
  /* SPI3 parameter configuration*/
  hspi3.Instance = SPI3;
  hspi3.Init.Mode = SPI_MODE_MASTER;
  hspi3.Init.Direction = SPI_DIRECTION_2LINES;
  hspi3.Init.DataSize = SPI_DATASIZE_16BIT;
  hspi3.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi3.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi3.Init.NSS = SPI_NSS_SOFT;
  hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi3.Init.CRCPolynomial = 0x0;
  hspi3.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  hspi3.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi3.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi3.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi3.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi3.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi3.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi3.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi3.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi3.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  if (HAL_SPI_Init(&hspi3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI3_Init 2 */

  /* USER CODE END SPI3_Init 2 */

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
  TIMEx_BreakInputConfigTypeDef sBreakInputConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_CENTERALIGNED1;
  htim1.Init.Period = 3999;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakInputConfig.Source = TIM_BREAKINPUTSOURCE_BKIN;
  sBreakInputConfig.Enable = TIM_BREAKINPUTSOURCE_ENABLE;
  sBreakInputConfig.Polarity = TIM_BREAKINPUTSOURCE_POLARITY_LOW;
  if (HAL_TIMEx_ConfigBreakInput(&htim1, TIM_BREAKINPUT_BRK, &sBreakInputConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 2000;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.Pulse = 0;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 10;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_ENABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
  sBreakDeadTimeConfig.Break2Filter = 0;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

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
  htim4.Init.Prescaler = 9999;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 9999;
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
  sConfigOC.Pulse = 5000;
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
  * @brief TIM8 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM8_Init(void)
{

  /* USER CODE BEGIN TIM8_Init 0 */

  /* USER CODE END TIM8_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM8_Init 1 */

  /* USER CODE END TIM8_Init 1 */
  htim8.Instance = TIM8;
  htim8.Init.Prescaler = 0;
  htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim8.Init.Period = 65535;
  htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim8.Init.RepetitionCounter = 0;
  htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim8) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim8, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM8_Init 2 */

  /* USER CODE END TIM8_Init 2 */

}

/**
  * @brief USB_OTG_HS Initialization Function
  * @param None
  * @retval None
  */
static void MX_USB_OTG_HS_PCD_Init(void)
{

  /* USER CODE BEGIN USB_OTG_HS_Init 0 */

  /* USER CODE END USB_OTG_HS_Init 0 */

  /* USER CODE BEGIN USB_OTG_HS_Init 1 */

  /* USER CODE END USB_OTG_HS_Init 1 */
  hpcd_USB_OTG_HS.Instance = USB_OTG_HS;
  hpcd_USB_OTG_HS.Init.dev_endpoints = 9;
  hpcd_USB_OTG_HS.Init.speed = PCD_SPEED_FULL;
  hpcd_USB_OTG_HS.Init.dma_enable = DISABLE;
  hpcd_USB_OTG_HS.Init.phy_itface = USB_OTG_EMBEDDED_PHY;
  hpcd_USB_OTG_HS.Init.Sof_enable = DISABLE;
  hpcd_USB_OTG_HS.Init.low_power_enable = DISABLE;
  hpcd_USB_OTG_HS.Init.lpm_enable = DISABLE;
  hpcd_USB_OTG_HS.Init.vbus_sensing_enable = ENABLE;
  hpcd_USB_OTG_HS.Init.use_dedicated_ep1 = DISABLE;
  hpcd_USB_OTG_HS.Init.use_external_vbus = DISABLE;
  if (HAL_PCD_Init(&hpcd_USB_OTG_HS) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_OTG_HS_Init 2 */

  /* USER CODE END USB_OTG_HS_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
  /* DMA2_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);

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
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(EXT_SEN_CS_GPIO_Port, EXT_SEN_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SPI2_SEN_CS_GPIO_Port, SPI2_SEN_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(CAN_STB_GPIO_Port, CAN_STB_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(Driver_Enable_GPIO_Port, Driver_Enable_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(Driver_CS_GPIO_Port, Driver_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : EXT_SEN_CS_Pin CAN_STB_Pin */
  GPIO_InitStruct.Pin = EXT_SEN_CS_Pin|CAN_STB_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : SPI2_SEN_CS_Pin */
  GPIO_InitStruct.Pin = SPI2_SEN_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SPI2_SEN_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : Driver_Enable_Pin */
  GPIO_InitStruct.Pin = Driver_Enable_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(Driver_Enable_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : Driver_CS_Pin */
  GPIO_InitStruct.Pin = Driver_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(Driver_CS_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	if(&hadc1 == hadc)
	{
		HAL_GPIO_WritePin( EXT_SEN_CS_GPIO_Port, EXT_SEN_CS_Pin, GPIO_PIN_RESET );
		HAL_SPI_Receive_IT(&hspi3, &SENSOR_HALL_DATA, 1);
	}
}


HAL_StatusTypeDef debug_uart4_write_text(uint8_t *pData)
{
	//Sends a text message over UART
	if (HAL_UART_Transmit(&huart4, pData, strlen(pData), 10000) != HAL_OK)
	{
	  return HAL_ERROR;
	}
	return HAL_OK;
}

HAL_StatusTypeDef debug_uart4_write_float(float number)
{
	//Sends a number over UART
	char temp[15];
	memset(temp, 0, 15);
	sprintf(temp, "%f", number );
	if (HAL_UART_Transmit(&huart4, temp, 10, 10000) != HAL_OK)
	  {
	    return HAL_ERROR;
	  }
	return HAL_OK;
}

HAL_StatusTypeDef Send_Command_Driver(uint16_t TX, uint16_t *RX, uint16_t length)
{
	uint32_t Timeout = 1;
	//Sends and Receives two bytes (one massage)
	HAL_StatusTypeDef return_Value = HAL_OK;
	HAL_GPIO_WritePin( Driver_CS_GPIO_Port, Driver_CS_Pin, GPIO_PIN_RESET );
	if(HAL_SPI_TransmitReceive(&hspi2, &TX, RX, length, Timeout) != HAL_OK)
	{
		return_Value = HAL_ERROR;
	}
	HAL_GPIO_WritePin( Driver_CS_GPIO_Port, Driver_CS_Pin, GPIO_PIN_SET );

	return return_Value;
}

HAL_StatusTypeDef Driver_set_up(void)
{
	//Function is setting up the MOSFet driver
	HAL_StatusTypeDef return_Value = HAL_OK;
	HAL_GPIO_WritePin(Driver_Enable_GPIO_Port, Driver_Enable_Pin, GPIO_PIN_SET);//Enable the Driver
	HAL_Delay(1);

	const uint16_t WRITE_BIT = 0;
	const uint16_t READ_BIT = (1<<15);
	const uint16_t FAULT_STATUS1_REG = 0;
	const uint16_t FAULT_STATUS2_REG = 0x0800;
	const uint16_t DRIVER_CONTROL_REG = 0x1000;
	const uint16_t GATE_DRIVE_HS_REG = 0x1800;
	const uint16_t GATE_DRIVE_LS_REG = 0x2000;
	const uint16_t OCP_CONTROL_REG = 0x2800;
	const uint16_t CSA_CONTROL_REG = 0x3000;
	const uint16_t DRIVER_CONFIG_REG = 0x3800;
	const uint16_t Actual_data = 0x7FF;

	uint16_t RECEIVE_MSG = 0;

	//Check whether the drive is connected
	Send_Command_Driver((READ_BIT | CSA_CONTROL_REG), &RECEIVE_MSG, 1);
	if(RECEIVE_MSG != (0x283 & Actual_data))
	{
		return HAL_ERROR;
	}

	Send_Command_Driver((WRITE_BIT | DRIVER_CONTROL_REG | (1<<10)), &RECEIVE_MSG, 1);//In case of a FAULT all H-bridges are turned of

	Send_Command_Driver((READ_BIT | DRIVER_CONTROL_REG), &RECEIVE_MSG, 1);
	if(RECEIVE_MSG != ((1<<10) & Actual_data))
	{
		return HAL_ERROR;
	}

	//Gate HS Drive
	//Send_Command_Driver(&(WRITE_BIT | GATE_DRIVE_HS_REG | 0b01110001000), &RECEIVE_MSG, 1);//Halfs the drive strength

	//Gate LS Drive
	//Send_Command_Driver(&(WRITE_BIT | GATE_DRIVE_HS_REG | 0b11010001000), &RECEIVE_MSG, 1);//Halfs the drive strength and reduces the peak gate current drive time

	Send_Command_Driver((WRITE_BIT | OCP_CONTROL_REG | 0b01001100110), &RECEIVE_MSG, 1);//Sets the dead time to 200ns and the VDS_LVL to 0.3V
	Send_Command_Driver((READ_BIT | OCP_CONTROL_REG), &RECEIVE_MSG, 1);
	if(RECEIVE_MSG != (0b01001100110 & Actual_data))
	{
		return HAL_ERROR;
	}

	Send_Command_Driver((WRITE_BIT | CSA_CONTROL_REG | 0b01010000000), &RECEIVE_MSG, 1);//Sets SENSE OCP to 0,25V
	Send_Command_Driver((READ_BIT | CSA_CONTROL_REG), &RECEIVE_MSG, 1);
	if(RECEIVE_MSG != (0b01010000000 & Actual_data))
	{
		return HAL_ERROR;
	}

	Send_Command_Driver((WRITE_BIT | DRIVER_CONFIG_REG | 0b1), &RECEIVE_MSG, 1);//activates the auto calibration routine
	Send_Command_Driver((READ_BIT | DRIVER_CONFIG_REG), &RECEIVE_MSG, 1);
	if(RECEIVE_MSG != (0b1 & Actual_data))
	{
		return HAL_ERROR;
	}

	return return_Value;
}

float ShuntVolts_to_Amps(float VMeas)
{
	//Converts the shunt voltage to amps
	VMeas = VMeas - (3.3f/2.f);//Bias subtraction
	VMeas = VMeas * (1.f/20.f);//Divide by the amplcation
	VMeas = VMeas * (1.f/0.005f);//Divide by shunt resistance (U/R) = A
	return VMeas;
}

float Volts_to_HalfbridgeVolts(float VMeas)
{
	//Converts the Measured voltage to the actual voltage on Half bridges (also usable for the supply voltage)
	VMeas = VMeas * (85.f/5.f);//Divide by a Factor
	return VMeas;
}

HAL_StatusTypeDef Send_Command_Sensor(uint16_t TX, uint16_t *RX, uint16_t length)
{
	uint32_t Timeout = 1;
	//Sends and Receives two bytes (one massage)
	HAL_StatusTypeDef return_Value = HAL_OK;
	HAL_GPIO_WritePin( EXT_SEN_CS_GPIO_Port, EXT_SEN_CS_Pin, GPIO_PIN_RESET );
	if(HAL_SPI_TransmitReceive(&hspi3, &TX, RX, length, Timeout) != HAL_OK)
	{
		return_Value = HAL_ERROR;
	}
	HAL_GPIO_WritePin( EXT_SEN_CS_GPIO_Port, EXT_SEN_CS_Pin, GPIO_PIN_SET );

	return return_Value;
}

float Convert_HALL_Sensor_Data(uint16_t Data)
{
	return (((float)Data*360.0f)/65536.0f);
}

float Convert_HALL_Sensor_Data_to_Electrical_angle(uint16_t Data, Motor_Parameter *MParam)
{
	float Not_pole_count_adjusted = fmodf((((float)Data*360.0f)/65536.0f) + angle_sens_offset, 360.f/MParam->Pole_pair_count);
	return Not_pole_count_adjusted * MParam->Pole_pair_count;
}

void Clark_Transform(float I_U, float I_V, float I_W, float* I_Alpha, float* I_Beta)
{
	//Calculates I_Alpha and I_Beta based on the other inputs
	const float inv_sqrt_3 = 0.57735026918962576450914878050196f;
	*I_Alpha = I_U;
	*I_Beta = (I_U + 2 * I_V) * inv_sqrt_3;
}

void Park_Transform(float I_Alpha, float I_Beta, float Phi, float* I_Q, float* I_D)
{
	//Calculates I_D and I_Q based on the park transform. Phi should be in degree
	float Sinus_val = 0; float Cosinus_val = 0;
	fast_Sinus_Cosinus(Phi, &Sinus_val, &Cosinus_val);
	*I_D = I_Alpha * Cosinus_val + I_Beta * Sinus_val;
	*I_Q = I_Beta * Cosinus_val - I_Alpha * Sinus_val;
}

void Inv_Park_Transform(float V_D, float V_Q, float Phi, float* V_Alpha, float* V_Beta)
{
	float Sinus_val = 0; float Cosinus_val = 0;
	fast_Sinus_Cosinus(Phi, &Sinus_val, &Cosinus_val);
	*V_Alpha = V_D * Cosinus_val - V_Q * Sinus_val;
	*V_Beta = V_D * Sinus_val - V_Q * Cosinus_val;
}

void Inv_Clark_Transform(float V_Alpha, float V_Beta, float* V_R1, float* V_R2, float* V_R3)
{
	const float sqrt_3 = 1.7320508075688772935274463415059f;
	const float divide_by_2 = 0.5f;
	*V_R1 = V_Beta;
	*V_R2 = ((-V_Beta) + sqrt_3 * V_Alpha) * divide_by_2;
	*V_R3 = ((-V_Beta) - sqrt_3 * V_Alpha) * divide_by_2;
}

void Calc_SVPWM_amp_angle(float Angle, float Amplitude, float Supply_voltage, float* PWM_U, float* PWM_V, float* PWM_W)
{
	//has some error that is not solved
	//Calculates the (proportional) dutycycle values for each phase depending on angle and amplitude
	float U_MEM, V_MEM, W_MEM;//Temporary storage
	const float sqrt_3 = 1.7320508075688772935274463415059f;

	//Calculates the sector in which the desired angle is
	uint16_t Sektor = ((uint16_t)(fabs(Angle * (1.f/60.f))) % 6) +1;

	//Time (proportional) Calculation for the switching state 1
	float T1 = (Amplitude * sqrt_3 * fast_Sinus(((float)Sektor) * 60 - Angle)) /  Supply_voltage;
	//Time (proportional) Calculation for the switching state 2
	float T2 = (Amplitude * sqrt_3 * fast_Sinus(Angle - ((float)(Sektor -1)) * 60)) / Supply_voltage;
	//Time (proportional) Calculation for the switching state 3
	float T0 = 1 - T1 - T2; // the zero state

	switch(Sektor)
	{
	case 1:
		//Sector 1
		U_MEM = T1 + T2 + T0/2;
		V_MEM = T2 + T0/2;
		W_MEM = T0/2;
		break;
	case 2:
		//Sector 2
		U_MEM = T1 +  T0/2;
		V_MEM = T1 + T2 + T0/2;
		W_MEM = T0/2;
		break;
	case 3:
		//Sector 3
		U_MEM = T0/2;
		V_MEM = T1 + T2 + T0/2;
		W_MEM = T2 + T0/2;
		break;
	case 4:
		//Sector 4
		U_MEM = T0/2;
		V_MEM = T1+ T0/2;
		W_MEM = T1 + T2 + T0/2;
		break;
	case 5:
		//Sector 5
		U_MEM = T2 + T0/2;
		V_MEM = T0/2;
		W_MEM = T1 + T2 + T0/2;
		break;
	case 6:
		//Sector 6
		U_MEM = T1 + T2 + T0/2;
		V_MEM = T0/2;
		W_MEM = T1 + T0/2;
		break;
	default:
		//In case of an error set everything to zero
		U_MEM = 0;
		V_MEM = 0;
		W_MEM = 0;
	}

	//Returns the proportional on time for an period of all phases
	*PWM_U = U_MEM;
	*PWM_V = V_MEM;
	*PWM_W = W_MEM;
}

void Calc_SVPWM_Alpha_Beta(float V_Alpha, float V_Beta, float* PWM_U, float* PWM_V, float* PWM_W)
{
	//Calculates the PWM value (proportional on time) for the half bridges (Alpha and Beta max value should be one)
	const float inv_sqrt_3 = 0.57735026918962576450914878050196f;
	const float sqrt_3 = 1.7320508075688772935274463415059f;
	const float divide_by_2 = 0.5f;

	//Inv Clark Transform
	float U = V_Alpha;
	float V = ((-V_Alpha) + sqrt_3 * V_Beta) * divide_by_2;
	float W = ((-V_Alpha) - sqrt_3 * V_Beta) * divide_by_2;

	//calculating the min mean from the min and max values for U,V,W
	//finding the min value
	float min = 0; float max = 0;
	if(U < V && U < W)
		min = U;
	else if(V < U && V < W)
		min = V;
	else
		min = W;
	//finding the max value
	if(U > V && U > W)
		max = U;
	else if(V > U && V > W)
		max = V;
	else
		max = W;

	float mid = (max + min) * divide_by_2;

	*PWM_U = (U - mid) * inv_sqrt_3 + 0.5f;
	*PWM_V = (V - mid) * inv_sqrt_3 + 0.5f;
	*PWM_W = (W - mid) * inv_sqrt_3 + 0.5f;
}

float float_low_pass_filter(float Alpha, float Val, float Val_prev)
{
	//calculates a filtered Value
	return ((Alpha * Val_prev) + ((1.f - Alpha) * Val));

	//Calculating alpha:
	// alpha = Ts / (Tf + Ts);
	//Ts equals the sampling time in seconds
	//Tf is a time constant, the cutoff frequency is fc = 1 / (2pi * Tf)
}


HAL_StatusTypeDef Currentmode_FOC(FOC_Values *Values, PIDController *pid_I_D, PIDController *pid_I_Q )
{
	//Calculates a current mode Field Oriented Control algorithm with the desired PID values
	float I_Alpha = 0; float I_Beta = 0;
	Clark_Transform(Values->IP_U, Values->IP_V, Values->IP_W, &I_Alpha, &I_Beta);//Clark Transformation

	float I_Q = 0; float I_D = 0;
	Park_Transform(I_Alpha, I_Beta, Values->electrical_angle, &I_Q, &I_D);//Park Transformation

	//PID Control calculation
	PIDController_Update(pid_I_D, Values->Id_target, I_D);//I D control
	PIDController_Update(pid_I_Q, Values->Iq_target, I_Q);//I Q control

	float V_Alpha = 0; float V_Beta = 0;
	Inv_Park_Transform(pid_I_D->out, pid_I_Q->out, Values->electrical_angle, &V_Alpha, &V_Beta);

	Calc_SVPWM_Alpha_Beta(V_Alpha, V_Beta, &Values->PWM_U_Dutycycle, &Values->PWM_V_Dutycycle, &Values->PWM_W_Dutycycle);

	//No sanity check for now
	return HAL_OK;
}


void Current_Mode_PID_Controller_set_up(PIDController *pid_I_D, PIDController *pid_I_Q)
{
	//Initialises the PID controllers
	pid_I_D->Kp = PID_KP_I_D; pid_I_D->Ki = PID_KI_I_D; pid_I_D->Kd = PID_KD_I_D;
	pid_I_D->tau = PID_TAU_I_D;
	pid_I_D->limMin = PID_LIM_MIN_INT_I_D; pid_I_D->limMax = PID_LIM_MAX_INT_I_D;
	pid_I_D->T = SAMPLE_TIME_S;

	PIDController_Init(pid_I_D);//I_D Controller

	pid_I_D->Kp = PID_KP_I_Q; pid_I_D->Ki = PID_KI_I_Q; pid_I_D->Kd = PID_KD_I_Q;
	pid_I_D->tau = PID_TAU_I_Q;
	pid_I_D->limMin = PID_LIM_MIN_INT_I_Q; pid_I_D->limMax = PID_LIM_MAX_INT_I_Q;
	pid_I_D->T = SAMPLE_TIME_S;

	PIDController_Init(pid_I_Q);//I_Q Controller

}

void Hall_sensor_calibration_val_gen(Motor_Parameter *MParam)
{
	//a routine that lets rotor rotate by changing the electrical angle while reading out the hall sensor values
	debug_uart4_write_text("Reference Angle [degree],Measured Angle [degree] \n");
	for(int h = 0; h < 2; h++)
	{
		for(float phi = 0.2f; phi < 360; phi = phi + 3.f)
		{
			float calibration_power = 0.2f;
			float V_Alpha = 0; float V_Beta = 0;
			fast_Sinus_Cosinus(-phi * MParam->Pole_pair_count, &V_Beta, &V_Alpha);//Generate an vector with angle phi and length 1

			float PWM_U = 0; float PWM_V = 0; float PWM_W = 0;
			Calc_SVPWM_Alpha_Beta(calibration_power * V_Alpha, calibration_power *V_Beta, &PWM_U, &PWM_V, &PWM_W);

			TIM1->CCR1 = proportinal_PWM_to_act_val(PWM_U);
			TIM1->CCR2 = proportinal_PWM_to_act_val(PWM_V);
			TIM1->CCR3 = proportinal_PWM_to_act_val(PWM_W);

			HAL_Delay(1000);//Let the readings stabilize

			int n = 100; int k = 0; //how many readings should be combined to one
			float Measured_angle;
			while(k < n)
			{
				Measured_angle = Measured_angle + Convert_HALL_Sensor_Data(SENSOR_HALL_DATA);

				NEW_SENSOR_DATA = 0;
				while(NEW_SENSOR_DATA == 0)
				{

				}
				k++;
			}
			Measured_angle = Measured_angle / (float)n;

			debug_uart4_write_float(phi + (float)h * 360.f);
			debug_uart4_write_text(",");
			debug_uart4_write_float(fmodf(Measured_angle + angle_sens_offset,360));
			debug_uart4_write_text("\r\n");

			//debug_uart4_write_text(" Elektrischer Winkel: ");
			//debug_uart4_write_float(Convert_HALL_Sensor_Data_to_Electrical_angle(SENSOR_HALL_DATA, MParam));
			//debug_uart4_write_text("\r\n");
		}
	}
	TIM1->CCR1 = 0;
	TIM1->CCR2 = 0;
	TIM1->CCR3 = 0;
}

void Hall_sensor_calibration_look_up_table_gen(Motor_Parameter *MParam, float* Look_up_table)
{
	debug_uart4_write_text("Look up table Generation: \r\n");
	//not yet ready
	//a routine that lets rotor rotate by changing the electrical angle while reading out the hall sensor values and generating an look up table
	for(float phi = 0.f; phi < 360 * MParam->Pole_pair_count; phi = phi + 1.f)
	{
		float calibration_power = 0.15f;
		float V_Alpha = 0; float V_Beta = 0;
		fast_Sinus_Cosinus(-phi, &V_Beta, &V_Alpha);//Generate an vector with angle phi and length 1

		float PWM_U = 0; float PWM_V = 0; float PWM_W = 0;
		Calc_SVPWM_Alpha_Beta(calibration_power * V_Alpha, calibration_power *V_Beta, &PWM_U, &PWM_V, &PWM_W);

		TIM1->CCR1 = proportinal_PWM_to_act_val(PWM_U);
		TIM1->CCR2 = proportinal_PWM_to_act_val(PWM_V);
		TIM1->CCR3 = proportinal_PWM_to_act_val(PWM_W);

		HAL_Delay(1000);//Let the readings stabilize

		int n = 100; //how many readings should be combined to one
		float Measured_angle;
		while(n > 0)
		{
			Measured_angle = Measured_angle + (float)SENSOR_HALL_DATA;
			NEW_SENSOR_DATA = 0;

			while(NEW_SENSOR_DATA == 0)
			{

			}
			n--;
		}
		Measured_angle = Measured_angle / (float)100;

		Look_up_table[(int)Convert_HALL_Sensor_Data(Measured_angle)] = phi;

		debug_uart4_write_float(phi);
		debug_uart4_write_text(",");
		debug_uart4_write_float((float)((int)Measured_angle));
		debug_uart4_write_text("\r\n");
	}
}

uint16_t proportinal_PWM_to_act_val(float PWM_val)
{
	//transforms a proportianal PWM (1-0) value to the actual register value required for the timer
	const uint16_t max_value = 3999; const uint16_t min_value = 0;
	uint16_t return_Value = (uint16_t)(((float)max_value) * PWM_val);
	if(return_Value > max_value)
		return  max_value;
	else
		return return_Value;
}

void Controll_Data_preparation(FOC_Values *Controll_Values,Motor_Parameter *MParam, Controller_Hardware_Parameter *CAL_Values)
{
	//Prepares the sensor data for the foc routine
	Controll_Values->IP_U = ShuntVolts_to_Amps((float)ADC1_result[1] * Bit16_to_Volt) - CAL_Values->I_U_offset;
	Controll_Values->IP_V = ShuntVolts_to_Amps((float)ADC1_result[0] * Bit16_to_Volt) - CAL_Values->I_V_offset;
	Controll_Values->IP_W = ShuntVolts_to_Amps((float)ADC2_result[0] * Bit16_to_Volt) - CAL_Values->I_W_offset;

	Controll_Values->electrical_angle = Convert_HALL_Sensor_Data_to_Electrical_angle(SENSOR_HALL_DATA,MParam);

	Controll_Values->Id_target = 2.f;
	Controll_Values->Iq_target = 1.f;
}

void set_PWM_Half_bridges(FOC_Values *Controll_Values)
{
	TIM1->CCR1 = proportinal_PWM_to_act_val(Controll_Values->PWM_U_Dutycycle);
	TIM1->CCR2 = proportinal_PWM_to_act_val(Controll_Values->PWM_V_Dutycycle);
	TIM1->CCR3 = proportinal_PWM_to_act_val(Controll_Values->PWM_W_Dutycycle);
}

void Current_sensor_calibration(Controller_Hardware_Parameter *CAL_Values)
{
	TIM1->CCR1 = 0;
	TIM1->CCR2 = 0;
	TIM1->CCR3 = 0;
	const uint16_t Samples = 1000;

	debug_uart4_write_text("Starting Currentsensor Calibration \r\n");

	float Cal_U_val = 0, Cal_V_val = 0, Cal_W_val = 0;

	for(uint16_t n = 0; n < Samples; n++)
	{
		Cal_U_val = Cal_U_val + ShuntVolts_to_Amps((float)ADC1_result[1] * Bit16_to_Volt);
		Cal_V_val = Cal_V_val + ShuntVolts_to_Amps((float)ADC1_result[0] * Bit16_to_Volt);
		Cal_W_val = Cal_W_val + ShuntVolts_to_Amps((float)ADC2_result[0] * Bit16_to_Volt);

		NEW_SENSOR_DATA = 0;

		while(NEW_SENSOR_DATA == 0)
		{

		}
	}
	CAL_Values->I_U_offset = Cal_U_val / Samples;
	CAL_Values->I_V_offset = Cal_V_val / Samples;
	CAL_Values->I_W_offset = Cal_W_val / Samples;

	debug_uart4_write_text("Finished Currentsensor Calibration \r\n");
}
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
