//.$file${.::EthernetCommandInterface.hpp} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//
// Model: ArenaController.qm
// File:  ${.::EthernetCommandInterface.hpp}
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
//.$endhead${.::EthernetCommandInterface.hpp} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#ifndef ETHERNET_COMMAND_INTERFACE_HPP
#define ETHERNET_COMMAND_INTERFACE_HPP

#include "ArenaController.hpp"


//============================================================================
// generate declaration of the active object
//.$declare${AOs::EthernetCommandInterface} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace AC {

//.${AOs::EthernetCommandInterface} ..........................................
class EthernetCommandInterface : public QP::QActive {
public:
    static EthernetCommandInterface instance;
    QP::QTimeEvt ethernet_time_evt_;
    std::uint8_t binary_response_[constants::byte_count_per_response_max];
    std::uint8_t binary_response_byte_count_;
    void * connection_;
    std::uint8_t const * binary_command_;
    std::uint32_t binary_command_byte_count_;

public:
    EthernetCommandInterface();

protected:
    Q_STATE_DECL(initial);
    Q_STATE_DECL(Inactive);
    Q_STATE_DECL(Active);
    Q_STATE_DECL(Unintitalized);
    Q_STATE_DECL(WaitingForNewCommand);
    Q_STATE_DECL(CreatingServerConnection);
    Q_STATE_DECL(Waiting);
    Q_STATE_DECL(ProcessingBinaryCommand);
    Q_STATE_DECL(ChoosingCommandProcessor);
};

} // namespace AC
//.$enddecl${AOs::EthernetCommandInterface} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

#endif