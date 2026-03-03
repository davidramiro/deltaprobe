#include "flash.h"

#include "display.h"
#include "main.h"

/**
 *  @brief Computes a checksum based on the provided parameters.
 *
 *  This function calculates a 32-bit checksum using the input cycle count,
 *  threshold value, and ADC channel index. XOR operations are performed
 *  on the parameters with predefined shifts to generate the checksum.
 *
 *  @param cycles The number of cycles to include in the checksum calculation.
 *  @param threshold The threshold value to include in the checksum calculation.
 *  @param channel The ADC channel identifier to include in the checksum calculation.
 *  @return The computed 32-bit checksum.
 */
uint32_t checksum(const uint8_t cycles, const uint16_t threshold,
                  const uint8_t channel) {
  uint32_t sum = 0xDEADBEEFu;

  sum ^= cycles;
  sum ^= threshold << 8;
  sum ^= channel << 24;

  return sum;
}

/**
 *  @brief Reads and validates configuration data from flash memory.
 *
 *  This function retrieves stored configuration data including the number
 *  of cycles, threshold value, ADC channel identifier, and a checksum from
 *  designated flash memory addresses. The checksum is calculated based on
 *  the retrieved data and compared with the stored checksum. If the two checksums
 *  do not match, the flash memory is updated with default or new data. Otherwise,
 *  the retrieved data is loaded into the corresponding global variables.
 */
void read_flash(void) {
  const uint8_t cycles_read = *(__IO uint32_t *)CYCLES_MEM_ADDR;
  const uint16_t threshold_read = *(__IO uint32_t *)THRESHOLD_MEM_ADDR;
  const uint32_t checksum_read = *(__IO uint32_t *)CHECKSUM_MEM_ADDR;
  const uint8_t adc_channel_read = *(__IO uint32_t *)ADC_CHANNEL_MEM_ADDR;

  if (checksum(cycles_read, threshold_read, adc_channel_read) !=
      checksum_read) {
    // checksum mismatched, values in flash outdated or never written.
    save_to_flash();
  } else {
    num_cycles = cycles_read;
    sensor_threshold = threshold_read;
    adc_channel = adc_channel_read;
  }
}

/**
 *  @brief Saves provided configuration data to flash memory.
 *
 *  This function writes key parameters, including cycle count,
 *  threshold value, ADC channel, and their checksum, to predefined
 *  flash memory addresses. Before writing, it verifies if the data
 *  in memory matches the current parameters to avoid unnecessary writes.
 *  The function performs multiple stages, including unlocking the
 *  flash memory, erasing the target sector, and writing the data with
 *  validation. Errors during any phase are reported via specific
 *  return codes.
 *
 *  @return FLASH_OK on successful write operation and verification.
 *  @return FLASH_ERROR_UNLOCK if unlocking flash memory fails.
 *  @return FLASH_ERROR_ERASE if sector erasure fails.
 *  @return FLASH_ERROR_PROGRAM_CYCLES if writing the cycle count fails.
 *  @return FLASH_ERROR_PROGRAM_THRESHOLD if writing the threshold fails.
 *  @return FLASH_ERROR_PROGRAM_SENSOR if writing the ADC channel fails.
 *  @return FLASH_ERROR_PROGRAM_CHECKSUM if writing the checksum fails.
 *  @return FLASH_ERROR_VERIFY if verification of the written data fails.
 */
FlashStatus save_to_flash(void) {
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

  render_flash_screen(1);

  HAL_StatusTypeDef status = HAL_FLASH_Unlock();
  if (status != HAL_OK) {
    HAL_GPIO_WritePin(INF_LED_GPIO_Port, INF_LED_Pin, GPIO_PIN_RESET);
    return FLASH_ERROR_UNLOCK;
  }

  render_flash_screen(2);

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

  render_flash_screen(3);

  status =
      HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, CYCLES_MEM_ADDR, num_cycles);
  if (status != HAL_OK) {
    HAL_FLASH_Lock();
    HAL_GPIO_WritePin(INF_LED_GPIO_Port, INF_LED_Pin, GPIO_PIN_RESET);
    return FLASH_ERROR_PROGRAM_CYCLES;
  }

  render_flash_screen(4);

  status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, THRESHOLD_MEM_ADDR,
                             sensor_threshold);
  if (status != HAL_OK) {
    HAL_FLASH_Lock();
    HAL_GPIO_WritePin(INF_LED_GPIO_Port, INF_LED_Pin, GPIO_PIN_RESET);
    return FLASH_ERROR_PROGRAM_THRESHOLD;
  }

  render_flash_screen(5);

  status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, ADC_CHANNEL_MEM_ADDR,
                             adc_channel);
  if (status != HAL_OK) {
    HAL_FLASH_Lock();
    return FLASH_ERROR_PROGRAM_SENSOR;
  }

  render_flash_screen(6);

  status =
      HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, CHECKSUM_MEM_ADDR,
                        checksum(num_cycles, sensor_threshold, adc_channel));
  if (status != HAL_OK) {
    HAL_FLASH_Lock();
    return FLASH_ERROR_PROGRAM_CHECKSUM;
  }

  HAL_FLASH_Lock();

  render_flash_screen(7);

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