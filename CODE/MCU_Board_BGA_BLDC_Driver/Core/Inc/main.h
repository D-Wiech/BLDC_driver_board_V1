/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"
#include "stm32h7xx_ll_cordic.h"
#include "stm32h7xx_ll_bus.h"
#include "stm32h7xx_ll_cortex.h"
#include "stm32h7xx_ll_rcc.h"
#include "stm32h7xx_ll_system.h"
#include "stm32h7xx_ll_utils.h"
#include "stm32h7xx_ll_pwr.h"
#include "stm32h7xx_ll_gpio.h"
#include "stm32h7xx_ll_dma.h"

#include "stm32h7xx_ll_exti.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define External_uart_RX_Pin GPIO_PIN_7
#define External_uart_RX_GPIO_Port GPIOB
#define EXT_SEN_CS_Pin GPIO_PIN_15
#define EXT_SEN_CS_GPIO_Port GPIOA
#define External_uart_TX_Pin GPIO_PIN_6
#define External_uart_TX_GPIO_Port GPIOB
#define EXT_SEN_MISO_Pin GPIO_PIN_11
#define EXT_SEN_MISO_GPIO_Port GPIOC
#define EXT_SEN_SCK_Pin GPIO_PIN_10
#define EXT_SEN_SCK_GPIO_Port GPIOC
#define SPI2_SEN_CS_Pin GPIO_PIN_1
#define SPI2_SEN_CS_GPIO_Port GPIOE
#define EXT_SEN_MOSI_Pin GPIO_PIN_12
#define EXT_SEN_MOSI_GPIO_Port GPIOC
#define DEBUG_RX_Pin GPIO_PIN_0
#define DEBUG_RX_GPIO_Port GPIOD
#define CAN_STB_Pin GPIO_PIN_8
#define CAN_STB_GPIO_Port GPIOA
#define DEBUG_TX_Pin GPIO_PIN_1
#define DEBUG_TX_GPIO_Port GPIOD
#define Driver_Enable_Pin GPIO_PIN_9
#define Driver_Enable_GPIO_Port GPIOC
#define U_Supply_Pin GPIO_PIN_0
#define U_Supply_GPIO_Port GPIOC
#define U_Phase_U_Pin GPIO_PIN_1
#define U_Phase_U_GPIO_Port GPIOC
#define NTC_Pin_Pin GPIO_PIN_3
#define NTC_Pin_GPIO_Port GPIOC
#define I_Phase_V_Pin GPIO_PIN_0
#define I_Phase_V_GPIO_Port GPIOA
#define PWM_V_L_Pin GPIO_PIN_10
#define PWM_V_L_GPIO_Port GPIOE
#define Driver_Sen_MOSI_Pin GPIO_PIN_15
#define Driver_Sen_MOSI_GPIO_Port GPIOB
#define I_Phase_U_Pin GPIO_PIN_1
#define I_Phase_U_GPIO_Port GPIOA
#define PWM_V_H_Pin GPIO_PIN_11
#define PWM_V_H_GPIO_Port GPIOE
#define Driver_fault_Pin GPIO_PIN_15
#define Driver_fault_GPIO_Port GPIOE
#define Driver_Sen_MISO_Pin GPIO_PIN_14
#define Driver_Sen_MISO_GPIO_Port GPIOB
#define U_Phase_W_Pin GPIO_PIN_2
#define U_Phase_W_GPIO_Port GPIOA
#define I_Phase_W_Pin GPIO_PIN_0
#define I_Phase_W_GPIO_Port GPIOB
#define PWM_U_L_Pin GPIO_PIN_8
#define PWM_U_L_GPIO_Port GPIOE
#define PWM_W_L_Pin GPIO_PIN_12
#define PWM_W_L_GPIO_Port GPIOE
#define Driver_Sen_SCK_Pin GPIO_PIN_13
#define Driver_Sen_SCK_GPIO_Port GPIOB
#define U_Phase_V_Pin GPIO_PIN_3
#define U_Phase_V_GPIO_Port GPIOA
#define PWM_U_H_Pin GPIO_PIN_9
#define PWM_U_H_GPIO_Port GPIOE
#define PWM_W_H_Pin GPIO_PIN_13
#define PWM_W_H_GPIO_Port GPIOE
#define Driver_CS_Pin GPIO_PIN_12
#define Driver_CS_GPIO_Port GPIOB
#define USER_LED_Pin GPIO_PIN_12
#define USER_LED_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
