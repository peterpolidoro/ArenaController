#include <Arduino.h>
#include <Watchdog_t4.h>
#include <QNEthernet.h>
#include <TimerOne.h>
#include <SPI.h>
#include <EventResponder.h>
#include <SdFat.h>

#include "bsp.hpp"
#include "ArenaController.hpp"


using namespace QP;
using namespace qindesign::network;

namespace AC
{
namespace constants
{
// SPI Settings
constexpr uint32_t spi_clock_speed = 5000000;

constexpr uint8_t reset_pin = 34;

// frame
constexpr uint8_t panel_count_per_frame_row_max = 5;
constexpr uint8_t panel_count_per_frame_col_max = 12;
constexpr uint8_t panel_count_per_frame_max = \
  panel_count_per_frame_row_max * panel_count_per_frame_col_max; // 60
constexpr uint16_t byte_count_per_frame_grayscale_max = \
  panel_count_per_frame_max * \
  byte_count_per_panel_grayscale; // 7920

// region
constexpr uint8_t region_count_per_frame = 2;
constexpr SPIClass * region_spi_ptrs[region_count_per_frame] = {&SPI, &SPI1};
constexpr uint8_t region_cipo_pins[region_count_per_frame] = {12, 1};

constexpr uint8_t region_row_panel_count_max = panel_count_per_frame_row_max;
constexpr uint8_t region_col_panel_count_max = \
  panel_count_per_frame_col_max/region_count_per_frame; // 6

constexpr uint8_t panel_set_select_pins[region_row_panel_count_max][region_col_panel_count_max] =
{
  {0, 6, 24, 31, 20, 39},
  {2, 7, 25, 32, 17, 38},
  {3, 8, 28, 23, 16, 37},
  {4, 9, 29, 22, 41, 36},
  {5, 10, 30, 21, 40, 35}
};

// files
constexpr char base_dir_str[] = "patterns";
constexpr uint8_t filename_length_max = 15;
constexpr uint16_t frame_count_y_max = 1;
constexpr uint16_t frame_count_x_max = 20;
} // namespace constants
} // namespace AC

//----------------------------------------------------------------------------
// QS facilities

static QP::QSpyId const l_BSP_ID = {1U}; // QSpy source ID

//----------------------------------------------------------------------------
// Static global variables
static QEvt const panelSetTransferredEvt = {AC::PANEL_SET_TRANSFERRED_SIG, 0U, 0U};

static WDT_T4<WDT1> wdt;
static EventResponder transfer_panel_complete_event;
static uint8_t transfer_panel_complete_count;

// Serial Communication Interface
static HardwareSerial & serial_communication_interface_stream = Serial1;
static usb_serial_class & qs_serial_stream = Serial;

// usb_serial_class & serial_communication_interface_stream = Serial;
// HardwareSerial & qs_serial_stream = Serial1;

static EthernetServer ethernet_server{AC::constants::ethernet_server_port};
static IPAddress static_ip{192, 168, 10, 62};
static IPAddress subnet_mask{255, 255, 255, 0};
static IPAddress gateway{192, 168, 10, 1};

//----------------------------------------------------------------------------
// Local functions
void watchdogCallback ()
{
}

