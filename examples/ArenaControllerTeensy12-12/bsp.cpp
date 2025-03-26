#include <Arduino.h>
#include <Watchdog_t4.h>
#include <TimerOne.h>
#include <SPI.h>
#include <EventResponder.h>

#include "ArenaController.hpp"


extern "C" {
#include "mongoose_glue.h"
#define TRNG_ENT_COUNT 16
void ENET_IRQHandler(void);
uint64_t mg_millis(void) {
  return millis();
}
bool mg_random(void *buf, size_t len) {
  static bool initialised;
  static uint32_t rng_index = TRNG_ENT_COUNT;
  uint32_t r, i;

  if (!initialised) {
    initialised = true;
    CCM_CCGR6 |= CCM_CCGR6_TRNG(CCM_CCGR_ON);
    TRNG_MCTL = TRNG_MCTL_RST_DEF | TRNG_MCTL_PRGM;  // reset to program mode
    TRNG_MCTL = TRNG_MCTL_SAMP_MODE(2);  // start run mode, vonneumann
    TRNG_ENT15;  // discard any stale data, start gen cycle
  }

  for (i = 0; i < len; i++) {
    if (rng_index >= TRNG_ENT_COUNT) {
      rng_index = 0;
      while ((TRNG_MCTL & TRNG_MCTL_ENT_VAL) == 0 &&
             (TRNG_MCTL & TRNG_MCTL_ERR) == 0);  // wait for entropy ready
    }
    r = *(&TRNG_ENT0 + rng_index++);
    ((uint8_t *) buf)[i] = (uint8_t) (r & 255);
  }
  return true;
}
}

#define CLRSET(reg, clear, set) ((reg) = ((reg) & ~(clear)) | (set))
#define RMII_PAD_INPUT_PULLDOWN 0x30E9
#define RMII_PAD_INPUT_PULLUP 0xB0E9
#define RMII_PAD_CLOCK 0x0031

void trng_init() {
}

// initialize the ethernet hardware
void ethernet_init(void) {
  CCM_CCGR1 |= CCM_CCGR1_ENET(CCM_CCGR_ON);
  // configure PLL6 for 50 MHz, pg 1173
  CCM_ANALOG_PLL_ENET_CLR =
      CCM_ANALOG_PLL_ENET_POWERDOWN | CCM_ANALOG_PLL_ENET_BYPASS | 0x0F;
  CCM_ANALOG_PLL_ENET_SET = CCM_ANALOG_PLL_ENET_ENABLE |
                            CCM_ANALOG_PLL_ENET_BYPASS
                            /*| CCM_ANALOG_PLL_ENET_ENET2_REF_EN*/
                            | CCM_ANALOG_PLL_ENET_ENET_25M_REF_EN
                            /*| CCM_ANALOG_PLL_ENET_ENET2_DIV_SELECT(1)*/
                            | CCM_ANALOG_PLL_ENET_DIV_SELECT(1);
  while (
      !(CCM_ANALOG_PLL_ENET & CCM_ANALOG_PLL_ENET_LOCK));  // wait for PLL lock
  CCM_ANALOG_PLL_ENET_CLR = CCM_ANALOG_PLL_ENET_BYPASS;
  // configure REFCLK to be driven as output by PLL6, pg 326

  CLRSET(IOMUXC_GPR_GPR1,
         IOMUXC_GPR_GPR1_ENET1_CLK_SEL | IOMUXC_GPR_GPR1_ENET_IPG_CLK_S_EN,
         IOMUXC_GPR_GPR1_ENET1_TX_CLK_DIR);

  // Configure pins
  IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_14 = 5;  // Reset   B0_14 Alt5 GPIO7.15
  IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_15 = 5;  // Power   B0_15 Alt5 GPIO7.14
  GPIO7_GDIR |= (1 << 14) | (1 << 15);
  GPIO7_DR_SET = (1 << 15);                                    // Power on
  GPIO7_DR_CLEAR = (1 << 14);                                  // Reset PHY chip
  IOMUXC_SW_PAD_CTL_PAD_GPIO_B1_04 = RMII_PAD_INPUT_PULLDOWN;  // PhyAdd[0] = 0
  IOMUXC_SW_PAD_CTL_PAD_GPIO_B1_06 = RMII_PAD_INPUT_PULLDOWN;  // PhyAdd[1] = 1
  IOMUXC_SW_PAD_CTL_PAD_GPIO_B1_05 = RMII_PAD_INPUT_PULLUP;    // Slave mode
  IOMUXC_SW_PAD_CTL_PAD_GPIO_B1_11 = RMII_PAD_INPUT_PULLDOWN;  // Auto MDIX
  IOMUXC_SW_PAD_CTL_PAD_GPIO_B1_07 = RMII_PAD_INPUT_PULLUP;
  IOMUXC_SW_PAD_CTL_PAD_GPIO_B1_08 = RMII_PAD_INPUT_PULLUP;
  IOMUXC_SW_PAD_CTL_PAD_GPIO_B1_09 = RMII_PAD_INPUT_PULLUP;
  IOMUXC_SW_PAD_CTL_PAD_GPIO_B1_10 = RMII_PAD_CLOCK;
  IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_05 = 3;         // RXD1    B1_05 Alt3, pg 525
  IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_04 = 3;         // RXD0    B1_04 Alt3, pg 524
  IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_10 = 6 | 0x10;  // REFCLK  B1_10 Alt6, pg 530
  IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_11 = 3;         // RXER    B1_11 Alt3, pg 531
  IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_06 = 3;         // RXEN    B1_06 Alt3, pg 526
  IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_09 = 3;         // TXEN    B1_09 Alt3, pg 529
  IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_07 = 3;         // TXD0    B1_07 Alt3, pg 527
  IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_08 = 3;         // TXD1    B1_08 Alt3, pg 528
  IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_15 = 0;         // MDIO    B1_15 Alt0, pg 535
  IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_14 = 0;         // MDC     B1_14 Alt0, pg 534
  IOMUXC_ENET_MDIO_SELECT_INPUT = 2;            // GPIO_B1_15_ALT0, pg 792
  IOMUXC_ENET0_RXDATA_SELECT_INPUT = 1;         // GPIO_B1_04_ALT3, pg 792
  IOMUXC_ENET1_RXDATA_SELECT_INPUT = 1;         // GPIO_B1_05_ALT3, pg 793
  IOMUXC_ENET_RXEN_SELECT_INPUT = 1;            // GPIO_B1_06_ALT3, pg 794
  IOMUXC_ENET_RXERR_SELECT_INPUT = 1;           // GPIO_B1_11_ALT3, pg 795
  IOMUXC_ENET_IPG_CLK_RMII_SELECT_INPUT = 1;    // GPIO_B1_10_ALT6, pg 791
  delay(1);
  GPIO7_DR_SET = (1 << 14);  // Start PHY chip
  // ENET_MSCR = ENET_MSCR_MII_SPEED(9);
  delay(1);

  SCB_ID_CSSELR = 0;  // Disable DC cache for Ethernet DMA to work
  asm volatile("dsb" ::: "memory");  // Perhaps the alternative way
  SCB_CCR &= ~SCB_CCR_DC;            // would be to invalidate DC cache
  asm volatile("dsb" ::: "memory");  // after each IO in the driver

  // Setup IRQ handler
  attachInterruptVector(IRQ_ENET, ENET_IRQHandler);
  NVIC_ENABLE_IRQ(IRQ_ENET);
}

