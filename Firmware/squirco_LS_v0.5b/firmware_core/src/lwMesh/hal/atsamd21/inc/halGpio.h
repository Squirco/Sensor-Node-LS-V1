/**
 * \file halGpio.h
 *
 * \brief ATSAMD21 GPIO interface
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
 * $Id: halGpio.h 9267 2014-03-18 21:46:19Z ataradov $
 *
 */

#ifndef _HAL_GPIO_H_
#define _HAL_GPIO_H_

/*- Includes ---------------------------------------------------------------*/
#include "sysTypes.h"

/*- Definitions ------------------------------------------------------------*/
#define HAL_GPIO_PORTA 0
#define HAL_GPIO_PORTB 1
#define HAL_GPIO_PORTC 2

#define HAL_GPIO_PIN(name, port, bit) \
  INLINE void HAL_GPIO_##name##_set(void) \
  { \
    PORT->Group[HAL_GPIO_PORT##port].OUTSET.reg = (1 << bit); \
  } \
  INLINE void HAL_GPIO_##name##_clr(void) \
  { \
    PORT->Group[HAL_GPIO_PORT##port].OUTCLR.reg = (1 << bit); \
  } \
  INLINE void HAL_GPIO_##name##_toggle(void) \
  { \
    PORT->Group[HAL_GPIO_PORT##port].OUTTGL.reg = (1 << bit); \
  } \
  INLINE void HAL_GPIO_##name##_in(void) \
  { \
    PORT->Group[HAL_GPIO_PORT##port].DIRCLR.reg = (1 << bit); \
    PORT->Group[HAL_GPIO_PORT##port].PINCFG[bit].reg |= PORT_PINCFG_INEN; \
    PORT->Group[HAL_GPIO_PORT##port].PINCFG[bit].reg &= ~PORT_PINCFG_PULLEN; \
  } \
  INLINE void HAL_GPIO_##name##_out(void) \
  { \
    PORT->Group[HAL_GPIO_PORT##port].DIRSET.reg = (1 << bit); \
    PORT->Group[HAL_GPIO_PORT##port].PINCFG[bit].reg |= PORT_PINCFG_INEN; \
  } \
  INLINE void HAL_GPIO_##name##_pullup(void) \
  { \
    PORT->Group[HAL_GPIO_PORT##port].OUTSET.reg = (1 << bit); \
    PORT->Group[HAL_GPIO_PORT##port].PINCFG[bit].reg |= PORT_PINCFG_PULLEN; \
  } \
  INLINE uint8_t HAL_GPIO_##name##_read(void) \
  { \
   return (PORT->Group[HAL_GPIO_PORT##port].IN.reg & (1 << bit)) != 0; \
  } \
  INLINE uint8_t HAL_GPIO_##name##_state(void) \
  { \
    return (PORT->Group[HAL_GPIO_PORT##port].DIR.reg & (1 << bit)) != 0; \
  } \
  INLINE void HAL_GPIO_##name##_pmuxen(void) \
  { \
    PORT->Group[HAL_GPIO_PORT##port].PINCFG[bit].reg |= PORT_PINCFG_PMUXEN; \
  } \
  INLINE void HAL_GPIO_##name##_pmuxdis(void) \
  { \
    PORT->Group[HAL_GPIO_PORT##port].PINCFG[bit].reg &= ~PORT_PINCFG_PMUXEN; \
  }

#endif // _HAL_GPIO_H_
