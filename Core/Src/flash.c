#include "flash.h"

#include "display.h"
#include "main.h"

uint32_t checksum(const uint8_t cycles, const uint16_t threshold, const uint8_t channel) {
    uint32_t sum = 0xDEADBEEFu;  // Magic seed (not blank flash)

    sum ^= cycles;
    sum ^= threshold;
    sum ^= channel;

    return sum;
}

void readFlash(void) {
    uint8_t cycles_read = *(__IO uint32_t *)CYCLES_MEM_ADDR;
    uint16_t threshold_read = *(__IO uint32_t *)THRESHOLD_MEM_ADDR;
    uint32_t checksum_read = *(__IO uint32_t *)CHECKSUM_MEM_ADDR;
    uint8_t adc_channel_read = *(__IO uint32_t *)ADC_CHANNEL_MEM_ADDR;

    if (checksum(cycles_read, threshold_read, adc_channel_read) != checksum_read) {
        // Checksum mismatch - write defaults to flash
        // Ignore return value on initialization - defaults are already set
        saveToFlash();
    } else {
        num_cycles = cycles_read;
        sensor_threshold = threshold_read;
        adc_channel = adc_channel_read;
    }
}

FlashStatus saveToFlash(void) {
    uint8_t cycles_read = *(__IO uint32_t *)CYCLES_MEM_ADDR;
    uint16_t threshold_read = *(__IO uint32_t *)THRESHOLD_MEM_ADDR;
    uint32_t checksum_read = *(__IO uint32_t *)CHECKSUM_MEM_ADDR;
    uint8_t channel_read = *(__IO uint32_t *)ADC_CHANNEL_MEM_ADDR;

    // No need to write if data unchanged
    if (cycles_read == num_cycles && threshold_read == sensor_threshold && channel_read == adc_channel &&
        checksum_read == checksum(cycles_read, threshold_read, channel_read))
    {
        return FLASH_OK;
    }

    drawFlashScreen(1);

    HAL_StatusTypeDef status;

    status = HAL_FLASH_Unlock();
    if (status != HAL_OK) {
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
        return FLASH_ERROR_ERASE;
    }

    drawFlashScreen(3);

    status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, CYCLES_MEM_ADDR, num_cycles);
    if (status != HAL_OK) {
        HAL_FLASH_Lock();
        return FLASH_ERROR_PROGRAM_CYCLES;
    }

    drawFlashScreen(4);

    status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, THRESHOLD_MEM_ADDR, sensor_threshold);
    if (status != HAL_OK) {
        HAL_FLASH_Lock();
        return FLASH_ERROR_PROGRAM_THRESHOLD;
    }

    drawFlashScreen(5);

    status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, ADC_CHANNEL_MEM_ADDR, adc_channel);
    if (status != HAL_OK) {
        HAL_FLASH_Lock();
        return FLASH_ERROR_PROGRAM_SENSOR;
    }

    status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, CHECKSUM_MEM_ADDR, checksum(num_cycles, sensor_threshold, adc_channel));
    if (status != HAL_OK) {
        HAL_FLASH_Lock();
        return FLASH_ERROR_PROGRAM_CHECKSUM;
    }

    HAL_FLASH_Lock();

    drawFlashScreen(6);

    cycles_read = *(__IO uint32_t *)CYCLES_MEM_ADDR;
    threshold_read = *(__IO uint32_t *)THRESHOLD_MEM_ADDR;
    channel_read = *(__IO uint32_t *)ADC_CHANNEL_MEM_ADDR;
    checksum_read = *(__IO uint32_t *)CHECKSUM_MEM_ADDR;

    if (cycles_read != num_cycles || threshold_read != sensor_threshold ||
        channel_read != adc_channel ||
        checksum_read != checksum(num_cycles, sensor_threshold, channel_read)) {
        return FLASH_ERROR_VERIFY;
    }

    return FLASH_OK;
}
