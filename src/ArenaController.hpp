//.$file${.::ArenaController.hpp} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//
// Model: ArenaController.qm
// File:  ${.::ArenaController.hpp}
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
//.$endhead${.::ArenaController.hpp} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#ifndef ARENA_CONTROLLER_HPP
#define ARENA_CONTROLLER_HPP

#include "qpcpp.hpp"
#include "fsp.hpp"


namespace ArduinoInterface
{
void setup();
void loop();
}

//.$declare${Shared} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace AC {

//.${Shared::CommandEvt} .....................................................
class CommandEvt : public QP::QEvt {};
//.${Shared::DisplayFramesEvt} ...............................................
class DisplayFramesEvt : public QP::QEvt {
public:
    std::uint8_t const (*panel_buffer)[];
    std::uint8_t panel_buffer_byte_count;
};
//.${Shared::TransferFrameEvt} ...............................................
class TransferFrameEvt : public QP::QEvt {
public:
    std::uint8_t const (*panel_buffer)[];
    std::uint8_t panel_buffer_byte_count;
    std::uint8_t region_row_panel_count;
    std::uint8_t region_col_panel_count;
};
//.${Shared::SetDisplayFrequencyEvt} .........................................
class SetDisplayFrequencyEvt : public QP::QEvt {
public:
    std::uint32_t display_frequency_hz;
};
//.${Shared::EthernetCommandEvt} .............................................
class EthernetCommandEvt : public QP::QEvt {
public:
    void * connection;
    std::uint8_t const (*binary_command)[];
    std::uint32_t binary_command_byte_count;
};
extern QP::QActive * const AO_Arena;
extern QP::QActive * const AO_Display;
extern QP::QActive * const AO_SerialCommandInterface;
extern QP::QActive * const AO_EthernetCommandInterface;
extern QP::QActive * const AO_Frame;
extern QP::QActive * const AO_Watchdog;
//.${Shared::getRegionInstance} ..............................................
QP::QHsm * getRegionInstance(std::uint8_t id) ;

} // namespace AC
//.$enddecl${Shared} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

#endif