void ipAddressToString(IPAddress ip_address, char * ip_address_str)
{
  sprintf(ip_address_str,"%u.%u.%u.%u", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
}

//----------------------------------------------------------------------------
// BSP functions

void BSP::init()
{
  // initialize the hardware used in this sketch...
  // NOTE: interrupts are configured and started later in QF::onStartup()

  // setup pins
  pinMode(LED_BUILTIN, OUTPUT);
  ledOff();

  for (uint8_t region_index = 0; region_index<AC::constants::region_count_per_frame; ++region_index)
  {
    pinMode(AC::constants::region_cipo_pins[region_index], INPUT);
    SPIClass * spi_ptr = AC::constants::region_spi_ptrs[region_index];
    spi_ptr->begin();
  }
  QS_OBJ_DICTIONARY(&l_BSP_ID);
}

void BSP::ledOff()
{
  digitalWriteFast(LED_BUILTIN, LOW);
}

void BSP::ledOn()
{
  digitalWriteFast(LED_BUILTIN, HIGH);
}

void BSP::initializeWatchdog()
{
  WDT_timings_t config;
  config.trigger = AC::constants::watchdog_trigger_seconds;
  config.timeout = AC::constants::watchdog_timeout_seconds;
  config.callback = watchdogCallback;
  wdt.begin(config);
}

void BSP::feedWatchdog()
{
  wdt.feed();
}

void BSP::initializeArena()
{
  pinMode(AC::constants::reset_pin, OUTPUT);
  digitalWriteFast(AC::constants::reset_pin, LOW);
}

void transferPanelCompleteCallback(EventResponderRef event_responder)
{
  ++transfer_panel_complete_count;
  if (transfer_panel_complete_count == AC::constants::region_count_per_frame)
  {
    QF::PUBLISH(&panelSetTransferredEvt, &l_BSP_ID);
  }
}

void BSP::initializeFrame()
{
  transfer_panel_complete_event.attachImmediate(&transferPanelCompleteCallback);
  for (uint8_t panel_set_col_index = 0; panel_set_col_index<AC::constants::region_col_panel_count_max; ++panel_set_col_index)
  {
    for (uint8_t panel_set_row_index = 0; panel_set_row_index<AC::constants::region_row_panel_count_max; ++panel_set_row_index)
    {
      const uint8_t & pss_pin = AC::constants::panel_set_select_pins[panel_set_row_index][panel_set_col_index];
      pinMode(pss_pin, OUTPUT);
      digitalWriteFast(pss_pin, HIGH);
    }
  }
}

bool BSP::beginSerial()
{
  serial_communication_interface_stream.begin(AC::constants::serial_baud_rate);
  serial_communication_interface_stream.setTimeout(AC::constants::serial_timeout);
  return true;
}

bool BSP::pollSerialCommand()
{
  return serial_communication_interface_stream.available();
}

uint8_t BSP::readSerialByte()
{
  return serial_communication_interface_stream.read();
}

void BSP::readSerialStringCommand(char * command_str, char first_char)
{
  char command_tail[AC::constants::string_command_length_max];
  size_t chars_read = serial_communication_interface_stream.readBytesUntil(AC::constants::command_termination_character,
    command_tail, AC::constants::string_command_length_max - 1);
  command_tail[chars_read] = '\0';
  command_str[0] = first_char;
  command_str[1] = '\0';
  strcat(command_str, command_tail);
}

void BSP::writeSerialStringResponse(char * response)
{
  serial_communication_interface_stream.println(response);
}

// void BSP::readSerialStringCommand(uint8_t first_byte)
// {
//     String command_tail = serial_communication_interface_stream.readStringUntil('\n');
//     String command_string = String(String((char)first_byte) + command_tail);
//     // size_t bytes_available = serial_communication_interface_stream.available();
//     // byte command_byte_array[bytes_available];
//     // Serial.readBytes(command_byte_array, bytes_available);

//     // char command_str[bytes_available + 2];
//     // command_str[0] = first_byte;
//     // // memcpy(command_str + 1, command_byte_array, bytes_available);
//     // // memcpy(command_str, command_byte_array, bytes_available);
//     // command_str[bytes_available] = 0;
//     // String command_string = String(command_str);
//     String response = processCommandString(command_string);
//     Serial.println(response);
//     QF::PUBLISH(&commandProcessedEvt, &l_BSP_ID);
//    return;

//   // size_t bytes_available = serial_communication_interface_stream.available();
//   // if (bytes_available)
//   // {

//     // byte command_byte_array[bytes_available];
//     // Serial.readBytes(command_byte_array, bytes_available);
//   // }
// }

bool BSP::beginEthernet()
{
  return Ethernet.begin(static_ip, subnet_mask, gateway);
}

bool BSP::checkForEthernetIPAddress()
{
  return Ethernet.localIP();
}

void BSP::getServerIpAddressString(char * ip_address_str)
{
  IPAddress ip_address = Ethernet.localIP();
  ipAddressToString(ip_address, ip_address_str);
}

void BSP::getEthernetHardwareStatusString(char * hardware_status_str)
{
  hardware_status_str[0] = '\0';
  EthernetHardwareStatus status = Ethernet.hardwareStatus();
  switch (status)
  {
    case EthernetNoHardware:
      strcpy(hardware_status_str, "NONE");
      break;
    case EthernetW5100:
      strcpy(hardware_status_str, "W5100");
      break;
    case EthernetW5200:
      strcpy(hardware_status_str, "W5200");
      break;
    case EthernetW5500:
      strcpy(hardware_status_str, "W5500");
      break;
    case EthernetTeensy41:
      strcpy(hardware_status_str, "TEENSY41");
      break;
    case EthernetOtherHardware:
      strcpy(hardware_status_str, "OTHER");
      break;
  }
}

void BSP::getEthernetLinkStatusString(char * link_status_str)
{
  // Serial.print("hasLinkState: ");
  // Serial.println(Ethernet.driverCapabilities().hasLinkState);
  link_status_str[0] = '\0';
  EthernetLinkStatus status = Ethernet.linkStatus();
  switch (status)
  {
    case Unknown:
      strcpy(link_status_str, "UNKNOWN");
      break;
    case LinkON:
      strcpy(link_status_str, "ON");
      break;
    case LinkOFF:
      strcpy(link_status_str, "OFF");
      break;
  }
}

bool BSP::beginEthernetServer()
{
  ethernet_server.begin();
  return ethernet_server ? true : false;
}

bool BSP::pollEthernetCommand()
{
  EthernetClient ethernet_client = ethernet_server.accept();
  if (ethernet_client)
  {
    IPAddress ip = ethernet_client.remoteIP();
    ethernet_client.close();
  }
  return false;
}

void BSP::readEthernetBinaryCommand()
{
  // EthernetClient ethernet_client = ethernet_server.accept();
  // uint16_t available_bytes = ethernet_client.available();
  // Serial.print("available_bytes: ");
  // Serial.println(available_bytes);
}

void BSP::displayFrame()
{
  delay(2);
  // QF::PUBLISH(&frameDisplayedEvt, &l_BSP_ID);
}

uint8_t BSP::getRegionRowPanelCountMax()
{
  return AC::constants::region_row_panel_count_max;
}

uint8_t BSP::getRegionColPanelCountMax()
{
  return AC::constants::region_col_panel_count_max;
}

void BSP::enablePanelSetSelectPin(uint8_t row_index, uint8_t col_index)
{
  for (uint8_t region_index = 0; region_index<AC::constants::region_count_per_frame; ++region_index)
  {
    SPIClass * spi_ptr = AC::constants::region_spi_ptrs[region_index];
    spi_ptr->beginTransaction(SPISettings(AC::constants::spi_clock_speed, AC::constants::spi_bit_order, AC::constants::spi_data_mode));
  }
  const uint8_t & pss_pin = AC::constants::panel_set_select_pins[row_index][col_index];
  digitalWriteFast(pss_pin, LOW);
  // Serial.print("setting ");
  // Serial.print(pss_pin);
  // Serial.println(" LOW");
}

void BSP::disablePanelSetSelectPin(uint8_t row_index, uint8_t col_index)
{
  const uint8_t & pss_pin = AC::constants::panel_set_select_pins[row_index][col_index];
  digitalWriteFast(pss_pin, HIGH);
  for (uint8_t region_index = 0; region_index<AC::constants::region_count_per_frame; ++region_index)
  {
    SPIClass * spi_ptr = AC::constants::region_spi_ptrs[region_index];
    spi_ptr->endTransaction();
  }
  // Serial.print("setting ");
  // Serial.print(pss_pin);
  // Serial.println(" HIGH");
}

void BSP::transferPanelSet(const uint8_t (*panel_buffer)[], uint8_t panel_buffer_byte_count)
{
  transfer_panel_complete_count = 0;
  for (uint8_t region_index = 0; region_index<AC::constants::region_count_per_frame; ++region_index)
  {
    SPIClass * spi_ptr = AC::constants::region_spi_ptrs[region_index];
    spi_ptr->transfer(panel_buffer, NULL, panel_buffer_byte_count, transfer_panel_complete_event);
  }
}

//----------------------------------------------------------------------------
// QF callbacks...

//
// NOTE: The usual source of system clock tick in ARM Cortex-M (SysTick timer)
// is aready used by the Arduino library. Therefore, this code uses a different
// hardware Timer1 of the Teensy 4 board for providing the system clock tick.
//
// NOTE: You can re-define the macros to use a different ATSAM timer/channel.
//

#define TIMER1_CLCK_HZ  1000000
#define TIMER_HANDLER   T1_Handler

// interrupts.................................................................
void TIMER_HANDLER()
{
  QF::TICK_X(0, &l_BSP_ID); // process time events for tick rate 0
}
//............................................................................
void QF::onStartup()
{
  // configure the timer-counter channel........
  Timer1.initialize(TIMER1_CLCK_HZ / AC::constants::ticks_per_second);
  Timer1.attachInterrupt(TIMER_HANDLER);
  // ...
}
//............................................................................
void QV::onIdle()
{ // called with interrupts DISABLED
#ifdef NDEBUG
  // Put the CPU and peripherals to the low-power mode. You might
  // need to customize the clock management for your application,
  // see the datasheet for your particular MCU.
  QV_CPU_SLEEP();  // atomically go to sleep and enable interrupts
#else
  QF_INT_ENABLE(); // simply re-enable interrupts

  // transmit QS outgoing data (QS-TX)
  uint16_t len = qs_serial_stream.availableForWrite();
  if (len > 0U)
  { // any space available in the output buffer?
    uint8_t const *buf = QS::getBlock(&len);
    if (buf)
    {
      qs_serial_stream.write(buf, len); // asynchronous and non-blocking
    }
  }

  // receive QS incoming data (QS-RX)
  len = qs_serial_stream.available();
  if (len > 0U)
  {
    do
    {
      QP::QS::rxPut(qs_serial_stream.read());
    } while (--len > 0U);
    QS::rxParse();
  }
#endif
}
//............................................................................
extern "C" Q_NORETURN Q_onAssert(char const * const module, int location)
{
  //
  // NOTE: add here your application-specific error handling
  //
  (void)module;
  (void)location;

  QF_INT_DISABLE(); // disable all interrupts
  BSP::ledOn();  // trun the LED on
  for (;;)
  { // freeze in an endless loop for now...
  }
}

//----------------------------------------------------------------------------
// QS callbacks...
//............................................................................
bool QP::QS::onStartup(void const * arg)
{
  static uint8_t qsTxBuf[2048]; // buffer for QS transmit channel (QS-TX)
  static uint8_t qsRxBuf[1024];  // buffer for QS receive channel (QS-RX)
  initBuf  (qsTxBuf, sizeof(qsTxBuf));
  rxInitBuf(qsRxBuf, sizeof(qsRxBuf));
  qs_serial_stream.begin(115200); // run serial port at 115200 baud rate
  return true; // return success
}
//............................................................................
void QP::QS::onCommand(uint8_t cmdId, uint32_t param1,
  uint32_t param2, uint32_t param3)
{
}

//............................................................................
void QP::QS::onCleanup()
{
}
//............................................................................
QP::QSTimeCtr QP::QS::onGetTime()
{
  return millis();
}
//............................................................................
void QP::QS::onFlush()
{
  uint16_t len = 0xFFFFU; // big number to get as many bytes as available
  uint8_t const *buf = QS::getBlock(&len); // get continguous block of data
  while (buf != nullptr)
  { // data available?
    qs_serial_stream.write(buf, len); // might poll until all bytes fit
    len = 0xFFFFU; // big number to get as many bytes as available
    buf = QS::getBlock(&len); // try to get more data
  }
  qs_serial_stream.flush(); // wait for the transmission of outgoing data to complete
}
//............................................................................
void QP::QS::onReset()
{
  WDT_timings_t config;
  config.timeout = 0.1;
  wdt.begin(config);
}
