#include "buzzer.h"

#include <stdbool.h>
#include "stm32f1xx_hal.h"
#include "stm32f1xx_conf.h"

#define MAX_FREQUENCY 2000000
#define MAX_FREQUENCY_OF_TIMER 100000000
#define MAX_VOLUME 100

static int32_t _originDuration = -1; // -1 means endless duration
static int32_t _durationCounter = -1;
static TIM_HandleTypeDef* _timer;
static int _timerChannelId;

static uint32_t _deferredSoundMs = -1;
static uint32_t _deferredFreq = 0;
static uint32_t _deferredVolume = 0;
static int32_t _deferredDurationMs = 0;

static void switchInterrupts(bool enable)
{
	if(enable) {
		HAL_TIM_Base_Start_IT(_timer);
	} else {
		HAL_TIM_Base_Stop_IT(_timer);
	}
}

static void switchPWM(bool enable)
{
	if(enable) {
		HAL_TIM_PWM_Start(_timer, _timerChannelId);
	} else {
		HAL_TIM_PWM_Stop(_timer, _timerChannelId);
	}
}

static void setFreqAndVolume(uint32_t freq, uint32_t volume)
{
	if(freq == 0) {
		return;
	}

	if(freq > MAX_FREQUENCY) {
		freq = MAX_FREQUENCY;
	}

	if(volume > MAX_VOLUME) {
		volume = MAX_VOLUME;
	}

	uint32_t ARRPeriod = MAX_FREQUENCY_OF_TIMER / freq;

	TIM_OC_InitTypeDef sConfigOC = {0};
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = ARRPeriod * volume / (2 * 100);
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;

	_timer->Init.Period = ARRPeriod;
	HAL_TIM_Base_Init(_timer);
	HAL_TIM_PWM_ConfigChannel(_timer, &sConfigOC, _timerChannelId);
}

static bool decreaseCounter(uint32_t* counterMs)
{
	if(*counterMs == -1) {
		return false;
	} else if(*counterMs == 0) {
		*counterMs = -1;
		return true;
	} else if( *counterMs >= _timer->Init.Period) {
		*counterMs = *counterMs - _timer->Init.Period;
		return false;
	} else {
		*counterMs = 0;
		return true;
	}
}

void initializeBuzzer()
{
	_timer = BUZZER_INTERRUPTS_TIMER;
	_timerChannelId = BUZZER_TIMER_CHANNEL_ID;

	_timer->Init.Period = 100000; // by default
	HAL_TIM_Base_Init(_timer);
}

void switchBuzzer(bool enable)
{
	if(enable) {
		if(_originDuration > 0) {
			_durationCounter = _originDuration;
			switchInterrupts(true);
		}
		switchPWM(true);
	} else {
		switchPWM(false);
	}
}

void setBuzzerParameters(uint32_t freq, uint32_t volume, int32_t durationMs)
{
	_originDuration = durationMs * 1000;
	setFreqAndVolume(freq, volume);
}

void handleBuzzerTimerInterrupt()
{
	if(decreaseCounter(&_deferredSoundMs)) {
		playSound(_deferredFreq, _deferredVolume, _deferredDurationMs);
		if(_originDuration == -1) {
			switchInterrupts(false);
		}
		_deferredSoundMs = -1;
		_deferredFreq = 0;
		_deferredVolume = 0;
		_deferredDurationMs = 0;
		return;
	}

	if(decreaseCounter(&_durationCounter)) {
		switchPWM(false);
		if(_deferredSoundMs == -1) {
			switchInterrupts(false);
		}
		onBuzzerStop();
	}
}

void playSound(uint32_t freq, uint32_t volume, int32_t durationMs)
{
	setBuzzerParameters(freq, volume, durationMs);
	switchBuzzer(true);
}

void playSoundDeferred(uint32_t delay, uint32_t freq, uint32_t volume, int32_t durationMs)
{
	if(delay == 0) {
		playSound(freq, volume, durationMs);
		return;
	}
	_deferredSoundMs = delay;
	_deferredFreq = freq;
	_deferredVolume = volume;
	_deferredDurationMs = durationMs;

	switchInterrupts(true);
}

__weak void onBuzzerStop()
{

}
