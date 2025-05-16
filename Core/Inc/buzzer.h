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

// Инициализация аппаратного буззера (таймера и канала)
void initializeBuzzer();

// Включение или выключение буззера (PWM)
void switchBuzzer(bool enable);

// Установка параметров буззера: частота (Hz), громкость (0-100), длительность (мс)
void setBuzzerParameters(uint32_t freq, uint32_t volume, int32_t durationMs);

// Воспроизведение звука с заданными параметрами (частота, громкость, длительность)
void playSound(uint32_t freq, uint32_t volume, int32_t durationMs);

// Отложенное воспроизведение звука через delay миллисекунд
void playSoundDeferred(uint32_t delay, uint32_t freq, uint32_t volume, int32_t durationMs);

// Обработка таймерного прерывания для управления буззером
void handleBuzzerTimerInterrupt();

// Слабый (weak) колбэк, вызывается при остановке буззера (может быть переопределён пользователем)
void onBuzzerStop();

#endif
