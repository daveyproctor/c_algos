#include "flash.h"

#include "stdint.h"

// In the real application, this is a separate device connected via SPI, MMC or
// something like that.
// For the purposes of this assignment,  we "fake" it with a 1 MByte array.
static uint8_t flash_memory[FLASH_MEMORY_SIZE];

bool flash_write(uint32_t address, uint8_t *src, uint32_t length) {
  for (uint32_t idx = 0; idx < length; ++idx) {
    flash_memory[address + idx] = src[idx];
  }
  return true;
}

bool flash_read(uint32_t address, uint8_t *dst, uint32_t length) {
  for (uint32_t idx = 0; idx < length; ++idx) {
    dst[idx] = flash_memory[address + idx];
  }
  return true;
}
