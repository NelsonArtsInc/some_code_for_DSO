/*
 * buzzer.h
 *
 *  Created on: May 5, 2025
 *      Author: Maktub
 */

#ifndef CORE_INC_BUZZER_H_
#define CORE_INC_BUZZER_H_

#include <stdbool.h>
#include <stdint.h>

void initializeBuzzer();
void switchBuzzer(bool enable);
void setBuzzerParameters(uint32_t freq, uint32_t volume, int32_t durationMs);
void playSound(uint32_t freq, uint32_t volume, int32_t durationMs);
void playSoundDeferred(uint32_t delay, uint32_t freq, uint32_t volume, int32_t durationMs);
void handleBuzzerTimerInterrupt();

// weak callbacks
void onBuzzerStop();

#endif
