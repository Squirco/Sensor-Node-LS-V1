/**
 * \file sysTypes.h
 *
 * \brief System types and definitions
 *
 * Copyright (C) 2012-2014, Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 * Modification and other use of this code is subject to Atmel's Limited
 * License Agreement (license.txt).
 *
 * $Id: sysTypes.h 9267 2014-03-18 21:46:19Z ataradov $
 *
 */

#ifndef _SYS_TYPES_H_
#define _SYS_TYPES_H_

#include <stdint.h>

#define SYS_LW_MESH_ENV

#if defined(__ICCAVR__)
  #include <inavr.h>
  #include <ioavr.h>
  #include <intrinsics.h>
  #include <pgmspace.h>

  #define SYS_MCU_ARCH_AVR

  #define PACK

  #define PRAGMA(x) _Pragma(#x)

  #define INLINE PRAGMA(inline=forced) static

  #define SYS_EnableInterrupts() __enable_interrupt()

  #define wdt_reset() (__watchdog_reset())

  #define wdt_enable(timeout) do { \
     uint8_t __atomic = SREG; __disable_interrupt(); \
    __watchdog_reset(); \
    WDTCSR |= (1 << WDCE) | (1 << WDE); \
    WDTCSR = (1 << WDE) | timeout; \
    SREG = __atomic; \
  } while (0)

  #define wdt_disable() do { \
    MCUSR = 0; \
    WDTCSR |= (1 << WDCE) | (1 << WDE); \
    WDTCSR = 0x00; \
  } while (0)

  #define ISR(vec) PRAGMA(vector=vec) __interrupt void handler_##vec(void)

  #define ATOMIC_SECTION_ENTER   { uint8_t __atomic = SREG; __disable_interrupt();
  #define ATOMIC_SECTION_LEAVE   SREG = __atomic; }
/*
#elif defined(__ICCARM__)
  #error Unsupported compiler

#elif defined(__ICCAVR32__)
  #error Unsupported compiler
*/
#else
  #if defined(HAL_ATSAMD20J18)
    #define DONT_USE_CMSIS_INIT
    #include "samd20j18.h"

    #define SYS_MCU_ARCH_CORTEX_M

  #elif defined(HAL_ATSAMD21J18)
    #define DONT_USE_CMSIS_INIT
    #include "samd21j18a.h"

    #define SYS_MCU_ARCH_CORTEX_M

  #else // All AVRs
    #include <avr/io.h>
    #include <avr/wdt.h>
    #include <avr/interrupt.h>
    #include <avr/pgmspace.h>

    #define SYS_MCU_ARCH_AVR
  #endif

  #if defined(SYS_MCU_ARCH_CORTEX_M)
    #define PRAGMA(x)

    #define PACK __attribute__ ((packed))

    #define INLINE static inline __attribute__ ((always_inline))

    #define SYS_EnableInterrupts() __asm volatile ("cpsie i");

    #define ATOMIC_SECTION_ENTER   { register uint32_t __atomic; \
                                     __asm volatile ("mrs %0, primask" : "=r" (__atomic) ); \
                                     __asm volatile ("cpsid i");
    #define ATOMIC_SECTION_LEAVE   __asm volatile ("msr primask, %0" : : "r" (__atomic) ); }

  #elif defined(SYS_MCU_ARCH_AVR)
    #define PRAGMA(x)

    #define PACK __attribute__ ((packed))

    #define INLINE static inline __attribute__ ((always_inline))

    #define SYS_EnableInterrupts() sei()

    #define ATOMIC_SECTION_ENTER   { uint8_t __atomic = SREG; cli();
    #define ATOMIC_SECTION_LEAVE   SREG = __atomic; }

  #endif

#endif

#if defined(HAL_ATMEGA1281)
  #define SYS_DEVICE_SIZE        131072
  #define SYS_PAGE_SIZE          256
  #define SYS_BOOTLOADER_SIZE    2048

#elif defined(HAL_ATMEGA128RFA1)
  #define SYS_DEVICE_SIZE        131072
  #define SYS_PAGE_SIZE          256
  #define SYS_BOOTLOADER_SIZE    2048

#elif defined(HAL_ATMEGA256RFR2)
#elif defined(HAL_ATXMEGA128B1)
#elif defined(HAL_ATXMEGA256A3U)
#elif defined(HAL_ATSAMD20J18)
#elif defined(HAL_ATSAMD21J18)
#else
  #error Unknown HAL
#endif

#endif // _SYS_TYPES_H_
