/**
 * \file halPhy.c
 *
 * \brief ATSAMD21 PHY interface implementation
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
 * $Id: halPhy.c 9267 2014-03-18 21:46:19Z ataradov $
 *
 */

/*- Includes ---------------------------------------------------------------*/
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "hal.h"
#include "halPhy.h"
#include "phy.h"

/*- Implementations --------------------------------------------------------*/

/*************************************************************************//**
*****************************************************************************/
uint8_t HAL_PhySpiWriteByte(uint8_t value)
{
  return HAL_PhySpiWriteByteInline(value);
}

/*************************************************************************//**
*****************************************************************************/
void HAL_PhyReset(void)
{
  HAL_GPIO_PHY_RST_clr();
  HAL_Delay(10);
  HAL_GPIO_PHY_RST_set();
}

/*************************************************************************//**
*****************************************************************************/
static void halPhySpiSync(void)
{
  while (SERCOM4->SPI.SYNCBUSY.reg);
}

/*************************************************************************//**
*****************************************************************************/
void halPhyInit(void)
{
  // Configure IO pins
  HAL_GPIO_PHY_SLP_TR_out();
  HAL_GPIO_PHY_SLP_TR_clr();
  HAL_GPIO_PHY_RST_out();
  HAL_GPIO_PHY_IRQ_in();
  HAL_GPIO_PHY_IRQ_pmuxen();
  HAL_GPIO_PHY_CS_out();
  HAL_GPIO_PHY_MISO_in();
  HAL_GPIO_PHY_MISO_pmuxen();
  HAL_GPIO_PHY_MOSI_out();
  HAL_GPIO_PHY_MOSI_pmuxen();
  HAL_GPIO_PHY_SCK_out();
  HAL_GPIO_PHY_SCK_pmuxen();

  // Configure SPI
  PORT->Group[HAL_GPIO_PORTC].PMUX[9].bit.PMUXE = PORT_PMUX_PMUXE_F_Val;  // SCK
  PORT->Group[HAL_GPIO_PORTC].PMUX[9].bit.PMUXO = PORT_PMUX_PMUXO_F_Val;  // MISO
  PORT->Group[HAL_GPIO_PORTB].PMUX[15].bit.PMUXE = PORT_PMUX_PMUXE_F_Val; // MOSI

  PM->APBCMASK.reg |= PM_APBCMASK_SERCOM4;

  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(SERCOM4_GCLK_ID_CORE) |
      GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(0);

  SERCOM4->SPI.CTRLB.reg = SERCOM_SPI_CTRLB_RXEN;
  halPhySpiSync();

#if F_CPU <= 16000000
  SERCOM4->SPI.BAUD.reg = 0;
#elif F_CPU <= 32000000
  SERCOM4->SPI.BAUD.reg = 1;
#elif F_CPU <= 48000000
  SERCOM4->SPI.BAUD.reg = 2;
#else
  #error Unsupported frequency
#endif

  SERCOM4->SPI.CTRLA.reg = SERCOM_SPI_CTRLA_ENABLE |
      SERCOM_SPI_CTRLA_MODE(SERCOM_SPI_CTRLA_MODE_SPI_MASTER_Val) |
      SERCOM_SPI_CTRLA_DIPO(0) | SERCOM_SPI_CTRLA_DOPO(1);
  halPhySpiSync();
}
