

#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"
#include "qp_port.h"
#include "BoardSupportPackage.h"
#include "BatteryController.h"
#include "DebugTracer.h"
#include "initQP.h"
#include "myEvents.h"


/* the start point */
int main (int argc, char* argv[]);


int main (int argc, char* argv[]) {
	(void)argc;
	(void)argv;
	uint_fast8_t prio = 1;

	BSP::system.systemInit();
	BSP::system.setLedState(BSP::RED_LED, true);
	QP::initializeFramework();

	/* the whole queue for events */
    static QP::QEvt const *queueSto[200];

	DT::item.start(prio++, queueSto, Q_DIM(queueSto), (void *)0, 0);
	BC::item.start(prio++, queueSto, Q_DIM(queueSto), (void *)0, 0);

	return QP::QF::run();
}
