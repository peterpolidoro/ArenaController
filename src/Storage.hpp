// ----------------------------------------------------------------------------
// Storage.hpp
//
//
// Authors:
// Peter Polidoro peter@polidoro.io
// ----------------------------------------------------------------------------
#ifndef ARENA_CONTROLLER_STORAGE_HPP
#define ARENA_CONTROLLER_STORAGE_HPP

#include <SdFat.h>

#include "ArenaController/Constants.hpp"
#include "ArenaController/Patterns.hpp"


// 1 for FAT16/FAT32, 2 for exFAT, 3 for FAT16/FAT32 and exFAT.
#define SD_FAT_TYPE 2

// Try max SPI clock for an SD. Reduce SPI_CLOCK if errors occur.
#define SPI_CLOCK SD_SCK_MHZ(50)

// Try to select the best SD storage configuration.
#if HAS_SDIO_CLASS
#define SD_CONFIG SdioConfig(FIFO_SDIO)
#elif ENABLE_DEDICATED_SPI
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, DEDICATED_SPI, SPI_CLOCK)
#else  // HAS_SDIO_CLASS
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, SHARED_SPI, SPI_CLOCK)
#endif  // HAS_SDIO_CLASS

class ArenaController;

namespace arena_controller
{
class Storage
{
public:
  void listFiles();
  void printFileInformation();
  void printFileHeaders();
  void printFileSizes();

  void openFileForWriting();
  void openFileForReading();
  void rewindFileForReading();
  void closeFile();
  void writePanelToFile(const uint8_t * panel_buffer, size_t panel_byte_count);
  void readPanelFromFile(uint8_t * panel_buffer, size_t panel_byte_count);
private:
  SdExFat sd_;
  ExFile dir_;
  ExFile file_;
  //uint8_t file_buffer[79407U];
  char file_name_[arena_controller::constants::file_name_size_max];
  uint64_t file_position_;

  void setup();
  friend class ::ArenaController;
};
}
#endif