//.$file${./ArenaControlle~::SerialCommandInterface.cpp} vvvvvvvvvvvvvvvvvvvvv
//
// Model: ArenaController.qm
// File:  ${./ArenaControlle~::SerialCommandInterface.cpp}
//
// This code has been generated by QM 5.1.3 <www.state-machine.com/qm/>.
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
//
//.$endhead${./ArenaControlle~::SerialCommandInterface.cpp} ^^^^^^^^^^^^^^^^^^
#include "ArenaController.hpp"  // ArenaController application interface


using namespace QP;

//============================================================================
// generate declaration of the active object
//.$declare${AOs::SerialCommandInterface} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace AC {

//.${AOs::SerialCommandInterface} ............................................
class SerialCommandInterface : public QP::QActive {
public:
    QP::QTimeEvt serial_time_evt_;
    static SerialCommandInterface instance;

public:
    SerialCommandInterface();

protected:
    Q_STATE_DECL(initial);
    Q_STATE_DECL(Active);
    Q_STATE_DECL(NotReady);
    Q_STATE_DECL(PollingForNewCommand);
    Q_STATE_DECL(Waiting);
    Q_STATE_DECL(ProcessingCommand);
    Q_STATE_DECL(Inactive);
};

} // namespace AC
//.$enddecl${AOs::SerialCommandInterface} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//============================================================================
// generate definition of to opaque pointer to the AO
//.$skip${QP_VERSION} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//. Check for the minimum required QP version
#if (QP_VERSION < 690U) || (QP_VERSION != ((QP_RELEASE^4294967295U) % 0x3E8U))
#error qpcpp version 6.9.0 or higher required
#endif
//.$endskip${QP_VERSION} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//.$define${Shared::AO_SerialCommandInterface} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace AC {

//.${Shared::AO_SerialCommandInterface} ......................................
QP::QActive * const AO_SerialCommandInterface = &SerialCommandInterface::instance;

} // namespace AC
//.$enddef${Shared::AO_SerialCommandInterface} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//============================================================================
// generate definition of the AO
//.$define${AOs::SerialCommandInterface} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace AC {

//.${AOs::SerialCommandInterface} ............................................
SerialCommandInterface SerialCommandInterface::instance;
//.${AOs::SerialCommandInt~::SerialCommandInterface} .........................
SerialCommandInterface::SerialCommandInterface()
: QActive(Q_STATE_CAST(&SerialCommandInterface::initial)),
    serial_time_evt_(this, SERIAL_TIMEOUT_SIG, 0U)
{}