using namespace QP;
using namespace AC;

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
static QEvt const panelSetTransferredEvt = {PANEL_SET_TRANSFERRED_SIG, 0U, 0U};

static WDT_T4<WDT1> wdt;
static EventResponder transfer_panel_complete_event;
static uint8_t transfer_panel_complete_count;

// Serial Communication Interface
static HardwareSerial & serial_communication_interface_stream = Serial1;
static usb_serial_class & qs_serial_stream = Serial;
// usb_serial_class & serial_communication_interface_stream = Serial;
// HardwareSerial & qs_serial_stream = Serial1;

// Ethernet Communication Interface
// static const char * s_lsn = "tcp://192.168.10.62:62222";
static const char * s_lsn = "tcp://192.168.10.62:6222";

// Log
static char log_str[constants::string_log_length_max];
static uint16_t log_str_pos = 0;

//----------------------------------------------------------------------------
// Local functions
void watchdogCallback ()
{
}

// void ipAddressToString(IPAddress ip_address, char * ip_address_str)
// {
//   sprintf(ip_address_str,"%u.%u.%u.%u", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
// }

//----------------------------------------------------------------------------
// BSP functions

void BSP::init()
{
  // initialize the hardware used in this sketch...
  // NOTE: interrupts are configured and started later in QF::onStartup()

  // setup pins
  pinMode(LED_BUILTIN, OUTPUT);
  ledOff();

  for (uint8_t region_index = 0; region_index<constants::region_count_per_frame; ++region_index)
  {
    pinMode(constants::region_cipo_pins[region_index], INPUT);
    SPIClass * spi_ptr = constants::region_spi_ptrs[region_index];
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
  config.trigger = constants::watchdog_trigger_seconds;
  config.timeout = constants::watchdog_timeout_seconds;
  config.callback = watchdogCallback;
  wdt.begin(config);
}

void BSP::feedWatchdog()
{
  wdt.feed();
}

void BSP::initializeArena()
{
  pinMode(constants::reset_pin, OUTPUT);
  digitalWriteFast(constants::reset_pin, LOW);
}

void transferPanelCompleteCallback(EventResponderRef event_responder)
{
  ++transfer_panel_complete_count;
  if (transfer_panel_complete_count == constants::region_count_per_frame)
  {
    QF::PUBLISH(&panelSetTransferredEvt, &l_BSP_ID);
  }
}

void BSP::initializeFrame()
{
  transfer_panel_complete_event.attachImmediate(&transferPanelCompleteCallback);
  for (uint8_t panel_set_col_index = 0; panel_set_col_index<constants::region_col_panel_count_max; ++panel_set_col_index)
  {
    for (uint8_t panel_set_row_index = 0; panel_set_row_index<constants::region_row_panel_count_max; ++panel_set_row_index)
    {
      const uint8_t & pss_pin = constants::panel_set_select_pins[panel_set_row_index][panel_set_col_index];
      pinMode(pss_pin, OUTPUT);
      digitalWriteFast(pss_pin, HIGH);
    }
  }
}

bool BSP::beginSerial()
{
  serial_communication_interface_stream.begin(constants::serial_baud_rate);
  serial_communication_interface_stream.setTimeout(constants::serial_timeout);
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
  char command_tail[constants::string_command_length_max];
  size_t chars_read = serial_communication_interface_stream.readBytesUntil(constants::command_termination_character,
    command_tail, constants::string_command_length_max - 1);
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

void log_fn(char ch, void *param)
{
  if ((ch == '\n') || (log_str_pos == (constants::string_log_length_max - 1)))
  {
    log_str[log_str_pos] = 0;
    QS_BEGIN_ID(ETHERNET_LOG, AO_EthernetCommandInterface->m_prio)
      QS_STR(log_str);
    QS_END()
    log_str[0] = 0;
    log_str_pos = 0;
  }
  else if (ch != '\r')
  {
    log_str[log_str_pos++] = ch;
  }
}

bool BSP::initializeEthernet()
{
  mg_log_set_fn(log_fn, 0);
  ethernet_init();
  mongoose_init();
  return true;
}

void BSP::pollEthernet()
{
  mongoose_poll();
}
void sfn(struct mg_connection *c, int ev, void *ev_data)
{
  if (ev == MG_EV_OPEN && c->is_listening == 1)
  {
    MG_INFO(("SERVER is listening"));
  }
  else if (ev == MG_EV_ACCEPT)
  {
    MG_INFO(("SERVER accepted a connection"));
  }
  else if (ev == MG_EV_READ)
  {
    MG_INFO(("MG_EV_READ"));
    // struct mg_iobuf *r = &c->recv;
    // MG_INFO(("SERVER got data: %.*s", r->len, r->buf));
    // mg_send(c, r->buf, r->len);  // echo it back
    // r->len = 0;                  // Tell Mongoose we've consumed data
  }
  else if (ev == MG_EV_WRITE)
  {
    MG_INFO(("MG_EV_WRITE"));
  }
  else if (ev == MG_EV_CLOSE)
  {
    MG_INFO(("SERVER disconnected"));
  }
  else if (ev == MG_EV_ERROR)
  {
    MG_INFO(("SERVER error: %s", (char *) ev_data));
  }
  else if (ev == MG_EV_POLL)
  {
  }
  else
  {
    MG_INFO(("event %lu", ev));
  }
}

bool BSP::createEthernetServerConnection()
{
  struct mg_connection * c = mg_listen(&g_mgr, s_lsn, sfn, NULL);
  if (c == NULL)
  {
    MG_INFO(("SERVER cannot open a connection"));
    return false;
  }
  return true;
}

// void BSP::readEthernetBinaryCommand()
// {
// }

void BSP::displayFrame()
{
  delay(2);
  // QF::PUBLISH(&frameDisplayedEvt, &l_BSP_ID);
}

uint8_t BSP::getRegionRowPanelCountMax()
{
  return constants::region_row_panel_count_max;
}

uint8_t BSP::getRegionColPanelCountMax()
{
  return constants::region_col_panel_count_max;
}

void BSP::enablePanelSetSelectPin(uint8_t row_index, uint8_t col_index)
{
  for (uint8_t region_index = 0; region_index<constants::region_count_per_frame; ++region_index)
  {
    SPIClass * spi_ptr = constants::region_spi_ptrs[region_index];
    spi_ptr->beginTransaction(SPISettings(constants::spi_clock_speed, constants::spi_bit_order, constants::spi_data_mode));
  }
  const uint8_t & pss_pin = constants::panel_set_select_pins[row_index][col_index];
  digitalWriteFast(pss_pin, LOW);
  // Serial.print("setting ");
  // Serial.print(pss_pin);
  // Serial.println(" LOW");
}

void BSP::disablePanelSetSelectPin(uint8_t row_index, uint8_t col_index)
{
  const uint8_t & pss_pin = constants::panel_set_select_pins[row_index][col_index];
  digitalWriteFast(pss_pin, HIGH);
  for (uint8_t region_index = 0; region_index<constants::region_count_per_frame; ++region_index)
  {
    SPIClass * spi_ptr = constants::region_spi_ptrs[region_index];
    spi_ptr->endTransaction();
  }
  // Serial.print("setting ");
  // Serial.print(pss_pin);
  // Serial.println(" HIGH");
}

void BSP::transferPanelSet(const uint8_t (*panel_buffer)[], uint8_t panel_buffer_byte_count)
{
  transfer_panel_complete_count = 0;
  for (uint8_t region_index = 0; region_index<constants::region_count_per_frame; ++region_index)
  {
    SPIClass * spi_ptr = constants::region_spi_ptrs[region_index];
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
  Timer1.initialize(TIMER1_CLCK_HZ / constants::ticks_per_second);
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
  SCB_AIRCR = 0x05FA0004;
  while(true);
}
