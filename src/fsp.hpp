#ifndef FSP_HPP
#define FSP_HPP

#include <Arduino.h>

#include "constants.hpp"
#include "bsp.hpp"
#include "signals.hpp"
#include "records.hpp"
#include "pattern_header.hpp"

#include "Arena.hpp"
#include "Display.hpp"
#include "SerialCommandInterface.hpp"
#include "EthernetCommandInterface.hpp"
#include "Frame.hpp"
#include "Watchdog.hpp"
#include "Pattern.hpp"


struct FSP
{
  static void ArenaController_setup();

  static void Arena_initializeAndSubscribe(QP::QActive * const ao, QP::QEvt const * e);
  static void Arena_activateCommandInterfaces(QP::QActive * const ao, QP::QEvt const * e);
  static void Arena_deactivateCommandInterfaces(QP::QActive * const ao, QP::QEvt const * e);
  static void Arena_deactivateDisplay(QP::QActive * const ao, QP::QEvt const * e);
  static void Arena_displayFrame(QP::QActive * const ao, QP::QEvt const * e);
  static void Arena_fillFrameBufferWithAllOn(QP::QActive * const ao, QP::QEvt const * e);
  static void Arena_fillFrameBufferWithDecodedFrame(QP::QActive * const ao, QP::QEvt const * e);
  static void Arena_endDisplayingPattern(QP::QActive * const ao, QP::QEvt const * e);

  static void Display_initializeAndSubscribe(QP::QActive * const ao, QP::QEvt const * e);
  static void Display_setRefreshRate(QP::QActive * const ao, QP::QEvt const * e);
  static void Display_armRefreshTimer(QP::QActive * const ao, QP::QEvt const * e);
  static void Display_disarmRefreshTimer(QP::QActive * const ao, QP::QEvt const * e);
  static void Display_transferFrame(QP::QActive * const ao, QP::QEvt const * e);
  static void Display_defer(QP::QActive * const ao, QP::QEvt const * e);
  static void Display_recall(QP::QActive * const ao, QP::QEvt const * e);

  static void SerialCommandInterface_initializeAndSubscribe(QP::QActive * const ao, QP::QEvt const * e);
  static void SerialCommandInterface_armSerialTimer(QP::QActive * const ao, QP::QEvt const * e);
  static void SerialCommandInterface_disarmSerialTimer(QP::QActive * const ao, QP::QEvt const * e);
  static void SerialCommandInterface_beginSerial(QP::QActive * const ao, QP::QEvt const * e);
  static void SerialCommandInterface_pollSerialCommand(QP::QActive * const ao, QP::QEvt const * e);
  static void SerialCommandInterface_readFirstByte(QP::QActive * const ao, QP::QEvt const * e);
  static bool SerialCommandInterface_ifBinaryCommand(QP::QActive * const ao, QP::QEvt const * e);
  static void SerialCommandInterface_readSerialStringCommand(QP::QActive * const ao, QP::QEvt const * e);
  static void SerialCommandInterface_processStringCommand(QP::QActive * const ao, QP::QEvt const * e);
  static void SerialCommandInterface_writeSerialStringResponse(QP::QActive * const ao, QP::QEvt const * e);
  static void SerialCommandInterface_writeSerialBinaryResponse(QP::QActive * const ao, QP::QEvt const * e);

  static void EthernetCommandInterface_initializeAndSubscribe(QP::QActive * const ao, QP::QEvt const * e);
  static void EthernetCommandInterface_armEthernetTimer(QP::QActive * const ao, QP::QEvt const * e);
  static void EthernetCommandInterface_disarmEthernetTimer(QP::QActive * const ao, QP::QEvt const * e);
  static void EthernetCommandInterface_initializeEthernet(QP::QActive * const ao, QP::QEvt const * e);
  static void EthernetCommandInterface_pollEthernet(QP::QActive * const ao, QP::QEvt const * e);
  static void EthernetCommandInterface_createServerConnection(QP::QActive * const ao, QP::QEvt const * e);
  static void EthernetCommandInterface_analyzeCommand(QP::QActive * const ao, QP::QEvt const * e);
  static void EthernetCommandInterface_updateStreamCommand(QP::QActive * const ao, QP::QEvt const * e);
  static bool EthernetCommandInterface_ifStreamCommandComplete(QP::QActive * const ao, QP::QEvt const * e);
  static void EthernetCommandInterface_processBinaryCommand(QP::QActive * const ao, QP::QEvt const * e);
  static void EthernetCommandInterface_processStreamCommand(QP::QActive * const ao, QP::QEvt const * e);
  static void EthernetCommandInterface_writeBinaryResponse(QP::QActive * const ao, QP::QEvt const * e);

