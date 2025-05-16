/*
 * controls.h
 *
 *  Created on: May 5, 2025
 *      Author: Maktub
 */

#ifndef CORE_INC_CONTROLS_H_
#define CORE_INC_CONTROLS_H_

#include <stdint.h>

// Перечисление возможных состояний кнопки
enum ButtonState
{
    PRESSED,           // Кнопка нажата
    HOLDED,            // Кнопка удерживается
    RELEASED,          // Кнопка отпущена
    CLICKED,           // Одиночный клик
    DOUBLE_CLICKED,    // Двойной клик
    TRIPPLE_CLICKED,   // Тройной клик
    MULTIPLE_CLICKED,  // Множественный клик (4 и более)

    COUNT_OF_BUTTON_STATES // Количество состояний (служебное)
};

// Перечисление направлений вращения энкодера
enum WheelDirection
{
    WHEEL_LEFT = 0,    // Вращение влево
    WHEEL_RIGHT        // Вращение вправо
};

// Инициализация всех кнопок и энкодера
void initializeControls();

// Обработка таймерного прерывания для управления состояниями кнопок
void handleControlsTimerInterrupt();

// Обработка внешнего прерывания по изменению состояния GPIO (кнопки/энкодер)
void handleGPIOInterrupt(uint16_t GPIO_Pin);

// Принудительная обработка всех кнопок (например, после повторного использования пинов)
void forceHandleWeakControls();

// Слабые (weak) колбэки для обработки событий кнопок и энкодера
void onLeftButton(int newState);
void onMidLeftButton(int newState);
void onMidButton(int newState);
void onMidRightButton(int newState);
void onRightButton(int newState);
void onWheel(int newDirection);

#endif
