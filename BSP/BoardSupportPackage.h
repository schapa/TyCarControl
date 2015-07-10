/*
 * BoardSupportPackage.h
 *
 *  Created on: Jul 7, 2015
 *      Author: shapa
 */

#ifndef BOARDSUPPORTPACKAGE_H_
#define BOARDSUPPORTPACKAGE_H_

#include <stdint.h>
#include "stm32f4xx_hal_conf.h"
#include "UartController.h"

namespace BSP {

enum ledType{
	GREEN_LED,
	RED_LED
};

class BoardSupportPackage {
public:
	void systemInit();
	void onStartup();

	int	debugTraceSync(const char *__restrict fmt, ...);
	bool debugTraceAsync(const char *__restrict string);
	void setDebugIFaceSpeed(UC::UartSpeed);

	bool wifiSendAsync(const char *__restrict string);
	void setWifiIFaceSpeed(UC::UartSpeed);

	void setLedState(ledType led, bool state);

public:
	static BoardSupportPackage& getIstance();
	virtual ~BoardSupportPackage() {};
private:
	BoardSupportPackage();

private:
	bool debugInit();
	bool wifiUartInt();
	bool steeringPWMInt();
	void printStartupMessage();
private:
	bool systemInitialized;
	bool debugInitialized;
	bool wifiUartInitialized;
	const uint32_t printfBufferSize;
	UART_HandleTypeDef debugUart;
	DMA_HandleTypeDef debugUartTXDMA;
	uint32_t debugUartSpeed;

	UART_HandleTypeDef wifiUart;
	DMA_HandleTypeDef wifiUartTXDMA;
	uint32_t wifiUartSpeed;

	TIM_HandleTypeDef steeringPWM;

friend void onSystemTick(BoardSupportPackage&);
friend void onDebugUartInterrupt(BoardSupportPackage&);
friend void onDebugTransmitComplete(BoardSupportPackage&);
};

extern BoardSupportPackage& system;

}


#endif /* BOARDSUPPORTPACKAGE_H_ */
