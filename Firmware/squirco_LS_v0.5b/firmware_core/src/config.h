/**
 * \file config.h
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

/*- Definitions ------------------------------------------------------------*/
#define APP_SOFTVERSION         0x005b

#define HAL_UART_CHANNEL        1
#define HAL_UART_RX_FIFO_SIZE   1000
#define HAL_UART_TX_FIFO_SIZE   1000

#define PHY_ENABLE_RANDOM_NUMBER_GENERATOR

#define SYS_SECURITY_MODE                   0

#define NWK_BUFFERS_AMOUNT                  128
#define NWK_DUPLICATE_REJECTION_TABLE_SIZE  50
#define NWK_DUPLICATE_REJECTION_TTL         2000 // ms
#define NWK_ROUTE_TABLE_SIZE                10
#define NWK_ROUTE_DEFAULT_SCORE             3
#define NWK_ACK_WAIT_TIME                   1000 // ms
#define NWK_GROUPS_AMOUNT                   5
#define NWK_ROUTE_DISCOVERY_TABLE_SIZE      5
#define NWK_ROUTE_DISCOVERY_TIMEOUT         1000 // ms

#define NWK_ENABLE_ROUTING
#define NWK_ENABLE_SECURITY

#define APP_MAX_TX_RETRIES      1	// total attempts = APP_MAX_TX_RETRIES + 1
#define APP_ENDPOINT			1

#endif // _CONFIG_H_
