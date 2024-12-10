#ifndef FLASH_H_
#define FLASH_H_

#include "stdbool.h"
#include "stdint.h"

// This library provides read/write access to the flash memory.
// Please do not modify, unless you find a bug :)

// Number of bytes of flash memory available.
// Valid byte addresses range from [0, FLASH_MEMORY_SIZE)
#define FLASH_MEMORY_SIZE (1 << 20)

// Writes *length* bytes from *src* into flash memory, starting at *address*
// Returns true if successful.
bool flash_write(uint32_t address, uint8_t *src, uint32_t length);

// Reads *length* bytes from flash memory into *dst*, starting at *address*
bool flash_read(uint32_t address, uint8_t *dst, uint32_t length);

#endif  // FLASH_H_
