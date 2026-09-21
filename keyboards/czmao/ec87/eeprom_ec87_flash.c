/* EC87 flash-backed EEPROM driver.
 *
 * Behaves like the transient driver for the running firmware (all VIA / QMK
 * state lives in a RAM shadow), but commits the shadow to the top 8 KB of
 * internal flash so keymaps and macros survive an unplug/reboot.
 *
 * Why a custom driver instead of QMK wear_leveling: the F1 embedded-flash
 * erase/write stalls broke the capacitive-matrix ADC timing. Here we only ever
 * touch flash from the main loop ~1.2 s after the last write, erase/program
 * only the 1 KB pages that actually changed, and reuse the proven ec_flash
 * primitives (interrupts are masked for the same brief window the
 * calibration save already uses). */

#include <stdint.h>
#include <string.h>

#include "eeprom_driver.h"
#include "timer.h"
#include "ec_flash.h"

#ifndef EEPROM_SIZE
#    define EEPROM_SIZE EC_EEPROM_PAGE_SIZE
#endif

#define FLUSH_IDLE_MS 1200u

__attribute__((aligned(4))) static uint8_t cache[EEPROM_SIZE];

static bool     dirty      = false;
static uint32_t last_write = 0;

static void load_from_flash(void) {
    memcpy(cache, (const void *)EC_EEPROM_PAGE_ADDR, EEPROM_SIZE);
}

static bool commit(void) {
    const uint8_t *flash = (const uint8_t *)EC_EEPROM_PAGE_ADDR;
    bool ok = true;

    for (uint32_t pg = 0; pg < EC_EEPROM_PAGES && ok; pg++) {
        uint32_t off = pg * EC_STORE_PAGE_SIZE;
        if (memcmp(&cache[off], &flash[off], EC_STORE_PAGE_SIZE) == 0) {
            continue;
        }
        uint32_t addr = EC_EEPROM_PAGE_ADDR + off;
        if (!ec_flash_erase_page(addr)) {
            ok = false;
            break;
        }
        if (!ec_flash_write_at(addr, (const uint16_t *)&cache[off],
                               EC_STORE_PAGE_SIZE / 2)) {
            ok = false;
            break;
        }
    }

    if (ok) {
        dirty = false;
    }
    return ok;
}

static size_t clamp_length(intptr_t offset, size_t len) {
    if (offset < 0 || (uint32_t)offset >= EEPROM_SIZE) {
        return 0;
    }
    if ((uint32_t)offset + len > EEPROM_SIZE) {
        len = EEPROM_SIZE - offset;
    }
    return len;
}

void eeprom_driver_init(void) {
    load_from_flash();
    dirty      = false;
    last_write = 0;
}

void eeprom_driver_format(bool erase) {
    if (erase) {
        eeprom_driver_erase();
    }
}

void eeprom_driver_erase(void) {
    memset(cache, 0x00, EEPROM_SIZE);
    dirty      = true;
    last_write = timer_read32();
}

void eeprom_read_block(void *buf, const void *addr, size_t len) {
    intptr_t offset = (intptr_t)addr;
    memset(buf, 0x00, len);
    len = clamp_length(offset, len);
    if (len > 0) {
        memcpy(buf, &cache[offset], len);
    }
}

void eeprom_write_block(const void *buf, void *addr, size_t len) {
    intptr_t offset = (intptr_t)addr;
    len             = clamp_length(offset, len);
    if (len > 0 && memcmp(&cache[offset], buf, len) != 0) {
        memcpy(&cache[offset], buf, len);
        dirty      = true;
        last_write = timer_read32();
    }
}

void ec87_eeprom_task(void) {
    if (dirty && timer_elapsed32(last_write) >= FLUSH_IDLE_MS) {
        commit();
    }
}

bool ec87_eeprom_flush(void) {
    if (!dirty) {
        return true;
    }
    return commit();
}
