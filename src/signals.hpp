#ifndef SIGNALS_HPP
#define SIGNALS_HPP

#include "qpcpp.hpp"


namespace AC {

enum ArenaControllerSignals {

  // commands from serial or ethernet interface
  RESET_SIG = QP::Q_USER_SIG,
  ALL_ON_SIG,
  ALL_OFF_SIG,

  DEACTIVATE_DISPLAY_SIG,
  DISPLAY_FRAMES_SIG,
  TRANSFER_FRAME_SIG,
  PANEL_SET_TRANSFERRED_SIG,
  FRAME_TRANSFERRED_SIG,
  DISPLAY_FRAME_TIMEOUT_SIG,

  SERIAL_COMMAND_AVAILABLE_SIG,
  ETHERNET_COMMAND_AVAILABLE_SIG,
  COMMAND_PROCESSED_SIG,

  MAX_PUB_SIG,    // the last published signal

  // POST to SerialCommandInterface
  ACTIVATE_SERIAL_COMMAND_INTERFACE_SIG,
  DEACTIVATE_SERIAL_COMMAND_INTERFACE_SIG,
  SERIAL_READY_SIG,

  // POST to EthernetCommandInterface
  ACTIVATE_ETHERNET_COMMAND_INTERFACE_SIG,
  DEACTIVATE_ETHERNET_COMMAND_INTERFACE_SIG,
  ETHERNET_INITIALIZED_SIG,
  ETHERNET_IP_ADDRESS_FOUND_SIG,
  ETHERNET_SERVER_INITIALIZED_SIG,

  // POST to Display
  SET_DISPLAY_FREQUENCY_SIG,

  WATCHDOG_TIMEOUT_SIG, // signal for Watchdog timeout event
  SERIAL_TIMEOUT_SIG, // signal for SerialCommandInterface timeout event
  ETHERNET_TIMEOUT_SIG, // signal for EthernetCommandInterface timeout event

  MAX_SIG         // the last signal
};

} // namespace AC

#endif // SIGNALS_HPP
