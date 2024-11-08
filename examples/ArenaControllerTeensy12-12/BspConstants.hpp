// ----------------------------------------------------------------------------
// BspConstants.hpp
//
//
// Authors:
// Peter Polidoro peter@polidoro.io
// ----------------------------------------------------------------------------
#ifndef ARENA_CONTROLLER_BSP_CONSTANTS_HPP
#define ARENA_CONTROLLER_BSP_CONSTANTS_HPP


#if defined(__IMXRT1062__) && defined(ARDUINO_TEENSY41)
namespace AC
{
namespace constants
{
// Ethernet Communication Interface
const IPAddress ip(192, 168, 10, 196);
const uint32_t port = 62222;

// Serial Communication Interface
//HardwareSerial & serial_communication_interface_stream = Serial;
usb_serial_class & serial_communication_interface_stream = Serial;
const uint32_t SERIAL_COMMUNICATION_INTERFACE_BAUD_RATE = 115200;
const uint16_t SERIAL_COMMUNICATION_INTERFACE_TIMEOUT = 100;

// SPI Settings
constexpr uint32_t spi_clock_speed = 5000000;

constexpr uint8_t reset_pin = 34;

// frame
constexpr uint8_t panel_count_max_per_frame_row = 5;
constexpr uint8_t panel_count_max_per_frame_col = 12;
constexpr uint8_t panel_count_max_per_frame = \
  panel_count_max_per_frame_row * panel_count_max_per_frame_col; // 60
constexpr uint16_t byte_count_max_per_frame_grayscale = \
  panel_count_max_per_frame * \
  byte_count_per_panel_grayscale; // 7920

// region
constexpr uint8_t region_count_per_frame = 2;
constexpr SPIClass * region_spi_ptrs[region_count_per_frame] = {&SPI, &SPI1};

constexpr uint8_t panel_count_max_per_region_row = panel_count_max_per_frame_row;
constexpr uint8_t panel_count_max_per_region_col = \
  panel_count_max_per_frame_col/region_count_per_frame; // 6

constexpr uint8_t panel_select_pins[panel_count_max_per_region_row][panel_count_max_per_region_col] =
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
constexpr uint16_t frame_count_max_y = 1;
constexpr uint16_t frame_count_max_x = 20;

}
}
#endif
#endif
