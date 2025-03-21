//.$file${./ArenaControlle~::Watchdog.cpp} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//
// Model: ArenaController.qm
// File:  ${./ArenaControlle~::Watchdog.cpp}
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
//.$endhead${./ArenaControlle~::Watchdog.cpp} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#include "Watchdog.hpp"


using namespace QP;

//============================================================================
// generate definition of to opaque pointer to the AO
//.$skip${QP_VERSION} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//. Check for the minimum required QP version
#if (QP_VERSION < 690U) || (QP_VERSION != ((QP_RELEASE^4294967295U) % 0x3E8U))
#error qpcpp version 6.9.0 or higher required
#endif
//.$endskip${QP_VERSION} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//.$define${Shared::AO_Watchdog} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace AC {

//.${Shared::AO_Watchdog} ....................................................
QP::QActive * const AO_Watchdog = &Watchdog::instance;

} // namespace AC
//.$enddef${Shared::AO_Watchdog} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//============================================================================
// generate definition of the AO
//.$define${AOs::Watchdog} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace AC {

//.${AOs::Watchdog} ..........................................................
Watchdog Watchdog::instance;
//.${AOs::Watchdog::Watchdog} ................................................
Watchdog::Watchdog()
: QActive(Q_STATE_CAST(&Watchdog::initial)),
    watchdog_time_evt_(this, WATCHDOG_TIMEOUT_SIG, 0U)
{}

//.${AOs::Watchdog::SM} ......................................................
Q_STATE_DEF(Watchdog, initial) {
    //.${AOs::Watchdog::SM::initial}
    FSP::Watchdog_initializeAndSubscribe(this, e);

    QS_FUN_DICTIONARY(&Watchdog::Feeding);
    QS_FUN_DICTIONARY(&Watchdog::Initialized);
    QS_FUN_DICTIONARY(&Watchdog::Resetting);

    return tran(&Feeding);
}
//.${AOs::Watchdog::SM::Feeding} .............................................
Q_STATE_DEF(Watchdog, Feeding) {
    QP::QState status_;
    switch (e->sig) {
        //.${AOs::Watchdog::SM::Feeding}
        case Q_ENTRY_SIG: {
            FSP::Watchdog_armWatchdogTimer(this, e);
            status_ = Q_RET_HANDLED;
            break;
        }
        //.${AOs::Watchdog::SM::Feeding}
        case Q_EXIT_SIG: {
            FSP::Watchdog_disarmWatchdogTimer(this, e);
            status_ = Q_RET_HANDLED;
            break;
        }
        //.${AOs::Watchdog::SM::Feeding::initial}
        case Q_INIT_SIG: {
            status_ = tran(&Initialized);
            break;
        }
        //.${AOs::Watchdog::SM::Feeding::RESET}
        case RESET_SIG: {
            status_ = tran(&Resetting);
            break;
        }
        default: {
            status_ = super(&top);
            break;
        }
    }
    return status_;
}
//.${AOs::Watchdog::SM::Feeding::Initialized} ................................
Q_STATE_DEF(Watchdog, Initialized) {
    QP::QState status_;
    switch (e->sig) {
        //.${AOs::Watchdog::SM::Feeding::Initialized::WATCHDOG_TIMEOUT}
        case WATCHDOG_TIMEOUT_SIG: {
            FSP::Watchdog_feedWatchdog(this, e);
            status_ = Q_RET_HANDLED;
            break;
        }
        default: {
            status_ = super(&Feeding);
            break;
        }
    }
    return status_;
}
//.${AOs::Watchdog::SM::Resetting} ...........................................
Q_STATE_DEF(Watchdog, Resetting) {
    QP::QState status_;
    switch (e->sig) {
        default: {
            status_ = super(&top);
            break;
        }
    }
    return status_;
}

} // namespace AC
//.$enddef${AOs::Watchdog} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
