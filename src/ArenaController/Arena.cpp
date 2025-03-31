//.$file${./ArenaControlle~::Arena.cpp} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//
// Model: ArenaController.qm
// File:  ${./ArenaControlle~::Arena.cpp}
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
//.$endhead${./ArenaControlle~::Arena.cpp} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#include "Arena.hpp"


using namespace QP;

//============================================================================
// generate definition of to opaque pointer to the AO
//.$skip${QP_VERSION} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//. Check for the minimum required QP version
#if (QP_VERSION < 690U) || (QP_VERSION != ((QP_RELEASE^4294967295U) % 0x3E8U))
#error qpcpp version 6.9.0 or higher required
#endif
//.$endskip${QP_VERSION} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//.$define${Shared::AO_Arena} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace AC {

//.${Shared::AO_Arena} .......................................................
QP::QActive * const AO_Arena = &Arena::instance;

} // namespace AC
//.$enddef${Shared::AO_Arena} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//============================================================================
// generate definition of the AO
//.$define${AOs::Arena} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace AC {

//.${AOs::Arena} .............................................................
Arena Arena::instance;
//.${AOs::Arena::Arena} ......................................................
Arena::Arena()
: QActive(Q_STATE_CAST(&Arena::initial))
{}

//.${AOs::Arena::SM} .........................................................
Q_STATE_DEF(Arena, initial) {
    //.${AOs::Arena::SM::initial}
    FSP::Arena_initializeAndSubscribe(this, e);

    QS_FUN_DICTIONARY(&Arena::ArenaOn);
    QS_FUN_DICTIONARY(&Arena::AllOn);
    QS_FUN_DICTIONARY(&Arena::AllOff);
    QS_FUN_DICTIONARY(&Arena::StreamingFrame);

    return tran(&ArenaOn);
}
//.${AOs::Arena::SM::ArenaOn} ................................................
Q_STATE_DEF(Arena, ArenaOn) {
    QP::QState status_;
    switch (e->sig) {
        //.${AOs::Arena::SM::ArenaOn}
        case Q_ENTRY_SIG: {
            FSP::Arena_activateCommandInterfaces(this, e);
            status_ = Q_RET_HANDLED;
            break;
        }
        //.${AOs::Arena::SM::ArenaOn}
        case Q_EXIT_SIG: {
            FSP::Arena_deactivateCommandInterfaces(this, e);
            status_ = Q_RET_HANDLED;
            break;
        }
        //.${AOs::Arena::SM::ArenaOn::initial}
        case Q_INIT_SIG: {
            status_ = tran(&AllOff);
            break;
        }
        //.${AOs::Arena::SM::ArenaOn::ALL_OFF}
        case ALL_OFF_SIG: {
            status_ = tran(&AllOff);
            break;
        }
        //.${AOs::Arena::SM::ArenaOn::RESET}
        case RESET_SIG: {
            status_ = tran(&AllOff);
            break;
        }
        //.${AOs::Arena::SM::ArenaOn::STREAM_FRAME}
        case STREAM_FRAME_SIG: {
            FSP::Arena_fillFrameBufferWithStream(this, e);
            status_ = tran(&StreamingFrame);
            break;
        }
        //.${AOs::Arena::SM::ArenaOn::ALL_ON}
        case ALL_ON_SIG: {
            FSP::Arena_fillFrameBufferWithAllOn(this, e);
            status_ = tran(&AllOn);
            break;
        }
        default: {
            status_ = super(&top);
            break;
        }
    }
    return status_;
}
//.${AOs::Arena::SM::ArenaOn::AllOn} .........................................
Q_STATE_DEF(Arena, AllOn) {
    QP::QState status_;
    switch (e->sig) {
        //.${AOs::Arena::SM::ArenaOn::AllOn::FRAME_FILLED}
        case FRAME_FILLED_SIG: {
            FSP::Arena_displayFrames(this, e);
            status_ = Q_RET_HANDLED;
            break;
        }
        default: {
            status_ = super(&ArenaOn);
            break;
        }
    }
    return status_;
}
//.${AOs::Arena::SM::ArenaOn::AllOff} ........................................
Q_STATE_DEF(Arena, AllOff) {
    QP::QState status_;
    switch (e->sig) {
        //.${AOs::Arena::SM::ArenaOn::AllOff}
        case Q_ENTRY_SIG: {
            FSP::Arena_deactivateDisplay(this, e);
            status_ = Q_RET_HANDLED;
            break;
        }
        default: {
            status_ = super(&ArenaOn);
            break;
        }
    }
    return status_;
}
//.${AOs::Arena::SM::ArenaOn::StreamingFrame} ................................
Q_STATE_DEF(Arena, StreamingFrame) {
    QP::QState status_;
    switch (e->sig) {
        //.${AOs::Arena::SM::ArenaOn::StreamingFrame::FRAME_FILLED}
        case FRAME_FILLED_SIG: {
            FSP::Arena_displayFrames(this, e);
            status_ = Q_RET_HANDLED;
            break;
        }
        default: {
            status_ = super(&ArenaOn);
            break;
        }
    }
    return status_;
}

} // namespace AC
//.$enddef${AOs::Arena} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
