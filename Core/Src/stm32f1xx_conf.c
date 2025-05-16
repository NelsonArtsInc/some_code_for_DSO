/*
 * stm32f1xx_conf.c
 *
 *  Created on: May 6, 2025
 *      Author: Maktub
 */
#include "stm32f1xx_conf.h"

#include "main.h"
#include "stm32f1xx_api.h"
#include "stm32f1xx_conf.h"

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;

TIM_HandleTypeDef* CONTROLS_INTERRUPTS_TIMER = &htim1;
TIM_HandleTypeDef* BUZZER_INTERRUPTS_TIMER = &htim2;
int BUZZER_TIMER_CHANNEL_ID = TIM_CHANNEL_1;
static bool isGPIOInterruptsEnabled = true;

void hardfaultHandler()
{
	hardfaultInternalHandler();
}

void tim1Interrupt()
{
	handleControlsTimerInterrupt();
}

void tim2Interrupt()
{
	handleBuzzerTimerInterrupt();
}

void disableGPIOInterrupts()
{
	isGPIOInterruptsEnabled = false;
}

void enableGPIOInterrupts()
{
	isGPIOInterruptsEnabled = true;
}

void gpioInterrupt(uint16_t GPIO_Pin)
{
	if(isGPIOInterruptsEnabled) {
		handleGPIOInterrupt(GPIO_Pin);
	}

#ifdef GPIO_INTERRUPT_TRACE
	switch(GPIO_Pin)
	{
		case DB0_Pin: {
			bool isSet = HAL_GPIO_ReadPin(DB0_GPIO_Port, DB0_Pin);
			logString("DB0 %i", isSet);
			break;
		}
		case DB1_Pin: {
			bool isSet = HAL_GPIO_ReadPin(DB1_GPIO_Port, DB1_Pin);
			logString("DB1 %i", isSet);
			break;
		}
		case DB2_Pin:
			logString("DB2");
			break;
		case DB3_Pin:
			logString("DB3");
			break;
		case DB4_Pin:
			logString("DB4");
			break;
		case DB5_Pin:
			logString("DB5");
			break;
		case DB6_Pin:
			logString("DB6");
			break;
		case DB7_Pin:
			logString("DB7");
			break;
		case DB8_Pin:
			logString("DB8");
			break;
		case DB9_Pin:
			logString("DB9");
			break;
		default: {
			logString("Unknown interrupt");
		}
	}
#endif
}
