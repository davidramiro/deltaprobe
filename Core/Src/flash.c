#include "flash.h"

#include "display.h"
#include "main.h"

uint32_t checksum(const uint8_t cycles, const uint16_t threshold,
                  const uint8_t channel) {
  uint32_t sum = 0xDEADBEEFu;

  sum ^= cycles;
  sum ^= (threshold << 8);
  sum ^= (channel << 24);

  return sum;
}

/**
 *  @brief Reads configuration data from flash memory and validates it.
 */
void readFlash(void) {
  uint8_t cycles_read = *(__IO uint32_t *)CYCLES_MEM_ADDR;
  uint16_t threshold_read = *(__IO uint32_t *)THRESHOLD_MEM_ADDR;
  uint32_t checksum_read = *(__IO uint32_t *)CHECKSUM_MEM_ADDR;
  uint8_t adc_channel_read = *(__IO uint32_t *)ADC_CHANNEL_MEM_ADDR;

  if (checksum(cycles_read, threshold_read, adc_channel_read) !=
      checksum_read) {
    // checksum mismatched, values in flash outdated or never written.
    saveToFlash();
  } else {
    num_cycles = cycles_read;
    sensor_threshold = threshold_read;
    adc_channel = adc_channel_read;
  }
}

/**
 * @brief Saves the current configuration values to the internal flash memory.
 * @return FlashStatus Status of the save operation.
 */
FlashStatus saveToFlash(void) {
  uint8_t cycles_read = *(__IO uint32_t *)CYCLES_MEM_ADDR;
  uint16_t threshold_read = *(__IO uint32_t *)THRESHOLD_MEM_ADDR;
  uint32_t checksum_read = *(__IO uint32_t *)CHECKSUM_MEM_ADDR;
  uint8_t channel_read = *(__IO uint32_t *)ADC_CHANNEL_MEM_ADDR;

  // No need to write if data unchanged
  if (cycles_read == num_cycles && threshold_read == sensor_threshold &&
      channel_read == adc_channel &&
      checksum_read == checksum(cycles_read, threshold_read, channel_read)) {
    return FLASH_OK;
  }

  HAL_GPIO_WritePin(INF_LED_GPIO_Port, INF_LED_Pin, GPIO_PIN_SET);

  drawFlashScreen(1);

  HAL_StatusTypeDef status;

  status = HAL_FLASH_Unlock();
  if (status != HAL_OK) {
    HAL_GPIO_WritePin(INF_LED_GPIO_Port, INF_LED_Pin, GPIO_PIN_RESET);
    return FLASH_ERROR_UNLOCK;
  }

  drawFlashScreen(2);

  FLASH_EraseInitTypeDef eraseInit;
  eraseInit.TypeErase = FLASH_TYPEERASE_SECTORS;
  eraseInit.Sector = 7;
  eraseInit.NbSectors = 1;
  eraseInit.VoltageRange = FLASH_VOLTAGE_RANGE_3;

  uint32_t sectorError = 0;
  status = HAL_FLASHEx_Erase(&eraseInit, &sectorError);
  if (status != HAL_OK) {
    HAL_FLASH_Lock();
    HAL_GPIO_WritePin(INF_LED_GPIO_Port, INF_LED_Pin, GPIO_PIN_RESET);
    return FLASH_ERROR_ERASE;
  }

  drawFlashScreen(3);

  status =
      HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, CYCLES_MEM_ADDR, num_cycles);
  if (status != HAL_OK) {
    HAL_FLASH_Lock();
    HAL_GPIO_WritePin(INF_LED_GPIO_Port, INF_LED_Pin, GPIO_PIN_RESET);
    return FLASH_ERROR_PROGRAM_CYCLES;
  }

  drawFlashScreen(4);

  status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, THRESHOLD_MEM_ADDR,
                             sensor_threshold);
  if (status != HAL_OK) {
    HAL_FLASH_Lock();
    HAL_GPIO_WritePin(INF_LED_GPIO_Port, INF_LED_Pin, GPIO_PIN_RESET);
    return FLASH_ERROR_PROGRAM_THRESHOLD;
  }

  drawFlashScreen(5);

  status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, ADC_CHANNEL_MEM_ADDR,
                             adc_channel);
  if (status != HAL_OK) {
    HAL_FLASH_Lock();
    return FLASH_ERROR_PROGRAM_SENSOR;
  }

  drawFlashScreen(6);

  status =
      HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, CHECKSUM_MEM_ADDR,
                        checksum(num_cycles, sensor_threshold, adc_channel));
  if (status != HAL_OK) {
    HAL_FLASH_Lock();
    return FLASH_ERROR_PROGRAM_CHECKSUM;
  }

  HAL_FLASH_Lock();

  drawFlashScreen(7);

  cycles_read = *(__IO uint32_t *)CYCLES_MEM_ADDR;
  threshold_read = *(__IO uint32_t *)THRESHOLD_MEM_ADDR;
  channel_read = *(__IO uint32_t *)ADC_CHANNEL_MEM_ADDR;
  checksum_read = *(__IO uint32_t *)CHECKSUM_MEM_ADDR;

  if (cycles_read != num_cycles || threshold_read != sensor_threshold ||
      channel_read != adc_channel ||
      checksum_read != checksum(num_cycles, sensor_threshold, channel_read)) {
    return FLASH_ERROR_VERIFY;
  }

  HAL_GPIO_WritePin(INF_LED_GPIO_Port, INF_LED_Pin, GPIO_PIN_RESET);

  return FLASH_OK;
}

/**
 * @brief Packs the 8-bit and 16-bit values into a single 32-bit integer.
 * @param val8 The 8-bit value to be placed in the lower bits.
 * @param val16 The 16-bit value to be placed in the upper bits.
 * @return The resulting 32-bit packed integer
 */
uint32_t packedChecksum(const uint8_t val8, const uint16_t val16) {
  return val16 << 8 | val8;
}
