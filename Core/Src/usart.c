/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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
/* Includes ------------------------------------------------------------------*/
#include "usart.h"
#include <stdint.h>



/*
 * USART_Driver.c
 *
 *  Created on: Apr 22, 2026
 *      Author: frees
 */


//#include "USART_Driver.h"
//uint8_t lockUsartDma;

__attribute__((aligned(4))) uint8_t usart_tx_buffer[USART_DmaBuffSize];
__attribute__((aligned(4))) uint8_t usart_rx_buffer[USART_DmaBuffSize];
volatile uint16_t usart_lenOfmess = 0;
volatile uint8_t start_message;
volatile uint8_t USART_req;
volatile uint16_t USART_req_size;
volatile uint16_t USART_req_start_index= 0;
void circ_memcpy(uint8_t* buff_start, uint8_t* arr_for_save, uint32_t data_start_index, uint32_t data_size, uint32_t buff_len){
	for (uint32_t i = 0; i < data_size; i++) {
    	arr_for_save[i] = buff_start[(data_start_index + i) % buff_len];
	}
}
void Usart_Init(){
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
	RCC->APB1ENR |= RCC_APB1ENR_USART3EN;

	GPIOD->MODER &= ~(GPIO_MODER_MODER8 | GPIO_MODER_MODER9);
	GPIOD->MODER |= (GPIO_MODER_MODER8_1) | (GPIO_MODER_MODER9_1);
	GPIOD->AFR[1] &= ~(0xFFUL << (4 * 0)); // PD8, PD9
	GPIOD->AFR[1] |= (7UL << GPIO_AFRH_AFRH1_Pos)
			| (7UL << GPIO_AFRH_AFRH0_Pos); // AF7
	// ------------------------------------------------------
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
	(void) (RCC->AHB1ENR); // Барьер
	USART3->CR1 = 0;
	// USART3->CR1 &= ~USART_CR1_UE;

	//USART3->BRR = 0x0341;
	USART3 ->BRR = 0x3B;
	USART3->CR1 |= USART_CR1_RE | USART_CR1_TE //| USART_CR1_OVER8
			| USART_CR1_RTOIE;  // Приём, передача, USART вкл.

	USART3->CR2 = 0;                                  // 1 стоп-бит
	USART3->RTOR |= 33;
	// USART3->CR2 |= USART_CR2_RTOEN;

	USART3->CR3 = USART_CR3_DMAT | USART_CR3_DMAR;                           // | USART_CR3_DMAT;

	// USART RX CONFIG -------------------------------------------------------------------------- //
	Dma_Disable(DMA1_Stream3);
	DMA1->LIFCR |= DMA_LIFCR_CTCIF3 | DMA_LIFCR_CHTIF3 | DMA_LIFCR_CTEIF3
				| DMA_LIFCR_CDMEIF3 | DMA_LIFCR_CFEIF3;
	DMA1_Stream3->CR = 0;
	DMA1_Stream3->CR |= (4 << DMA_SxCR_CHSEL_Pos) | DMA_SxCR_PL_0
			| DMA_SxCR_MINC |        // Memory increment
			DMA_SxCR_TCIE | DMA_SxCR_DIR_0;
	//			DMA_SxCR_CIRC;

	DMA1_Stream3->PAR = (uint32_t) &(USART3->TDR); // TX
	// USART RX CONFIG -------------------------------------------------------------------------- //
	Dma_Disable(DMA1_Stream1);
	DMA1->LIFCR |= DMA_LIFCR_CTCIF1 | DMA_LIFCR_CHTIF1 | DMA_LIFCR_CTEIF1 |
				   DMA_LIFCR_CDMEIF1 | DMA_LIFCR_CFEIF1;

	DMA1_Stream1->CR = 0;
	DMA1_Stream1->CR |= (4 << DMA_SxCR_CHSEL_Pos) |
						DMA_SxCR_PL_0 | // Medium (01)
						DMA_SxCR_MINC | // Memory increment
						DMA_SxCR_TCIE |
						DMA_SxCR_CIRC;        			// Transfer complete
						DMA1_Stream1->PAR = (uint32_t)&(USART3->RDR); // RX
	// USART3->CR2 |= USART_CR2_STOP_0;
	USART3->CR1 |= USART_CR1_UE;
	USART3->CR2 |= USART_CR2_RTOEN;//

	NVIC_EnableIRQ(DMA1_Stream3_IRQn);
	NVIC_EnableIRQ(DMA1_Stream1_IRQn);
	// NVIC_EnableIRQ(USART3_IRQn); // Включаем прерывание
	// NVIC_SetPriority(USART3_IRQn, 0); // Устанавливаем приоритетi
	Dma_Start(DMA1_Stream1, usart_rx_buffer, USART_DmaBuffSize);
}
typedef struct{
	uint8_t *data;
	uint16_t buffSize;
}fifoUsart_t;
//static uint8_t start = 0;
static uint16_t USART_req_it = 0;
static uint16_t USART_req_tail = 0;



