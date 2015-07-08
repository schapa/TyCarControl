/*
 * initQP.cpp
 *
 *  Created on: Jul 8, 2015
 *      Author: shapa
 */

#include "initQP.h"
#include "myEvents.h"
#include "BoardSupportPackage.h"
#include "DebugTracer.h"

namespace QP {

extern "C" void Q_onAssert(char_t const Q_ROM * const file, int_t line);

static void init_QP_Pools();

void initializeFramework(void) {
	QF::init();

	init_QP_Pools();
}

static void init_QP_Pools() {
	static QF_MPOOL_EL(QEvt) qEvtPoolSto[200]; // storage for small event pool
	static QF_MPOOL_EL(DataEvt) traceEvtPoolSto[200]; //


	QF::poolInit(qEvtPoolSto, sizeof(qEvtPoolSto), sizeof(qEvtPoolSto[0]));
	QF::poolInit(traceEvtPoolSto, sizeof(traceEvtPoolSto), sizeof(traceEvtPoolSto[0]));

	BSP::system.debugTraceSync("Pool qEvtPoolSto - [%d] bytes", sizeof(qEvtPoolSto));
	BSP::system.debugTraceSync("Pool traceEvtPoolSto - [%d] bytes", sizeof(traceEvtPoolSto));
}

void QF::onStartup(void) {
	BSP::system.onStartup();
	BSP::system.debugTraceSync("Kernel started. QK version \"%s\"", QP::QF::getVersion());
}

void QK::onIdle(void) {
	__WFI();
}

extern "C" void Q_onAssert(char_t const Q_ROM * const file, int_t line) {

	BSP::system.debugTraceSync("QP::Q_onAssert(%s, %d)", file, line);
	__disable_irq();
	while (true) {
		;
	}
}

}
