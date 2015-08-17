/**
 * \file at86rf230.h
 *
 * \brief AT86RF230 registers description and interface
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
 * $Id: at86rf230.h 9267 2014-03-18 21:46:19Z ataradov $
 *
 */

#ifndef _AT86RF230_H_
#define _AT86RF230_H_

/*- Definitions ------------------------------------------------------------*/
#define AES_BLOCK_SIZE                 16
#define ED_UPDATE_INTERVAL             140 // us


/*- Types ------------------------------------------------------------------*/
#define TRX_STATUS_REG    0x01
#define TRX_STATE_REG     0x02
#define TRX_CTRL_0_REG    0x03
#define PHY_TX_PWR_REG    0x05
#define PHY_RSSI_REG      0x06
#define PHY_ED_LEVEL_REG  0x07
#define PHY_CC_CCA_REG    0x08
#define CCA_THRES_REG     0x09
#define IRQ_MASK_REG      0x0e
#define IRQ_STATUS_REG    0x0f
#define VREG_CTRL_REG     0x10
#define BATMON_REG        0x11
#define XOSC_CTRL_REG     0x12
#define PLL_CF_REG        0x1a
#define PLL_DCU_REG       0x1b
#define PART_NUM_REG      0x1c
#define VERSION_NUM_REG   0x1d
#define MAN_ID_0_REG      0x1e
#define MAN_ID_1_REG      0x1f
#define SHORT_ADDR_0_REG  0x20
#define SHORT_ADDR_1_REG  0x21
#define PAN_ID_0_REG      0x22
#define PAN_ID_1_REG      0x23
#define IEEE_ADDR_0_REG   0x24
#define IEEE_ADDR_1_REG   0x25
#define IEEE_ADDR_2_REG   0x26
#define IEEE_ADDR_3_REG   0x27
#define IEEE_ADDR_4_REG   0x28
#define IEEE_ADDR_5_REG   0x29
#define IEEE_ADDR_6_REG   0x2a
#define IEEE_ADDR_7_REG   0x2b
#define XAH_CTRL_REG      0x2c
#define CSMA_SEED_0_REG   0x2d
#define CSMA_SEED_1_REG   0x2e

// TRX_STATUS
#define CCA_DONE          7
#define CCA_STATUS        6
#define TRX_STATUS        0

// TRX_STATE
#define TRAC_STATUS       5
#define TRX_CMD           0

// TRX_CTRL_0
#define PAD_IO            6
#define PAD_IO_CLKM       4
#define CLKM_SHA_SEL      3
#define CLKM_CTRL         0

// PHY_TX_PWR
#define TX_AUTO_CRC_ON    7
#define TX_PWR            0

// PHY_RSSI
#define RX_CRC_VALID      7
#define RND_VALUE         5
#define RSSI              0

// PHY_CC_CCA
#define CCA_REQUEST       7
#define CCA_MODE          5
#define CHANNEL           0

// CCA_THRES
#define CCA_ED_THRES      0

// IRQ_MASK, IRQ_STATUS
#define BAT_LOW           7
#define TRX_UR            6
#define TRX_END           3
#define RX_START          2
#define PLL_UNLOCK        1
#define PLL_LOCK          0

// VREG_CTRL
#define AVREG_EXT         7
#define AVDD_OK           6
#define DVREG_EXT         3
#define DVDD_OK           2

// BATMON
#define BATMON_OK         5
#define BATMON_HR         4
#define BATMON_VTH        0

// XOSC_CTRL
#define XTAL_MODE         4
#define XTAL_TRIM         0

// PLL_CF
#define PLL_CF_START      7

// PLL_DCU
#define PLL_DCU_START     7

// XAH_CTRL
#define MAX_FRAME_RETRES  4
#define MAX_CSMA_RETRES   1

// CSMA_SEED_1
#define MIN_BE            6
#define AACK_SET_PD       5
#define AACK_I_AM_COORD   3
#define CSMA_SEED_1       0

#define RF_CMD_REG_W      ((1<<7) | (1<<6))
#define RF_CMD_REG_R      ((1<<7) | (0<<6))
#define RF_CMD_FRAME_W    ((0<<7) | (1<<6) | (1<<5))
#define RF_CMD_FRAME_R    ((0<<7) | (0<<6) | (1<<5))
#define RF_CMD_SRAM_W     ((0<<7) | (1<<6) | (0<<5))
#define RF_CMD_SRAM_R     ((0<<7) | (0<<6) | (0<<5))

#define TRX_CMD_NOP           0
#define TRX_CMD_TX_START      2
#define TRX_CMD_FORCE_TRX_OFF 3
#define TRX_CMD_FORCE_PLL_ON  4
#define TRX_CMD_RX_ON         6
#define TRX_CMD_TRX_OFF       8
#define TRX_CMD_PLL_ON        9
#define TRX_CMD_RX_AACK_ON    22
#define TRX_CMD_TX_ARET_ON    25

#define TRX_STATUS_P_ON               0
#define TRX_STATUS_BUSY_RX            1
#define TRX_STATUS_BUSY_TX            2
#define TRX_STATUS_RX_ON              6
#define TRX_STATUS_TRX_OFF            8
#define TRX_STATUS_PLL_ON             9
#define TRX_STATUS_SLEEP              15
#define TRX_STATUS_BUSY_RX_AACK       17
#define TRX_STATUS_BUSY_TX_ARET       18
#define TRX_STATUS_RX_AACK_ON         22
#define TRX_STATUS_TX_ARET_ON         25
#define TRX_STATUS_RX_ON_NOCLK        28
#define TRX_STATUS_RX_AACK_ON_NOCLK   29
#define TRX_STATUS_BUSY_RX_AACK_NOCLK 30
#define TRX_STATUS_STATE_TRANSITION   31
#define TRX_STATUS_MASK               0x1f

#define TRAC_STATUS_SUCCESS                0
#define TRAC_STATUS_SUCCESS_DATA_PENDING   1
#define TRAC_STATUS_SUCCESS_WAIT_FOR_ACK   2
#define TRAC_STATUS_CHANNEL_ACCESS_FAILURE 3
#define TRAC_STATUS_NO_ACK                 5
#define TRAC_STATUS_INVALID                7

#endif // _AT86RF230_H_
