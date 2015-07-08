//****************************************************************************
// Model: uartController.qm
// File:  ./UartController.cpp
//
// This code has been generated by QM tool (see state-machine.com/qm).
// DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
//
// This program is open source software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published
// by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
// for more details.
//****************************************************************************
//${Controller::.::UartController.c~} ........................................
#include "qp_port.h"
#include "UartController.h"
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include "BoardSupportPackage.h"
#include "myEvents.h"
#include "DebugTracer.h"

namespace UC {

static const char newLine = '\n';
static const char carretReturn = '\r';

//${Controller::UartController} ..............................................
//${Controller::UartController::UartController} ..............................
UartController::UartController(QP::QMActive& own)
  : QMsm(Q_STATE_CAST(&UartController::initial)),
    owner(own),
    isInDebugMode(false){
    //
}

//${Controller::UartController::~UartController} .............................
UartController::~UartController() {
    //
}


//${Controller::UartController::debugTrace} ..................................
void UartController::debugTrace(const char *__restrict fmt, ... ) {
    if (isInDebugMode) {
        va_list args;
        va_start (args, fmt);
        BSP::system.debugTraceSync(fmt, args);
        va_end (args);
    }
}
//${Controller::UartController::setDebugMode} ................................
void UartController::setDebugMode(bool state) {
    isInDebugMode = state;
}
//${Controller::UartController::SM} ..........................................
QP::QState UartController::initial(UartController * const me, QP::QEvt const * const e) {
    static struct {
        QP::QMState const *target;
        QP::QActionHandler act[3];
    } const tatbl_ = { // transition-action table
        &idleState_s,
        {
            Q_ACTION_CAST(&WorkingState_e), // entry
            Q_ACTION_CAST(&idleState_e), // entry
            Q_ACTION_CAST(0)  // zero terminator
        }
    };
    // ${Controller::UartController::SM::initial}
    (void)e; // supress warning
    me->debugTrace("UC: initial Transition");
    return QM_TRAN_INIT(&tatbl_);
}
//${Controller::UartController::SM::WorkingState} ............................
QP::QMState const UartController::WorkingState_s = {
    static_cast<QP::QMState const *>(0), // superstate (top)
    Q_STATE_CAST(&WorkingState),
    Q_ACTION_CAST(&WorkingState_e),
    Q_ACTION_CAST(&WorkingState_x),
    Q_ACTION_CAST(0)  // no intitial tran.
};
// ${Controller::UartController::SM::WorkingState}
QP::QState UartController::WorkingState_e(UartController * const me) {
    me->debugTrace("UC: WorkingState entry");
    return QM_ENTRY(&WorkingState_s);
}
// ${Controller::UartController::SM::WorkingState}
QP::QState UartController::WorkingState_x(UartController * const me) {
    me->debugTrace("UC: WorkingState exit");
    return QM_EXIT(&WorkingState_s);
}
// ${Controller::UartController::SM::WorkingState}
QP::QState UartController::WorkingState(UartController * const me, QP::QEvt const * const e) {
    QP::QState status_;
    switch (e->sig) {
        // ${Controller::UartController::SM::WorkingState::RECV_NEW}
        case RECV_NEW_SIG: {
            const QP::DataEvt *const recieved = static_cast<const QP::DataEvt*const>(e);
            // ${Controller::UartController::SM::WorkingState::RECV_NEW::[terminator]}
            if ((recieved->charachter == newLine) || (recieved->charachter == carretReturn)) {
                QP::DataEvt *evt = Q_NEW(QP::DataEvt, DT::NEW_BUFFERED_STRING_SIG);
                char *buff = evt->string = (char *)malloc(me->input.length()+1);
                while (!me->input.isEmpty()) {
                    *buff++ = me->input.get();
                }
                *buff = '\0';
                me->owner.post_( evt, 0);
                status_ = QM_HANDLED();
            }
            // ${Controller::UartController::SM::WorkingState::RECV_NEW::[waitfornewdata]}
            else {
                const QP::DataEvt *const recieved = static_cast<const QP::DataEvt*const>(e);
                me->input.add(recieved->charachter);
                status_ = QM_HANDLED();
            }
            break;
        }
        // ${Controller::UartController::SM::WorkingState::IDLE_DETECTED}
        case IDLE_DETECTED_SIG: {
            status_ = QM_HANDLED();
            break;
        }
        // ${Controller::UartController::SM::WorkingState::SEND_NEW}
        case SEND_NEW_SIG: {
            static struct {
                QP::QMState const *target;
                QP::QActionHandler act[2];
            } const tatbl_ = { // transition-action table
                &sendingState_s,
                {
                    Q_ACTION_CAST(&sendingState_e), // entry
                    Q_ACTION_CAST(0)  // zero terminator
                }
            };
            me->debugTrace("UC: SEND_NEW_SIG");
            const QP::DataEvt *const evt = static_cast<const QP::DataEvt *const>(e);
            me->sendData(evt->string);
            status_ = QM_TRAN(&tatbl_);
            break;
        }
        default: {
            status_ = QM_SUPER();
            break;
        }
    }
    return status_;
}
//${Controller::UartController::SM::WorkingState::idleState} .................
QP::QMState const UartController::idleState_s = {
    &UartController::WorkingState_s, // superstate
    Q_STATE_CAST(&idleState),
    Q_ACTION_CAST(&idleState_e),
    Q_ACTION_CAST(&idleState_x),
    Q_ACTION_CAST(0)  // no intitial tran.
};
// ${Controller::UartController::SM::WorkingState::idleState}
QP::QState UartController::idleState_e(UartController * const me) {
    me->debugTrace("UC: idleState entry");
    return QM_ENTRY(&idleState_s);
}
// ${Controller::UartController::SM::WorkingState::idleState}
QP::QState UartController::idleState_x(UartController * const me) {
    me->debugTrace("UC: idleState exit");
    return QM_EXIT(&idleState_s);
}
// ${Controller::UartController::SM::WorkingState::idleState}
QP::QState UartController::idleState(UartController * const me, QP::QEvt const * const e) {
    QP::QState status_;
    switch (e->sig) {
        default: {
            status_ = QM_SUPER();
            break;
        }
    }
    (void)me; /* avoid compiler warning in case 'me' is not used */
    return status_;
}
//${Controller::UartController::SM::WorkingState::sendingState} ..............
QP::QMState const UartController::sendingState_s = {
    &UartController::WorkingState_s, // superstate
    Q_STATE_CAST(&sendingState),
    Q_ACTION_CAST(&sendingState_e),
    Q_ACTION_CAST(&sendingState_x),
    Q_ACTION_CAST(0)  // no intitial tran.
};
// ${Controller::UartController::SM::WorkingState::sendingState}
QP::QState UartController::sendingState_e(UartController * const me) {
    me->debugTrace("UC: sendingState entry");
    return QM_ENTRY(&sendingState_s);
}
// ${Controller::UartController::SM::WorkingState::sendingState}
QP::QState UartController::sendingState_x(UartController * const me) {
    me->debugTrace("UC: sendingState exit");
    return QM_EXIT(&sendingState_s);
}
// ${Controller::UartController::SM::WorkingState::sendingState}
QP::QState UartController::sendingState(UartController * const me, QP::QEvt const * const e) {
    QP::QState status_;
    switch (e->sig) {
        // ${Controller::UartController::SM::WorkingState::sendingState::SEND_SUCESSFULL}
        case SEND_SUCESSFULL_SIG: {
            me->debugTrace("UC: SEND_SUCESSFULL_SIG");
            // ${Controller::UartController::SM::WorkingState::sendingState::SEND_SUCESSFULL::[Empty]}
            if (me->output.isEmpty()) {
                static struct {
                    QP::QMState const *target;
                    QP::QActionHandler act[3];
                } const tatbl_ = { // transition-action table
                    &idleState_s,
                    {
                        Q_ACTION_CAST(&sendingState_x), // exit
                        Q_ACTION_CAST(&idleState_e), // entry
                        Q_ACTION_CAST(0)  // zero terminator
                    }
                };
                status_ = QM_TRAN(&tatbl_);
            }
            // ${Controller::UartController::SM::WorkingState::sendingState::SEND_SUCESSFULL::[Next]}
            else {
                static struct {
                    QP::QMState const *target;
                    QP::QActionHandler act[3];
                } const tatbl_ = { // transition-action table
                    &sendingState_s,
                    {
                        Q_ACTION_CAST(&sendingState_x), // exit
                        Q_ACTION_CAST(&sendingState_e), // entry
                        Q_ACTION_CAST(0)  // zero terminator
                    }
                };
                char *data = me->output.get();
                me->sendData(data);
                status_ = QM_TRAN(&tatbl_);
            }
            break;
        }
        // ${Controller::UartController::SM::WorkingState::sendingState::SEND_NEW}
        case SEND_NEW_SIG: {
            me->debugTrace("UC: SEND_NEW_SIG. Just Consume");
            QP::DataEvt *evt = (QP::DataEvt*)e;
            me->output.add(evt->string);
            status_ = QM_HANDLED();
            break;
        }
        default: {
            status_ = QM_SUPER();
            break;
        }
    }
    return status_;
}


}