/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

// extern UART_HandleTypeDef huart3;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

// void MX_USART3_UART_Init(void);
#include "main.h"

void Usart_Init();
void addElement(uint8_t *data, uint16_t buffSize);
extern volatile uint8_t USART_req;
extern volatile uint16_t USART_req_size;
extern volatile uint16_t USART_req_start_index;

#define USART_DmaBuffSize (uint16_t)255


extern __attribute__((aligned(4))) uint8_t usart_rx_buffer[USART_DmaBuffSize];
/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */
typedef enum{
  buff1, 
  buff2,
  ide,
}USART3_State_t;
extern USART3_State_t usart3_state;
#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

