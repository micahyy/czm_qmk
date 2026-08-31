/* Copyright 2024 CZMAO
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

/* WS2812 uses the bitbang driver on this target; no additional HAL
 * drivers (PWM/DMA timers) need to be enabled. The board configs/mcuconf.h
 * (from the AT32F403A ChibiOS-Contrib port) provides all defaults. */

#include_next <mcuconf.h>
