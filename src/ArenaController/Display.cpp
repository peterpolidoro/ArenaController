//.$file${./ArenaControlle~::Display.cpp} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//
// Model: ArenaController.qm
// File:  ${./ArenaControlle~::Display.cpp}
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
//.$endhead${./ArenaControlle~::Display.cpp} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#include "Display.hpp"


using namespace QP;

//============================================================================
// generate definition of to opaque pointer to the AO
//.$skip${QP_VERSION} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//. Check for the minimum required QP version
#if (QP_VERSION < 690U) || (QP_VERSION != ((QP_RELEASE^4294967295U) % 0x3E8U))
#error qpcpp version 6.9.0 or higher required
#endif
//.$endskip${QP_VERSION} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//.$define${Shared::AO_Display} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace AC {

//.${Shared::AO_Display} .....................................................
QP::QActive * const AO_Display = &Display::instance;

} // namespace AC
//.$enddef${Shared::AO_Display} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//============================================================================
// generate definition of the AO
//.$define${AOs::Display} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace AC {

//.${AOs::Display} ...........................................................
Display Display::instance;
//.${AOs::Display::Display} ..................................................
Display::Display()
: QActive(Q_STATE_CAST(&Display::initial))
{}

//.${AOs::Display::SM} .......................................................
Q_STATE_DEF(Display, initial) {
    //.${AOs::Display::SM::initial}
    BSP::initializeDisplay();
    subscribe(DEACTIVATE_DISPLAY_SIG);
    subscribe(DISPLAY_FRAMES_SIG);
    subscribe(DISPLAY_FRAME_TIMEOUT_SIG);
    subscribe(FRAME_TRANSFERRED_SIG);
    display_frequency_hz_ = constants::display_frequency_hz_default;
    return tran(&Initialized);
}
//.${AOs::Display::SM::Initialized} ..........................................
Q_STATE_DEF(Display, Initialized) {
    QP::QState status_;
    switch (e->sig) {
        //.${AOs::Display::SM::Initialized::initial}
        case Q_INIT_SIG: {
            status_ = tran(&Inactive);
            break;
        }
        //.${AOs::Display::SM::Initialized::SET_DISPLAY_FREQUENCY}
        case SET_DISPLAY_FREQUENCY_SIG: {
            display_frequency_hz_ = Q_EVT_CAST(SetDisplayFrequencyEvt)->display_frequency_hz;
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
//.${AOs::Display::SM::Initialized::Inactive} ................................
Q_STATE_DEF(Display, Inactive) {
    QP::QState status_;
    switch (e->sig) {
        //.${AOs::Display::SM::Initialized::Inactive::DISPLAY_FRAMES}
        case DISPLAY_FRAMES_SIG: {
            panel_buffer_ = Q_EVT_CAST(DisplayFramesEvt)->panel_buffer;
            panel_buffer_byte_count_ = Q_EVT_CAST(DisplayFramesEvt)->panel_buffer_byte_count;
            status_ = tran(&DisplayingFrames);
            break;
        }
        default: {
            status_ = super(&Initialized);
            break;
        }
    }
    return status_;
}
//.${AOs::Display::SM::Initialized::Active} ..................................
Q_STATE_DEF(Display, Active) {
    QP::QState status_;
    switch (e->sig) {
        //.${AOs::Display::SM::Initialized::Active::DEACTIVATE_DISPLAY}
        case DEACTIVATE_DISPLAY_SIG: {
            status_ = tran(&Inactive);
            break;
        }
        default: {
            status_ = super(&Initialized);
            break;
        }
    }
    return status_;
}
//.${AOs::Display::SM::Initialized::Active::DisplayingFrames} ................
Q_STATE_DEF(Display, DisplayingFrames) {
    QP::QState status_;
    switch (e->sig) {
        //.${AOs::Display::SM::Initialized::Active::DisplayingFrames}
        case Q_ENTRY_SIG: {
            BSP::armDisplayFrameTimer(display_frequency_hz_);
            status_ = Q_RET_HANDLED;
            break;
        }
        //.${AOs::Display::SM::Initialized::Active::DisplayingFrames}
        case Q_EXIT_SIG: {
            BSP::disarmDisplayFrameTimer();
            status_ = Q_RET_HANDLED;
            break;
        }
        //.${AOs::Display::SM::Initialized::Active::DisplayingFrames::initial}
        case Q_INIT_SIG: {
            status_ = tran(&WaitingToDisplayFrame);
            break;
        }
        //.${AOs::Display::SM::Initialized::Active::DisplayingFrames::SET_DISPLAY_FREQUENCY}
        case SET_DISPLAY_FREQUENCY_SIG: {
            display_frequency_hz_ = Q_EVT_CAST(SetDisplayFrequencyEvt)->display_frequency_hz;
            status_ = tran(&DisplayingFrames);
            break;
        }
        default: {
            status_ = super(&Active);
            break;
        }
    }
    return status_;
}
//.${AOs::Display::SM::Initialized::Active::DisplayingFrames::WaitingToDisplayFrame} 
Q_STATE_DEF(Display, WaitingToDisplayFrame) {
    QP::QState status_;
    switch (e->sig) {
        //.${AOs::Display::SM::Initialized::Active::DisplayingFrames::WaitingToDisplay~::DISPLAY_FRAME_TIMEOUT}
        case DISPLAY_FRAME_TIMEOUT_SIG: {
            status_ = tran(&DisplayingFrame);
            break;
        }
        default: {
            status_ = super(&DisplayingFrames);
            break;
        }
    }
    return status_;
}
//.${AOs::Display::SM::Initialized::Active::DisplayingFrames::DisplayingFrame} 
Q_STATE_DEF(Display, DisplayingFrame) {
    QP::QState status_;
    switch (e->sig) {
        //.${AOs::Display::SM::Initialized::Active::DisplayingFrames::DisplayingFrame}
        case Q_ENTRY_SIG: {
            static AC::TransferFrameEvt transferFrameEvt = { AC::TRANSFER_FRAME_SIG, 0U, 0U};
            transferFrameEvt.panel_buffer = panel_buffer_;
            transferFrameEvt.panel_buffer_byte_count = panel_buffer_byte_count_;
            transferFrameEvt.region_row_panel_count = BSP::getRegionRowPanelCountMax();
            transferFrameEvt.region_col_panel_count = BSP::getRegionColPanelCountMax();
            QF::PUBLISH(&transferFrameEvt, this);
            status_ = Q_RET_HANDLED;
            break;
        }
        //.${AOs::Display::SM::Initialized::Active::DisplayingFrames::DisplayingFrame::FRAME_TRANSFERRED}
        case FRAME_TRANSFERRED_SIG: {
            status_ = tran(&WaitingToDisplayFrame);
            break;
        }
        default: {
            status_ = super(&DisplayingFrames);
            break;
        }
    }
    return status_;
}

} // namespace AC
//.$enddef${AOs::Display} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