//.${AOs::SerialCommandInt~::SM} .............................................
Q_STATE_DEF(SerialCommandInterface, initial) {
    //.${AOs::SerialCommandInt~::SM::initial}
    subscribe(SERIAL_COMMAND_AVAILABLE_SIG);
    subscribe(ETHERNET_COMMAND_AVAILABLE_SIG);
    subscribe(COMMAND_PROCESSED_SIG);
    return tran(&Inactive);
}
//.${AOs::SerialCommandInt~::SM::Active} .....................................
Q_STATE_DEF(SerialCommandInterface, Active) {
    QP::QState status_;
    switch (e->sig) {
        //.${AOs::SerialCommandInt~::SM::Active}
        case Q_ENTRY_SIG: {
            serial_time_evt_.armX(BSP::TICKS_PER_SEC/2, BSP::TICKS_PER_SEC/50);
            status_ = Q_RET_HANDLED;
            break;
        }
        //.${AOs::SerialCommandInt~::SM::Active}
        case Q_EXIT_SIG: {
            serial_time_evt_.disarm();
            status_ = Q_RET_HANDLED;
            break;
        }
        //.${AOs::SerialCommandInt~::SM::Active::initial}
        case Q_INIT_SIG: {
            status_ = tran(&NotReady);
            break;
        }
        //.${AOs::SerialCommandInt~::SM::Active::DEACTIVATE_SERIAL_COMMAND_INTERF~}
        case DEACTIVATE_SERIAL_COMMAND_INTERFACE_SIG: {
            status_ = tran(&Inactive);
            break;
        }
        //.${AOs::SerialCommandInt~::SM::Active::SERIAL_TIMEOUT}
        case SERIAL_TIMEOUT_SIG: {
            status_ = Q_RET_HANDLED;
            break;
        }
        default: {
            status_ = super(&top);
            break;
        }
    }
    return status_;
}
//.${AOs::SerialCommandInt~::SM::Active::NotReady} ...........................
Q_STATE_DEF(SerialCommandInterface, NotReady) {
    QP::QState status_;
    switch (e->sig) {
        //.${AOs::SerialCommandInt~::SM::Active::NotReady}
        case Q_ENTRY_SIG: {
            BSP::beginSerial();
            status_ = Q_RET_HANDLED;
            break;
        }
        //.${AOs::SerialCommandInt~::SM::Active::NotReady::SERIAL_READY}
        case SERIAL_READY_SIG: {
            status_ = tran(&PollingForNewCommand);
            break;
        }
        default: {
            status_ = super(&Active);
            break;
        }
    }
    return status_;
}
//.${AOs::SerialCommandInt~::SM::Active::PollingForNewCommand} ...............
Q_STATE_DEF(SerialCommandInterface, PollingForNewCommand) {
    QP::QState status_;
    switch (e->sig) {
        //.${AOs::SerialCommandInt~::SM::Active::PollingForNewCom~::SERIAL_TIMEOUT}
        case SERIAL_TIMEOUT_SIG: {
            BSP::pollSerialCommand();
            status_ = Q_RET_HANDLED;
            break;
        }
        //.${AOs::SerialCommandInt~::SM::Active::PollingForNewCom~::ETHERNET_COMMAND_AVAILABLE}
        case ETHERNET_COMMAND_AVAILABLE_SIG: {
            status_ = tran(&Waiting);
            break;
        }
        //.${AOs::SerialCommandInt~::SM::Active::PollingForNewCom~::SERIAL_COMMAND_AVAILABLE}
        case SERIAL_COMMAND_AVAILABLE_SIG: {
            BSP::readSerialCommand();
            status_ = tran(&ProcessingCommand);
            break;
        }
        default: {
            status_ = super(&Active);
            break;
        }
    }
    return status_;
}
//.${AOs::SerialCommandInt~::SM::Active::Waiting} ............................
Q_STATE_DEF(SerialCommandInterface, Waiting) {
    QP::QState status_;
    switch (e->sig) {
        //.${AOs::SerialCommandInt~::SM::Active::Waiting::COMMAND_PROCESSED}
        case COMMAND_PROCESSED_SIG: {
            status_ = tran(&PollingForNewCommand);
            break;
        }
        default: {
            status_ = super(&Active);
            break;
        }
    }
    return status_;
}
//.${AOs::SerialCommandInt~::SM::Active::ProcessingCommand} ..................
Q_STATE_DEF(SerialCommandInterface, ProcessingCommand) {
    QP::QState status_;
    switch (e->sig) {
        //.${AOs::SerialCommandInt~::SM::Active::ProcessingComman~::COMMAND_PROCESSED}
        case COMMAND_PROCESSED_SIG: {
            status_ = tran(&PollingForNewCommand);
            break;
        }
        default: {
            status_ = super(&Active);
            break;
        }
    }
    return status_;
}
//.${AOs::SerialCommandInt~::SM::Inactive} ...................................
Q_STATE_DEF(SerialCommandInterface, Inactive) {
    QP::QState status_;
    switch (e->sig) {
        //.${AOs::SerialCommandInt~::SM::Inactive::ACTIVATE_SERIAL_COMMAND_INTERFAC~}
        case ACTIVATE_SERIAL_COMMAND_INTERFACE_SIG: {
            status_ = tran(&Active);
            break;
        }
        default: {
            status_ = super(&top);
            break;
        }
    }
    return status_;
}

} // namespace AC
//.$enddef${AOs::SerialCommandInterface} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
