/*
 * stm32f1xx_conf.h
 *
 *  Created on: May 6, 2025
 *      Author: Maktub
 */

#ifndef CORE_INC_STM32F1XX_CONF_H_
#define CORE_INC_STM32F1XX_CONF_H_

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"

#include <stdint.h>
#include "stm32f1xx_hal_conf.h"
#include "stm32f1xx_hal_tim.h"

extern TIM_HandleTypeDef* CONTROLS_INTERRUPTS_TIMER;
extern TIM_HandleTypeDef* BUZZER_INTERRUPTS_TIMER;
extern int BUZZER_TIMER_CHANNEL_ID;

void tim1Interrupt();
void tim2Interrupt();
void gpioInterrupt(uint16_t GPIO_Pin);
void hardfaultHandler();

// When display initializing gpio interrupts should be ignored
void disableGPIOInterrupts();
void enableGPIOInterrupts();

#ifdef DEBUG
#define ASSERT(expr) assert_param(expr)
#else
#define ASSERT(expr) ((void)0U)
#endif

#pragma GCC diagnostic pop

#endif /* CORE_INC_STM32F1XX_CONF_H_ */
