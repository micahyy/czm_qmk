/* EC87 internal-flash calibration store (STM32F103 / APM32F103 compatible). */
#include "ec_flash.h"
#include "quantum.h" /* pulls in chibios/CMSIS for __disable_irq/__enable_irq */

#define FLASH_KEYR  (*(volatile uint32_t *)0x40022004u)
#define FLASH_OPTKEYR (*(volatile uint32_t *)0x40022008u)
#define FLASH_SR    (*(volatile uint32_t *)0x4002200Cu)
#define FLASH_CR    (*(volatile uint32_t *)0x40022010u)
#define FLASH_AR    (*(volatile uint32_t *)0x40022014u)

/* FLASH_CR bits */
#define CR_PG       (1u << 0)
#define CR_PER      (1u << 1)
#define CR_MER      (1u << 2)
#define CR_STRT     (1u << 6)
#define CR_LOCK     (1u << 7)
/* FLASH_SR bits */
#define SR_BSY      (1u << 0)
#define SR_PGERR    (1u << 2)
#define SR_WRPRTERR (1u << 4)
#define SR_EOP      (1u << 5)

#define EC_FLASH_KEY1  0x45670123u
#define EC_FLASH_KEY2  0xCDEF89ABu

static void flash_wait_ready(void) {
    volatile uint32_t guard = 1000000u;
    while ((FLASH_SR & SR_BSY) && --guard) { }
}

static bool flash_unlock(void) {
    if (!(FLASH_CR & CR_LOCK)) return true;
    FLASH_KEYR = EC_FLASH_KEY1;
    FLASH_KEYR = EC_FLASH_KEY2;
    if (FLASH_CR & CR_LOCK) return false;
    return true;
}

static void flash_lock(void) {
    FLASH_CR |= CR_LOCK;
}

static bool flash_sr_ok(void) {
    return !(FLASH_SR & (SR_PGERR | SR_WRPRTERR));
}

bool ec_store_erase(void) {
    __disable_irq();
    bool ok = false;
    do {
        if (!flash_unlock()) break;
        flash_wait_ready();
        FLASH_CR |= CR_PER;
        FLASH_AR = EC_STORE_PAGE_ADDR;
        FLASH_CR |= CR_STRT;
        flash_wait_ready();
        FLASH_CR &= ~CR_PER;
        ok = flash_sr_ok() && ((*(const uint16_t *)EC_STORE_PAGE_ADDR) == 0xFFFFu);
        flash_lock();
    } while (0);
    __enable_irq();
    return ok;
}

bool ec_store_write_halfwords(const uint16_t *data, uint32_t halfwords) {
    __disable_irq();
    bool ok = false;
    do {
        if (!flash_unlock()) break;
        FLASH_CR |= CR_PG;
        volatile uint16_t *dst = (volatile uint16_t *)EC_STORE_PAGE_ADDR;
        ok = true;
        for (uint32_t i = 0; i < halfwords; i++) {
            dst[i] = data[i];
            flash_wait_ready();
            if (!flash_sr_ok()) { ok = false; break; }
        }
        FLASH_CR &= ~CR_PG;
        flash_lock();
    } while (0);
    __enable_irq();
    return ok;
}
