/*
 * myEvents.h
 *
 *  Created on: Jul 8, 2015
 *      Author: shapa
 */

#ifndef MYEVENTS_H_
#define MYEVENTS_H_

#include "qp_port.h"
#include <stdint.h>

namespace QP {

	class DataEvt : public QEvt {
	public:
		union {
			void *ptr;
			char *string;
			uint32_t uint32;
			uint16_t uint16;
			uint8_t uint8;
			char charachter;
		};
	};

}
#endif /* MYEVENTS_H_ */
