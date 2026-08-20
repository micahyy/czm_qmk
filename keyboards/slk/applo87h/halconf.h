/* Copyright 2026 SLK
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

/*
 * ChibiOS HAL configuration for AT32F405 (SLK Applo87H).
 *
 * USB OTG2 (OTGHS with on-chip High-Speed PHY) is enabled in
 * boards/GENERIC_AT32_F405XX/configs/mcuconf.h, not here.
 * PWM is disabled — no WS2812 on this board revision.
 */

#define HAL_USE_PAL                 TRUE
#define HAL_USE_ADC                 FALSE
#define HAL_USE_CAN                 FALSE
#define HAL_USE_DAC                 FALSE
#define HAL_USE_GPT                 FALSE
#define HAL_USE_I2C                 FALSE
#define HAL_USE_I2S                 FALSE
#define HAL_USE_ICU                 FALSE
#define HAL_USE_MAC                 FALSE
#define HAL_USE_MMC_SPI             FALSE
#define HAL_USE_PWM                 FALSE
#define HAL_USE_RTC                 FALSE
#define HAL_USE_SDC                 FALSE
#define HAL_USE_SERIAL              FALSE
#define HAL_USE_SERIAL_USB          FALSE
#define HAL_USE_SPI                 FALSE
#define HAL_USE_UART                FALSE
#define HAL_USE_USB                 TRUE