void USART3_IRQHandler(void)
{
    if ((USART3->ISR & USART_ISR_RTOF) != 0) {
        USART3->ICR = USART_ICR_RTOCF;

        // Получаем количество принятых байт через DMA (если используется)
		USART_req_tail = USART_DmaBuffSize - DMA1_Stream1->NDTR;
		// addElement(&USART_req_tail, 1);
		USART_req_size = (USART_req_it <  USART_req_tail ? USART_req_tail - USART_req_it : USART_DmaBuffSize -  USART_req_it + USART_req_tail);
		USART_req_start_index = USART_req_it;
		USART_req_it = USART_req_tail;
        USART_req = 1;
        // ... ваша логика обработки принятых данных ...
        // HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
        HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
    }
}
#define FIFO_SIZE 24
static uint8_t it = 0;
static uint8_t tail = 0;
static fifoUsart_t fifoBuff[FIFO_SIZE];
//static uint8_t sizeFifo;
static uint8_t lockUsartDma = 0;
#define IT (it % FIFO_SIZE)
#define TAIL (tail % FIFO_SIZE)
void addElement(uint8_t *data, uint16_t buffSize){

	fifoBuff[TAIL].buffSize = buffSize;
	fifoBuff[TAIL].data = data;
	tail+=1;
	// if(tail < it) Led_On();
	if(lockUsartDma == 0) {
		lockUsartDma = 1;
		Dma_Start(DMA1_Stream3,fifoBuff[IT].data, fifoBuff[IT].buffSize);
	}
}
void call1(){
        it+=1;
        	if(IT != TAIL){
        		Dma_Start(DMA1_Stream3,fifoBuff[IT].data, fifoBuff[IT].buffSize);
        	}
        	else{
				    lockUsartDma = 0;
        	}
}







void call2(){

}
void DMA1_Stream3_IRQHandler(void) {
    if (DMA1->LISR & DMA_LISR_TCIF3) {        // Transfer Complete
        DMA1->LIFCR |= DMA_LIFCR_CTCIF3;
        call1();
    }
}

void DMA1_Stream1_IRQHandler(void) {
	if (DMA1->LISR & DMA_LISR_TCIF1) {
		DMA1->LIFCR = DMA_LIFCR_CTCIF1; // Сброс флага TCIF1
		// flag1 = tx_end;
	}
	// if (DMA1->LISR & DMA_LISR_TEIF1) {
	// 	DMA1->LIFCR = DMA_LIFCR_CTEIF1; // Сброс флага TEIF1
	// 	DMA1_Stream1->CR &= ~DMA_SxCR_EN;
	// 	while(DMA1_Stream1->CR & DMA_SxCR_EN);
	// 	DMA1_Stream1->CR |= DMA_SxCR_EN;
	// }
}
