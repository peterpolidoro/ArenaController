//$file${.::ArenaController.hpp} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//
// Model: ArenaController.qm
// File:  ${.::ArenaController.hpp}
//
// This code has been generated by QM 5.2.3 <www.state-machine.com/qm>.
// DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//
// This generated code is open source software: you can redistribute it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation.
//
// This code is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
// more details.
//
// NOTE:
// Alternatively, this generated code may be distributed under the terms
// of Quantum Leaps commercial licenses, which expressly supersede the GNU
// General Public License and are specifically designed for licensees
// interested in retaining the proprietary status of their code.
//
// Contact information:
// <www.state-machine.com/licensing>
// <info@state-machine.com>
//
//$endhead${.::ArenaController.hpp} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#ifndef ARENA_CONTROLLER_HPP
#define ARENA_CONTROLLER_HPP

#include "qpcpp.hpp"
#include "fsp.hpp"


namespace ArduinoInterface
{
void setup();
void loop();
}

//$declare${Shared} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace AC {

//${Shared::SetParameterEvt} .................................................
class SetParameterEvt : public QP::QEvt {
public:
    std::uint32_t value;
}; // class SetParameterEvt

//${Shared::EthernetCommandEvt} ..............................................
class EthernetCommandEvt : public QP::QEvt {
public:
    void * connection;
    std::uint8_t const * binary_command;
    std::uint32_t binary_command_byte_count;
}; // class EthernetCommandEvt

//${Shared::DisplayPatternEvt} ...............................................
class DisplayPatternEvt : public QP::QEvt {
public:
    std::uint16_t pattern_id;
    std::int16_t frame_rate;
    std::uint16_t runtime_duration;
}; // class DisplayPatternEvt

//${Shared::AO_Arena} ........................................................
extern QP::QActive * const AO_Arena;

//${Shared::AO_Display} ......................................................
extern QP::QActive * const AO_Display;

//${Shared::AO_SerialCommandInterface} .......................................
extern QP::QActive * const AO_SerialCommandInterface;

//${Shared::AO_EthernetCommandInterface} .....................................
extern QP::QActive * const AO_EthernetCommandInterface;

//${Shared::AO_Frame} ........................................................
extern QP::QActive * const AO_Frame;

//${Shared::AO_Watchdog} .....................................................
extern QP::QActive * const AO_Watchdog;

//${Shared::AO_Pattern} ......................................................
extern QP::QActive * const AO_Pattern;

} // namespace AC
//$enddecl${Shared} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

#endif
