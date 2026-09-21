/* Minimal STM32F103/APM32F103 internal-flash store for EC87 calibration.
 *
 * The whole keyboard image (with uf2boot) lives at 0x08004000 in a 128 KB
 * flash. We reserve the final 1 KB page (0x0801FC00) for calibration data;
 * the linker scripts are trimmed so application code never lands there.
 *
 * F1 flash: 1 KB pages on medium-density devices, half-word (16-bit)
 * programming. APM32F103 keeps the same controller interface. */
#pragma once

#include <stdint.h>
#include <stdbool.h>

#define EC_STORE_PAGE_ADDR  0x0801FC00u
#define EC_STORE_PAGE_SIZE  1024u

/* VIA/keymap persistent store: eight 1KB pages immediately below the
 * calibration page. Kept out of the linker region (flash is trimmed). */
#define EC_EEPROM_PAGE_ADDR  0x0801D000u
#define EC_EEPROM_PAGE_SIZE  8192u
#define EC_EEPROM_PAGES      (EC_EEPROM_PAGE_SIZE / EC_STORE_PAGE_SIZE)

/* Erase the calibration page and program the given payload (half-word
 * aligned, page_size bytes are written — pass a page-sized padded buffer).
 * Returns false on a controller error. Disables interrupts briefly while
 * erasing (~30 ms); USB tolerates the stall. */
bool ec_store_erase(void);
bool ec_store_write_halfwords(const uint16_t *data, uint32_t halfwords);

/* Erase a single 1KB page by absolute address (used to commit only changed
 * EEPROM pages). */
bool ec_flash_erase_page(uint32_t page_addr);
/* Write halfwords starting at an absolute address. */
bool ec_flash_write_at(uint32_t addr, const uint16_t *data, uint32_t halfwords);

/* Direct read pointer into flash memory. */
static inline const uint16_t *ec_store_ptr(void) {
    return (const uint16_t *)EC_STORE_PAGE_ADDR;
}
