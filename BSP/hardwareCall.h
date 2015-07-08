/*
 * hardwareCall.h
 *
 *  Created on: Jul 7, 2015
 *      Author: shapa
 */

#ifndef HARDWARECALL_H_
#define HARDWARECALL_H_

#include "stm32f429xx.h"
#include "stm32f4xx_hal_conf.h"

void initLedGpio();
void initDebugGpio(void);
void initWiFiGpio(void);

void setGreenLedState(bool);
void setRedLedState(bool);

HAL_StatusTypeDef initDebugIface(UART_HandleTypeDef &huart);
HAL_StatusTypeDef initWiFiIface(UART_HandleTypeDef &huart);

void allowInterrupts(void);
void uartSendLocked(UART_HandleTypeDef *huart, const char *string, uint32_t size);
bool uartSendDMA(UART_HandleTypeDef *huart, const char *string, uint32_t size);
void uartSendDMAComplete(__DMA_HandleTypeDef *hdma);

#endif /* HARDWARECALL_H_ */