  static void Frame_initializeAndSubscribe(QP::QActive * const ao, QP::QEvt const * e);
  static void Frame_fillFrameBufferWithAllOn(QP::QActive * const ao, QP::QEvt const * e);
  static void Frame_fillFrameBufferWithDecodedFrame(QP::QActive * const ao, QP::QEvt const * e);
  static void Frame_reset(QP::QActive * const ao, QP::QEvt const * e);
  static void Frame_beginTransferPanelSet(QP::QActive * const ao, QP::QEvt const * e);
  static void Frame_endTransferPanelSet(QP::QActive * const ao, QP::QEvt const * e);
  static bool Frame_ifFrameNotTransferred(QP::QActive * const ao, QP::QEvt const * e);
  static void Frame_publishFrameTransferred(QP::QActive * const ao, QP::QEvt const * e);
  static void Frame_switchGrayscale(QP::QActive * const ao, QP::QEvt const * e);
  static void Frame_defer(QP::QActive * const ao, QP::QEvt const * e);
  static void Frame_recall(QP::QActive * const ao, QP::QEvt const * e);

  static void Watchdog_initializeAndSubscribe(QP::QActive * const ao, QP::QEvt const * e);
  static void Watchdog_armWatchdogTimer(QP::QActive * const ao, QP::QEvt const * e);
  static void Watchdog_disarmWatchdogTimer(QP::QActive * const ao, QP::QEvt const * e);
  static void Watchdog_feedWatchdog(QP::QActive * const ao, QP::QEvt const * e);

  static void Pattern_initializeAndSubscribe(QP::QActive * const ao, QP::QEvt const * e);
  static void Pattern_checkAndStoreParameters(QP::QActive * const ao, QP::QEvt const * e);
  static void Pattern_armInitializeCardTimer(QP::QActive * const ao, QP::QEvt const * e);
  static void Pattern_initializeCard(QP::QActive * const ao, QP::QEvt const * e);
  static void Pattern_postAllOff(QP::QActive * const ao, QP::QEvt const * e);
  static void Pattern_openFile(QP::QActive * const ao, QP::QEvt const * e);
  static void Pattern_closeFile(QP::QActive * const ao, QP::QEvt const * e);
  static void Pattern_checkFile(QP::QActive * const ao, QP::QEvt const * e);
  static void Pattern_checkPattern(QP::QActive * const ao, QP::QEvt const * e);
  static void Pattern_armTimers(QP::QActive * const ao, QP::QEvt const * e);
  static void Pattern_disarmTimers(QP::QActive * const ao, QP::QEvt const * e);
  static void Pattern_deactivateDisplay(QP::QActive * const ao, QP::QEvt const * e);
  static void Pattern_readNextFrameFromFile(QP::QActive * const ao, QP::QEvt const * e);
  static void Pattern_decodeFrame(QP::QActive * const ao, QP::QEvt const * e);
  static void Pattern_fillFrameBufferWithDecodedFrame(QP::QActive * const ao, QP::QEvt const * e);
  static void Pattern_defer(QP::QActive * const ao, QP::QEvt const * e);
  static void Pattern_recall(QP::QActive * const ao, QP::QEvt const * e);
  static void Pattern_displayFrame(QP::QActive * const ao, QP::QEvt const * e);

  static uint8_t processBinaryCommand(uint8_t const * command_buffer,
    size_t command_byte_count,
    uint8_t response[AC::constants::byte_count_per_response_max]);
  static void processStreamCommand(uint8_t const * buffer, uint32_t frame_byte_count);
  static void processStringCommand(const char * command, char * response);
};

#endif // FSP_HPP
