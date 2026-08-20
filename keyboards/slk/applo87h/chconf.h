/* Copyright 2026 SLK
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 */

#pragma once

/* ChibiOS kernel configuration for AT32F405.
 * 100 kHz system tick gives 10us timing granularity for low-latency scanning.
 */
#define CH_CFG_ST_RESOLUTION             32
#define CH_CFG_ST_FREQUENCY              100000U
#define CH_CFG_INTERV_SEGREGATED_BUFFERS TRUE
#define CH_CFG_ST_TIMEDELTA              2

#define CH_CFG_USE_MEMCORE               TRUE
#define CH_CFG_USE_HEAP                  TRUE
#define CH_CFG_USE_FACTORY               TRUE
#define CH_CFG_FACTORY_LIMIT_OBJECTS     TRUE
#define CH_CFG_FACTORY_GENERIC_BUFFERS   TRUE
#define CH_CFG_FACTORY_SEMAPHORES        TRUE
#define CH_CFG_FACTORY_MAILBOXES         TRUE
#define CH_CFG_FACTORY_OBJ_FIFOS         TRUE
#define CH_CFG_FACTORY_OBJECTS_REGISTRY  TRUE

#define CH_CFG_USE_TM                    FALSE
#define CH_CFG_USE_MUTEXES               TRUE
#define CH_CFG_USE_CONDVARS              TRUE
#define CH_CFG_USE_EVENTS                TRUE
#define CH_CFG_USE_MESSAGES              TRUE
#define CH_CFG_USE_DYNAMIC               TRUE

#define CH_DBG_STATISTICS                FALSE
#define CH_DBG_SYSTEM_STATE_CHECK        FALSE
#define CH_DBG_ENABLE_CHECKS             FALSE
#define CH_DBG_ENABLE_ASSERTS            FALSE
#define CH_DBG_TRACE_MASK                CH_DBG_TRACE_MASK_DISABLED
#define CH_DBG_ENABLE_STACK_CHECK        FALSE

#define CH_CFG_SYSTEM_HALT_HOOK(reason)
#define CH_CFG_IRQ_PROLOGUE_HOOK()
#define CH_CFG_IRQ_EPILOGUE_HOOK()
#define CH_CFG_CONTEXT_SWITCH_HOOK(ntp, otp)
#define CH_CFG_THREAD_EXIT_HOOK(tp)
#define CH_CFG_IDLE_ENTER_HOOK()
#define CH_CFG_IDLE_LEAVE_HOOK()
#define CH_CFG_IDLE_LOOP_HOOK()
#define CH_CFG_SYSTEM_TICK_HOOK()

#include <chconf_verify.h>
