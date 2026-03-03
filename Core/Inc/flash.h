#pragma once
#include <stdint.h>

#define CYCLES_MEM_ADDR 0x08060000
#define THRESHOLD_MEM_ADDR 0x08060004
#define ADC_CHANNEL_MEM_ADDR 0x08060008
#define CHECKSUM_MEM_ADDR 0x08060010

typedef enum {
  FLASH_OK = 0,
  FLASH_ERROR_UNLOCK,
  FLASH_ERROR_ERASE,
  FLASH_ERROR_PROGRAM_CYCLES,
  FLASH_ERROR_PROGRAM_THRESHOLD,
  FLASH_ERROR_PROGRAM_SENSOR,
  FLASH_ERROR_PROGRAM_CHECKSUM,
  FLASH_ERROR_VERIFY
} FlashStatus;

FlashStatus save_to_flash(void);
void read_flash(void);
uint32_t checksum(uint8_t cycles, uint16_t threshold, uint8_t channel);