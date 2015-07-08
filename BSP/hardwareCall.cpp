/*
 * hardwareCall.cpp
 *
 *  Created on: Jul 7, 2015
 *      Author: shapa
 */



#include "hardwareCall.h"
#include "BoardSupportPackage.h"
#include "stm32f4xx_hal_conf.h"


#include <string.h>
#include <stdlib.h>

extern "C" {
	void SysTick_Handler(void);
	void UART8_IRQHandler(void);
	void DMA1_Stream1_IRQHandler(void);
}

void initLedGpio() {
	/* PG13 - RX, PG14 - TX */
	GPIO_InitTypeDef initializer;
	initializer.Alternate = 0;
	initializer.Mode = GPIO_MODE_OUTPUT_PP;
	initializer.Pin = GPIO_PIN_13 | GPIO_PIN_14;
	initializer.Pull = GPIO_NOPULL;
	initializer.Speed = GPIO_SPEED_LOW;
	__HAL_RCC_GPIOG_CLK_ENABLE();
	HAL_GPIO_Init(GPIOG, &initializer);
}

void setGreenLedState(bool state) {
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void setRedLedState(bool state) {
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void initDebugGpio(void) {
	/* PE0 - RX, PE1 - TX, UART8 */
	GPIO_InitTypeDef initializer;
	initializer.Alternate = GPIO_AF8_UART8;
	initializer.Mode = GPIO_MODE_AF_PP;
	initializer.Pin = GPIO_PIN_0 | GPIO_PIN_1;
	initializer.Pull = GPIO_PULLUP;
	initializer.Speed = GPIO_SPEED_LOW;
	__HAL_RCC_GPIOE_CLK_ENABLE();
	HAL_GPIO_Init(GPIOE, &initializer);
}

void initWiFiGpio(void) {
	/* PF6 - RX, PF7 - TX, UART7 */

}

HAL_StatusTypeDef initDebugIface(UART_HandleTypeDef &huart) {
	__HAL_RCC_UART8_CLK_ENABLE();
	HAL_UART_Init(&huart);
    __HAL_UART_ENABLE_IT(&huart, UART_IT_RXNE);

	__HAL_RCC_DMA1_CLK_ENABLE();
	HAL_DMA_Init(huart.hdmatx);
	return HAL_OK;
}

void allowInterrupts(void) {
	/* debug interrupts */
	HAL_NVIC_EnableIRQ(UART8_IRQn);
	HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
}

void uartSendLocked(UART_HandleTypeDef *huart, const char *string, uint32_t size) {
	if (!string) {
		return;
	}
	HAL_StatusTypeDef stat = HAL_UART_Transmit(huart, (uint8_t*)string, static_cast<uint16_t>(size), 0xFFFF);
	while (stat == HAL_BUSY || stat == HAL_TIMEOUT) {
		stat = HAL_UART_Transmit(huart, (uint8_t*)string, static_cast<uint16_t>(size), 0xFFFF);
	}
}

bool uartSendDMA(UART_HandleTypeDef *huart, const char *string, uint32_t size) {
	if (!string || !size) {
		return false;
	}

#if 0
	HAL_StatusTypeDef stat = HAL_UART_Transmit_DMA(huart, (uint8_t*)string, static_cast<uint16_t>(size));
#else
	HAL_StatusTypeDef stat = HAL_UART_Transmit_IT(huart, (uint8_t*)string, static_cast<uint16_t>(size));
#endif
	return stat == HAL_OK;
}

extern "C" void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
//	HAL sender is incrementing address. Compute the real ptr for free
	char *prt = (char *)(huart->pTxBuffPtr - huart->TxXferSize);
	delete prt;
	onDebugTransmitComplete(BSP::system);
}

extern "C" {
	void SysTick_Handler(void) {
		HAL_IncTick();
		onSystemTick(BSP::system);
	}

	void UART8_IRQHandler(void) {
		onDebugUartInterrupt(BSP::system);
	}

	void DMA1_Stream1_IRQHandler(void) {
		onDebugTransmitComplete(BSP::system);
	}
}
