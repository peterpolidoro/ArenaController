//.$file${./ArenaControlle~::Frame.cpp} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//
// Model: ArenaController.qm
// File:  ${./ArenaControlle~::Frame.cpp}
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
//.$endhead${./ArenaControlle~::Frame.cpp} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#include "ArenaController.hpp"  // ArenaController application interface


using namespace QP;

//============================================================================
// generate declaration of the active object
//.$declare${AOs::Frame} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace AC {

//.${AOs::Frame} .............................................................
class Frame : public QP::QActive {
public:
    static Frame instance;

private:
    std::uint8_t panel_set_index_row_;
    std::uint8_t panel_set_index_col_;
    std::uint8_t const (*panel_buffer_)[];
    std::uint8_t panel_set_max_row_;
    std::uint8_t panel_set_max_col_;

public:
    Frame();

protected:
    Q_STATE_DECL(initial);
    Q_STATE_DECL(Inactive);
    Q_STATE_DECL(Active);
    Q_STATE_DECL(TransferringGrayscaleFrame);
    Q_STATE_DECL(TransferringPanelSet);
};

} // namespace AC
//.$enddecl${AOs::Frame} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//============================================================================
// generate definition of to opaque pointer to the AO
//.$skip${QP_VERSION} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//. Check for the minimum required QP version
#if (QP_VERSION < 690U) || (QP_VERSION != ((QP_RELEASE^4294967295U) % 0x3E8U))
#error qpcpp version 6.9.0 or higher required
#endif
//.$endskip${QP_VERSION} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//.$define${Shared::AO_Frame} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace AC {

//.${Shared::AO_Frame} .......................................................
QP::QActive * const AO_Frame = &Frame::instance;

} // namespace AC
//.$enddef${Shared::AO_Frame} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//============================================================================
// generate definition of the AO
//.$define${AOs::Frame} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace AC {

//.${AOs::Frame} .............................................................
Frame Frame::instance;
//.${AOs::Frame::Frame} ......................................................
Frame::Frame()
: QActive(Q_STATE_CAST(&Frame::initial))
{}

//.${AOs::Frame::SM} .........................................................
Q_STATE_DEF(Frame, initial) {
    //.${AOs::Frame::SM::initial}
    BSP::initializeFrame();
    subscribe(TRANSFER_UNIFORM_GRAYSCALE_FRAME_SIG);
    subscribe(PANEL_SET_TRANSFERRED_SIG);
    subscribe(FRAME_TRANSFERRED_SIG);
    return tran(&Inactive);
}
//.${AOs::Frame::SM::Inactive} ...............................................
Q_STATE_DEF(Frame, Inactive) {
    QP::QState status_;
    switch (e->sig) {
        //.${AOs::Frame::SM::Inactive}
        case Q_ENTRY_SIG: {
            BSP::ledOff();
            status_ = Q_RET_HANDLED;
            break;
        }
        //.${AOs::Frame::SM::Inactive::TRANSFER_UNIFORM_GRAYSCALE_FRAME}
        case TRANSFER_UNIFORM_GRAYSCALE_FRAME_SIG: {
            panel_buffer_ = Q_EVT_CAST(TransferUniformGrayscaleFrameEvt)->panel_buffer;
            panel_set_max_row_ = BSP::getPanelSetMaxRow();
            panel_set_max_col_ = BSP::getPanelSetMaxCol();
            status_ = tran(&TransferringGrayscaleFrame);
            break;
        }
        default: {
            status_ = super(&top);
            break;
        }
    }
    return status_;
}
//.${AOs::Frame::SM::Active} .................................................
Q_STATE_DEF(Frame, Active) {
    QP::QState status_;
    switch (e->sig) {
        //.${AOs::Frame::SM::Active}
        case Q_ENTRY_SIG: {
            BSP::ledOn();
            panel_set_index_row_ = 0;
            panel_set_index_col_ = 0;
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
//.${AOs::Frame::SM::Active::TransferringGrayscaleFrame} .....................
Q_STATE_DEF(Frame, TransferringGrayscaleFrame) {
    QP::QState status_;
    switch (e->sig) {
        //.${AOs::Frame::SM::Active::TransferringGray~::initial}
        case Q_INIT_SIG: {
            status_ = tran(&TransferringPanelSet);
            break;
        }
        //.${AOs::Frame::SM::Active::TransferringGray~::FRAME_TRANSFERRED}
        case FRAME_TRANSFERRED_SIG: {
            status_ = tran(&Inactive);
            break;
        }
        default: {
            status_ = super(&Active);
            break;
        }
    }
    return status_;
}
//.${AOs::Frame::SM::Active::TransferringGray~::TransferringPanelSet} ........
Q_STATE_DEF(Frame, TransferringPanelSet) {
    QP::QState status_;
    switch (e->sig) {
        //.${AOs::Frame::SM::Active::TransferringGray~::TransferringPanelSet}
        case Q_ENTRY_SIG: {
            BSP::enablePanelSetSelectPin(panel_set_index_row_, panel_set_index_col_);
            BSP::transferPanelSet(panel_buffer_, AC::constants::byte_count_per_panel_grayscale);
            status_ = Q_RET_HANDLED;
            break;
        }
        //.${AOs::Frame::SM::Active::TransferringGray~::TransferringPanelSet}
        case Q_EXIT_SIG: {
            BSP::disablePanelSetSelectPin(panel_set_index_row_, panel_set_index_col_);
            ++panel_set_index_row_;
            if (panel_set_index_row_ == panel_set_max_row_)
            {
                panel_set_index_row_ = 0;
                ++panel_set_index_col_;
            }
            if (panel_set_index_col_ == panel_set_max_col_)
            {
                panel_set_index_col_ = 0;
            }
            status_ = Q_RET_HANDLED;
            break;
        }
        //.${AOs::Frame::SM::Active::TransferringGray~::TransferringPane~::PANEL_SET_TRANSFERRED}
        case PANEL_SET_TRANSFERRED_SIG: {
            //.${AOs::Frame::SM::Active::TransferringGray~::TransferringPane~::PANEL_SET_TRANSF~::[frameNotTransferred()]}
            if ((panel_set_index_row_ != (panel_set_max_row_-1)) || (panel_set_index_col_ != (panel_set_max_col_-1))) {
                status_ = tran(&TransferringPanelSet);
            }
            //.${AOs::Frame::SM::Active::TransferringGray~::TransferringPane~::PANEL_SET_TRANSF~::[else]}
            else {
                static QEvt const frameTransferredEvt = { AC::FRAME_TRANSFERRED_SIG, 0U, 0U};
                QF::PUBLISH(&frameTransferredEvt, this);
                status_ = Q_RET_HANDLED;
            }
            break;
        }
        default: {
            status_ = super(&TransferringGrayscaleFrame);
            break;
        }
    }
    return status_;
}

} // namespace AC
//.$enddef${AOs::Frame} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
