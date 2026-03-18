// TinyUSB MSC callbacks

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "tusb.h"

// Invoked when received SCSI_CMD_INQUIRY, v2 with full inquiry response
// Some inquiry_resp's fields are already filled with default values, application can update them
// Return length of inquiry response, typically sizeof(scsi_inquiry_resp_t) (36 bytes), can be longer if included vendor data.
uint32_t tud_msc_inquiry2_cb(uint8_t lun, scsi_inquiry_resp_t *inquiry_resp, uint32_t bufsize) {
  (void) lun;
  (void) bufsize;
  const char vid[] = "TinyUSB";
  const char pid[] = "Mass Storage";
  const char rev[] = "1.0";

  (void) strncpy((char*) inquiry_resp->vendor_id, vid, 8);
  (void) strncpy((char*) inquiry_resp->product_id, pid, 16);
  (void) strncpy((char*) inquiry_resp->product_rev, rev, 4);

  return sizeof(scsi_inquiry_resp_t); // 36 bytes
}

// Invoked when received Test Unit Ready command.
// return true allowing host to read/write this LUN e.g SD card inserted
bool tud_msc_test_unit_ready_cb(uint8_t lun) {
  (void) lun;

  // RAM disk is ready until ejected
  if (ejected) {
    // Additional Sense 3A-00 is NOT_FOUND
    return tud_msc_set_sense(lun, SCSI_SENSE_NOT_READY, 0x3a, 0x00);
  }

  return true;
}

// Invoked when received SCSI_CMD_READ_CAPACITY_10 and SCSI_CMD_READ_FORMAT_CAPACITY to determine the disk size
// Application update block count and block size
void tud_msc_capacity_cb(uint8_t lun, uint32_t *block_count, uint16_t *block_size) {
  (void) lun;
  *block_count = DISK_BLOCK_NUM;
  *block_size = DISK_BLOCK_SIZE;
}

// Invoked when received Start Stop Unit command
// - Start = 0 : stopped power mode, if load_eject = 1 : unload disk storage
// - Start = 1 : active mode, if load_eject = 1 : load disk storage
bool tud_msc_start_stop_cb(uint8_t lun, uint8_t power_condition, bool start, bool load_eject) {
  (void) lun;
  (void) power_condition;

  if (load_eject) {
    if (start) {
      // load disk storage
    } else {
      // unload disk storage
      ejected = true;
    }
  }

  return true;
}

// Callback invoked when received READ10 command.
// Copy disk's data to buffer (up to bufsize) and return number of copied bytes.
int32_t tud_msc_read10_cb(uint8_t lun, uint32_t lba, uint32_t offset, void *buffer, uint32_t bufsize) {
  (void) lun;

  // out of ramdisk
  if (lba >= DISK_BLOCK_NUM) {
    return -1;
  }

  // Check for overflow of offset + bufsize
  if (lba * DISK_BLOCK_SIZE + offset + bufsize > DISK_BLOCK_NUM * DISK_BLOCK_SIZE) {
    return -1;
  }

  uint8_t const *addr = msc_disk[lba] + offset;
  (void) memcpy(buffer, addr, bufsize);

  return (int32_t) bufsize;
}

bool tud_msc_is_writable_cb(uint8_t lun) {
  (void) lun;

  #ifdef CFG_EXAMPLE_MSC_READONLY
  return false;
  #else
  return true;
  #endif
}

// Callback invoked when received WRITE10 command.
// Process data in buffer to disk's storage and return number of written bytes
int32_t tud_msc_write10_cb(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t *buffer, uint32_t bufsize) {
  (void) lun;

  // out of ramdisk
  if (lba >= DISK_BLOCK_NUM) {
    return -1;
  }

  #ifndef CFG_EXAMPLE_MSC_READONLY
  uint8_t *addr = msc_disk[lba] + offset;
  (void) memcpy(addr, buffer, bufsize);
  #else
  (void) lba;
  (void) offset;
  (void) buffer;
  #endif

  return (int32_t) bufsize;
}

int32_t tud_msc_scsi_cb(uint8_t lun, uint8_t const scsi_cmd[16], void *buffer, uint16_t bufsize) {
    (void) lun;
    (void) scsi_cmd;
    (void) buffer;
    (void) bufsize;

    // currently no other commands are supported

    // Set Sense = Invalid Command Operation
    (void) tud_msc_set_sense(lun, SCSI_SENSE_ILLEGAL_REQUEST, 0x20, 0x00);

    return -1; // stall/failed command request;
}
