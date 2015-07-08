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

	void setLedState(ledType led, bool state);

public:
	static BoardSupportPackage& getIstance();
	virtual ~BoardSupportPackage() {};
private:
	BoardSupportPackage();

private:
	bool debugInit();
	bool wifiUartInt();
	void printStartupMessage();
private:
	bool systemInitialized;
	bool debugInitialized;
	bool wifiUartInitialized;
	const uint32_t printfBufferSize;
	UART_HandleTypeDef debugUart;
	DMA_HandleTypeDef debugUartTXDMA;

friend void onSystemTick(BoardSupportPackage&);
friend void onDebugUartInterrupt(BoardSupportPackage&);
friend void onDebugTransmitComplete(BoardSupportPackage&);
};

extern BoardSupportPackage& system;

}


#endif /* BOARDSUPPORTPACKAGE_H_ */
