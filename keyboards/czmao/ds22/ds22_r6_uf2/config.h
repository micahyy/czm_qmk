// Copyright 2024 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#define DYNAMIC_KEYMAP_MACRO_COUNT 32

#define WS2812_PWM_DRIVER               PWMD1
#define WS2812_PWM_CHANNEL              1
#define WS2812_PWM_COMPLEMENTARY_OUTPUT
#define WS2812_PWM_DMA_STREAM           STM32_DMA1_STREAM5
#define WS2812_PWM_DMA_CHANNEL          5
