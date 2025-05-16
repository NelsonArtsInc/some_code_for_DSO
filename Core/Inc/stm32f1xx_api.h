/*
 * stm32f1xx_api.h
 *
 *  Created on: May 5, 2025
 *      Author: Maktub
 *
 * Provides clear interface of current hardware configuration.
 * All hardware issues must be resolved here.
 */

#ifndef CORE_INC_STM32F1XX_API_H_
#define CORE_INC_STM32F1XX_API_H_

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "buzzer.h"
#include "controls.h"
#include "stm32f1xx_conf.h"
#include "main.h"

#define UART_TRANSMIT_TIMEOUT_MS 1000

void initializeAPI();
void hardfaultInternalHandler();
bool writeSink(const char * data, int dataSize);
void logData(const char * data, int dataSize);
void logString(const char * data, ...);
void delayUS(uint32_t us);
void delayMS(uint32_t ms);
uint32_t getTick();

void UARTTransmit(int channelId, const char* data, int dataSize, bool* isOk);
void LED_ON(bool state);
#ifdef __cplusplus
}
#endif

#endif /* CORE_INC_STM32F1XX_API_H_ */
