/**
 * \file halTimer.c
 *
 * \brief ATSAMD20 timer implementation
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
 * $Id: halTimer.c 9267 2014-03-18 21:46:19Z ataradov $
 *
 */

/*- Includes ---------------------------------------------------------------*/
#include <stdint.h>
#include "hal.h"
#include "halTimer.h"

/*- Definitions ------------------------------------------------------------*/
#define TIMER_PRESCALER  8
#define TIMER_TOP        (((F_CPU / 1000ul) / TIMER_PRESCALER) * HAL_TIMER_INTERVAL)

/*- Variables --------------------------------------------------------------*/
volatile uint8_t halTimerIrqCount;

/*- Implementations --------------------------------------------------------*/

/*************************************************************************//**
*****************************************************************************/
static inline void halTimerSync(void)
{
  while (TC4->COUNT16.STATUS.bit.SYNCBUSY);
}

/*************************************************************************//**
*****************************************************************************/
void HAL_TimerInit(void)
{
  halTimerIrqCount = 0;

  PM->APBCMASK.reg |= PM_APBCMASK_TC4;

  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(TC4_GCLK_ID) |
      GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(0);

  TC4->COUNT16.CTRLA.reg = TC_CTRLA_MODE_COUNT16 | TC_CTRLA_WAVEGEN_MFRQ |
      TC_CTRLA_PRESCALER(3/*DIV8*/) | TC_CTRLA_PRESCSYNC(TC_CTRLA_PRESCSYNC_PRESC);
  halTimerSync();

  TC4->COUNT16.COUNT.reg = 0;
  halTimerSync();

  TC4->COUNT16.CC[0].reg = TIMER_TOP;
  halTimerSync();

  TC4->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE;
  halTimerSync();

  TC4->COUNT16.INTENSET.reg = TC_INTENSET_MC(1);
  NVIC_EnableIRQ(TC4_IRQn);
}

/*************************************************************************//**
*****************************************************************************/
void HAL_TimerDelay(uint16_t us)
{
  uint16_t target = TC4->COUNT16.COUNT.reg + us;

  if (target > TIMER_TOP)
    target -= TIMER_TOP;

  TC4->COUNT16.INTFLAG.reg = TC_INTENSET_MC(2);

  TC4->COUNT16.CC[1].reg = target;
  halTimerSync();

  while (0 == (TC4->COUNT16.INTFLAG.reg & TC_INTFLAG_MC(2)));
}

/*************************************************************************//**
*****************************************************************************/
void HAL_IrqHandlerTc4(void)
{
  if (TC4->COUNT16.INTFLAG.reg & TC_INTFLAG_MC(1))
  {
    halTimerIrqCount++;
    TC4->COUNT16.INTFLAG.reg = TC_INTFLAG_MC(1);
  }
}
