/*
 * BoardSupportPackage.cpp
 *
 *  Created on: Jul 7, 2015
 *      Author: shapa
 */

#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "BoardSupportPackage.h"
#include "hardwareCall.h"
#include "DebugTracer.h"
#include "myEvents.h"

static const uint32_t uartSpeedRate[] = {
    [UC::SPEED_9600] = 9600,
    [UC::SPEED_115200] = 115200
};

namespace BSP {

void BoardSupportPackage::systemInit() {
	initLedGpio();
	debugInit();
	printStartupMessage();
}

void BoardSupportPackage::onStartup() {
	allowInterrupts();
}

int BoardSupportPackage::debugTraceSync(const char *__restrict fmt, ...) {
	if (!debugInitialized) {
		return 0;
	}
	int writeSize = 0;
	char string[printfBufferSize];
	va_list args;
	va_start (args, fmt);
	writeSize = vsnprintf (string, printfBufferSize-3, fmt, args);
	string[writeSize++] = '\n';
	string[writeSize++] = '\r';
	string[writeSize++] = '\0';
	uartSendLocked(&debugUart, string, writeSize);
	va_end (args);
	return writeSize;
}

void BoardSupportPackage::setLedState(ledType led, bool state) {
	switch (led) {
	case GREEN_LED:
		setGreenLedState(state);
		break;
	case RED_LED:
		setRedLedState(state);
		break;
	}
}


bool BoardSupportPackage::debugTraceAsync(const char *__restrict string) {
	return debugInitialized ? uartSendDMA(&debugUart, string, strlen(string)) : false;
}

BoardSupportPackage& BoardSupportPackage::getIstance() {
	static BoardSupportPackage instance;
	return instance;
}

BoardSupportPackage::BoardSupportPackage() :
		systemInitialized(false),
		debugInitialized(false),
		wifiUartInitialized(false),
		printfBufferSize(512),
		debugUartSpeed(uartSpeedRate[UC::SPEED_115200]),
		wifiUartSpeed(uartSpeedRate[UC::SPEED_9600]){
	UART_InitTypeDef uartParams = {
			debugUartSpeed,
			UART_WORDLENGTH_8B,
			UART_STOPBITS_1,
			UART_PARITY_NONE,
			UART_MODE_TX_RX,
			UART_HWCONTROL_NONE,
			UART_OVERSAMPLING_8
	};
	DMA_InitTypeDef txDmaParams = {
			DMA_CHANNEL_5, //wtf?
			DMA_MEMORY_TO_PERIPH,
			DMA_PINC_DISABLE,
			DMA_MINC_ENABLE,
			DMA_PDATAALIGN_BYTE,//DMA_PDATAALIGN_WORD, //uart->dr == 4 bytes
			DMA_MDATAALIGN_BYTE,
			DMA_PFCTRL,
			DMA_PRIORITY_LOW,
			DMA_FIFOMODE_DISABLE,
			DMA_FIFO_THRESHOLD_1QUARTERFULL, // ignored
			DMA_MBURST_SINGLE,
			DMA_PBURST_SINGLE
	};
	TIM_Base_InitTypeDef steeringPWMTimInit= {
			13107,
			TIM_COUNTERMODE_UP,
			0xFFFF,
			TIM_CLOCKDIVISION_DIV1,
			0
	};
	/* configure debug UART */
	debugUartTXDMA.Instance = DMA1_Stream0;
	debugUartTXDMA.Init = txDmaParams;
	debugUartTXDMA.Parent = &debugUart;
	debugUart.Instance = UART8;
	debugUart.Init = uartParams;
	debugUart.hdmatx = &debugUartTXDMA;

	/* configure ESP8266 UART */
	wifiUartTXDMA.Instance = DMA1_Stream1; /* TODO: change this */
	wifiUartTXDMA.Init = txDmaParams;
	wifiUartTXDMA.Parent = &wifiUart;
	uartParams.BaudRate = wifiUartSpeed;
	wifiUart.Instance = UART7;
	wifiUart.Init = uartParams;
	wifiUart.hdmatx = &wifiUartTXDMA;

	steeringPWM.Channel = HAL_TIM_ACTIVE_CHANNEL_1;
	steeringPWM.Init = steeringPWMTimInit;
	steeringPWM.Instance = TIM2;
}

BoardSupportPackage& system = BoardSupportPackage::getIstance();


bool BoardSupportPackage::debugInit() {
	initDebugGpio();
	debugInitialized = initUartIface(debugUart) == HAL_OK;
	return debugInitialized;
}

void BoardSupportPackage::setDebugIFaceSpeed(UC::UartSpeed speed) {
	debugUart.Init.BaudRate = uartSpeedRate[speed];
	debugInit();
}


bool BoardSupportPackage::wifiUartInt() {
	initWiFiGpio();
	wifiUartInitialized = initUartIface(wifiUart) == HAL_OK;
	return wifiUartInitialized;
}

bool BoardSupportPackage::wifiSendAsync(const char *__restrict string) {
	return wifiUartInitialized ? uartSendDMA(&wifiUart, string, strlen(string)) : false;
}

void BoardSupportPackage::setWifiIFaceSpeed(UC::UartSpeed speed) {
	wifiUart.Init.BaudRate = uartSpeedRate[speed];
	wifiUartInt();
}

bool BoardSupportPackage::steeringPWMInt() {
	initSteeringPWMGpio();

}

void BoardSupportPackage::printStartupMessage() {
	uint32_t sysClock = HAL_RCC_GetSysClockFreq();
	debugTraceSync("\r\nSystem is being started.");
	debugTraceSync("System clocks");
	debugTraceSync("\t\tmain system clock\t %d Mhz", (sysClock/1000)/1000);
}

void onSystemTick(BoardSupportPackage&) {
//	QK_ISR_ENTRY();
	QP::QF::tick();
//	QK_ISR_EXIT();
}

void onDebugUartInterrupt(BoardSupportPackage& system) {
	QK_ISR_ENTRY();
	if (__HAL_UART_GET_FLAG(&system.debugUart, UART_FLAG_RXNE)) {
		QP::DataEvt *evt = Q_NEW(QP::DataEvt, DT::CHAR_SIG);
		evt->charachter = (char)UART8->DR;
		DT::item.post_( evt, 0);
	}
	HAL_UART_IRQHandler(&system.debugUart);
	QK_ISR_EXIT();
}

void onDebugTransmitComplete(BoardSupportPackage& system) {
	QK_ISR_ENTRY();
	if (__HAL_DMA_GET_FLAG(system.debugUart.hdmatx, DMA_FLAG_TEIF0_4)) {
		__HAL_DMA_CLEAR_FLAG(system.debugUart.hdmatx, DMA_FLAG_TEIF0_4);
//		HAL_DMA_IRQHandler(system.debugUart.hdmatx);
		DT::item.post_( Q_NEW(QP::QEvt, DT::TRANSMITION_COMPLETE_SIG), 0);
	}
	QK_ISR_EXIT();
}

}
