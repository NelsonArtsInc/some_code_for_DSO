/*
 * controls.c
 *
 *  Created on: May 5, 2025
 *      Author: Maktub
 */

#include "controls.h"

#include "main.h"
#include "stm32f1xx_api.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_conf.h"

// Количество кнопок в системе
#define COUNT_BUTTONS 5
// Время антидребезга кнопки (мс)
#define BUTTON_THROTTLE_MS 2
// Минимальное время удержания для регистрации нажатия (мс)
#define BUTTON_PRESSED_MS 10
// Время удержания для перехода в состояние "удержано" (мс)
#define BUTTON_HOLDED_MS 350
// Время для фиксации отпускания кнопки (мс)
#define BUTTON_RELEASED_MS 10
// Время для определения клика (мс)
#define BUTTON_CLICKED_MS 120

// Внутренние состояния кнопки
enum ButtonInternalState
{
    _IDLE,      // Ожидание
    _FRONT,     // Кнопка нажата (переход)
    _PRESSED,   // Кнопка нажата (подтверждено)
    _HOLDED,    // Кнопка удерживается
    _BACK,      // Кнопка отпущена (переход)
    _RELEASED,  // Кнопка отпущена (подтверждено)
    _CLICKED    // Клик (одиночный/двойной/тройной)
};

// Структура для хранения состояния аппаратной кнопки
typedef struct
{
    // interface
    GPIO_TypeDef* GPIO_Port;      // Порт GPIO кнопки
    uint16_t GPIO_Pin;            // Пин GPIO кнопки
    GPIO_PinState idlePinState;   // Состояние пина в покое
    void (*callback)(int);        // Колбэк при изменении состояния

    // private
    uint32_t timeStampMs;         // Временная метка последнего события
    uint32_t pinChangedTimeStamp; // Временная метка последнего изменения пина
    uint16_t releaseCount;        // Счетчик отпусканий (для мультикликов)
    GPIO_PinState lastPinState;   // Последнее состояние пина
    int internalState;            // Текущее внутреннее состояние
    int internalPrevState;        // Предыдущее внутреннее состояние
    bool isActive;                // Активна ли кнопка (обрабатывается ли)
} HardButton;

// Структура для хранения состояния энкодера
typedef struct
{
    // interface
    GPIO_TypeDef* GPIO_Port_Lhs;      // Левый пин энкодера
    uint16_t GPIO_Pin_Lhs;
    GPIO_PinState idleLhsPinState;
    GPIO_TypeDef* GPIO_Port_Rhs;      // Правый пин энкодера
    uint16_t GPIO_Pin_Rhs;
    GPIO_PinState idleRhsPinState;
    void (*callback)(int);            // Колбэк при вращении

    // private
    uint8_t lastEncoderValue;         // Последнее значение энкодера
} HardEncoder;

// Пара для отслеживания переходов энкодера
typedef struct
{
    uint8_t lastPos;
    uint8_t newPos;
} EncoderPair;

// Массивы для определения направления вращения энкодера
#define COUNT_ENCODER_PAIRS 4
static TIM_HandleTypeDef* _timer;
static HardButton _buttons[COUNT_BUTTONS];
static uint16_t _activeButtonsCount = 0;
static HardEncoder _encoder;
static const EncoderPair _encoderLeftPairs[COUNT_ENCODER_PAIRS] = {
    {0b00, 0b10}, {0b10, 0b11}, {0b11, 0b01}, {0b01, 0b00}
};
static const EncoderPair _encoderRightPairs[COUNT_ENCODER_PAIRS] = {
    {0b01, 0b11}, {0b11, 0b10}, {0b10, 0b00}, {0b00, 0b01}
};

// Прототипы внутренних функций
static void initializeButton(HardButton* button, GPIO_TypeDef* GPIO_Port, uint16_t GPIO_Pin, GPIO_PinState idlePinState, void (*callback)(int));
static void initializeEncoder(
    HardEncoder* encoder,
    GPIO_TypeDef* GPIO_Port_Lhs,
    uint16_t GPIO_Pin_Lhs,
    GPIO_PinState idleLhsPinState,
    GPIO_TypeDef* GPIO_Port_Rhs,
    uint16_t GPIO_Pin_Rhs,
    GPIO_PinState idleRhsPinState,
    void (*callback)(int)
);
static void handleButtonInterrupt(HardButton* button, GPIO_PinState state);
static void handleEncoderInterrupt(HardEncoder* encoder, GPIO_PinState lhsState, GPIO_PinState rhsState);
static void switchInterrupts(bool enable);
static void activateButton(HardButton* button);
static void deactivateButton(HardButton* button);
static void setButtonState(HardButton* button, int newInternalState);

// Инициализация всех кнопок и энкодера
void initializeControls()
{
    _timer = CONTROLS_INTERRUPTS_TIMER;

    initializeEncoder(&_encoder, DB0_GPIO_Port, DB0_Pin, GPIO_PIN_SET, DB1_GPIO_Port, DB1_Pin, GPIO_PIN_SET, &onWheel);
    initializeButton(&_buttons[0], DB3_GPIO_Port, DB3_Pin, GPIO_PIN_SET, &onMidButton);
    initializeButton(&_buttons[1], DB4_GPIO_Port, DB4_Pin, GPIO_PIN_SET, &onLeftButton);
    initializeButton(&_buttons[2], DB5_GPIO_Port, DB5_Pin, GPIO_PIN_SET, &onMidLeftButton);
    initializeButton(&_buttons[3], DB8_GPIO_Port, DB8_Pin, GPIO_PIN_SET, &onMidRightButton);
    initializeButton(&_buttons[4], DB9_GPIO_Port, DB9_Pin, GPIO_PIN_SET, &onRightButton);
}

// Обработка внешнего прерывания по изменению состояния GPIO (кнопки/энкодер)
void handleGPIOInterrupt(uint16_t GPIO_Pin)
{
    uint32_t currentTick = getTick();
    // Если прерывание от энкодера
    if(GPIO_Pin == _encoder.GPIO_Pin_Lhs || GPIO_Pin == _encoder.GPIO_Pin_Rhs) {
        GPIO_PinState lhsState = HAL_GPIO_ReadPin(_encoder.GPIO_Port_Lhs, _encoder.GPIO_Pin_Lhs);
        GPIO_PinState rhsState = HAL_GPIO_ReadPin(_encoder.GPIO_Port_Rhs, _encoder.GPIO_Pin_Rhs);
        handleEncoderInterrupt(&_encoder, 0b01 & (~(lhsState ^ _encoder.idleLhsPinState)), 0b01 & (~(rhsState ^ _encoder.idleRhsPinState)));
        return;
    }
    // Обработка кнопок
    for(int i = 0; i < COUNT_BUTTONS; ++i) {
        HardButton* currentButton = _buttons + i;
        if(currentButton->GPIO_Pin == GPIO_Pin) {
            // Антидребезг: проверяем, прошло ли достаточно времени с последнего изменения
            if((currentTick - currentButton->pinChangedTimeStamp) > BUTTON_THROTTLE_MS) {
                GPIO_PinState newState = HAL_GPIO_ReadPin(currentButton->GPIO_Port, currentButton->GPIO_Pin);
                if(newState != currentButton->lastPinState) {
                    currentButton->lastPinState = newState;
                    currentButton->pinChangedTimeStamp = getTick();
                    handleButtonInterrupt(currentButton, newState);
                }
            }
            return;
        }
    }
}

// Принудительная обработка всех кнопок (например, при инициализации)
void forceHandleWeakControls()
{
    for(int i = 0; i < COUNT_BUTTONS; ++i) {
        handleGPIOInterrupt(_buttons[i].GPIO_Pin);
    }
    // Encoder handle (можно добавить обработку энкодера при необходимости)
}

// Обработка таймерного прерывания для управления состояниями кнопок
void handleControlsTimerInterrupt()
{
    for(int i = 0; i < COUNT_BUTTONS; ++i) {
        HardButton* currentButton = &_buttons[i];
        uint32_t currentTick = getTick();
        bool isDeactivateButton = false;
        switch(currentButton->internalState) {
            case _IDLE: {
                isDeactivateButton = true; // Кнопка неактивна
                break;
            }
            case _FRONT: {
                // Если кнопка удерживается достаточно долго — переходим в PRESSED
                if(currentTick - currentButton->timeStampMs >= BUTTON_PRESSED_MS) {
                    setButtonState(currentButton, _PRESSED);
                    currentButton->timeStampMs = getTick();
                    currentButton->callback(PRESSED);
                }
                break;
            }
            case _PRESSED: {
                // Если кнопка удерживается еще дольше — переходим в HOLDED
                if(currentTick - currentButton->timeStampMs >= BUTTON_HOLDED_MS) {
                    setButtonState(currentButton, _HOLDED);
                    currentButton->timeStampMs = getTick();
                    currentButton->callback(HOLDED);
                }
                break;
            }
            case _HOLDED: {
                currentButton->releaseCount = 0;
                isDeactivateButton = true; // После удержания — деактивируем
                break;
            }
            case _BACK: {
                // Если был шум — возвращаемся в IDLE
                if(currentButton->internalPrevState == _FRONT) {
                    setButtonState(currentButton, _IDLE);
                    isDeactivateButton = true;
                } else if(currentTick - currentButton->timeStampMs >= BUTTON_RELEASED_MS) {
                    setButtonState(currentButton, _RELEASED);
                    currentButton->timeStampMs = getTick();
                    currentButton->releaseCount += 1;
                    currentButton->callback(RELEASED);
                }
                break;
            }
            case _RELEASED: {
                // Определяем тип клика (одиночный, двойной и т.д.)
                if(currentTick - currentButton->timeStampMs >= BUTTON_CLICKED_MS) {
                    int userState;
                    switch(currentButton->releaseCount) {
                        case 1: {
                            userState = CLICKED;
                            break;
                        }
                        case 2: {
                            userState = DOUBLE_CLICKED;
                            break;
                        }
                        case 3: {
                            userState = TRIPPLE_CLICKED;
                            break;
                        }
                        // Можно добавить больше вариантов
                        default: {
                            userState = MULTIPLE_CLICKED;
                        }
                    }
                    setButtonState(currentButton, _CLICKED);
                    currentButton->timeStampMs = getTick();
                    currentButton->releaseCount = 0;
                    currentButton->callback(userState);
                }
                break;
            }
            case _CLICKED: {
                setButtonState(currentButton, _IDLE);
                isDeactivateButton = true;
                break;
            }
            default: {
                // Неизвестное состояние
//				ASSERT(!"Unknown button state");
            }
        }

        // Деактивация кнопки, если требуется
        if(isDeactivateButton && currentButton->isActive) {
            deactivateButton(currentButton);
        }
    }
}

// Инициализация одной кнопки
static void initializeButton(HardButton* button, GPIO_TypeDef* GPIO_Port, uint16_t GPIO_Pin, GPIO_PinState idlePinState, void (*callback)(int))
{
    button->GPIO_Port = GPIO_Port;
    button->GPIO_Pin = GPIO_Pin;
    button->idlePinState = idlePinState;
    button->callback = callback;
    button->timeStampMs = 0;
    button->pinChangedTimeStamp = 0;
    button->releaseCount = 0;
    button->lastPinState = button->idlePinState;
    button->internalState = _IDLE;
    button->internalPrevState = _IDLE;
    button->isActive = false;
}

// Инициализация энкодера
static void initializeEncoder(
    HardEncoder* encoder,
    GPIO_TypeDef* GPIO_Port_Lhs,
    uint16_t GPIO_Pin_Lhs,
    GPIO_PinState idleLhsPinState,
    GPIO_TypeDef* GPIO_Port_Rhs,
    uint16_t GPIO_Pin_Rhs,
    GPIO_PinState idleRhsPinState,
    void (*callback)(int)
)
{
    encoder->GPIO_Port_Lhs = GPIO_Port_Lhs;
    encoder->GPIO_Pin_Lhs = GPIO_Pin_Lhs;
    encoder->idleLhsPinState = idleLhsPinState;
    encoder->GPIO_Port_Rhs = GPIO_Port_Rhs;
    encoder->GPIO_Pin_Rhs = GPIO_Pin_Rhs;
    encoder->idleRhsPinState = idleRhsPinState;
    encoder->callback = callback;
    encoder->lastEncoderValue = 0b00;
}

// Обработка прерывания кнопки: установка состояния и активация
static void handleButtonInterrupt(HardButton* button, GPIO_PinState newState)
{
    button->timeStampMs = getTick();
    setButtonState(button, button->idlePinState != newState ? _FRONT : _BACK);

    if(!button->isActive) {
        activateButton(button);
    }
}

// Обработка прерывания энкодера: определение направления вращения
static void handleEncoderInterrupt(HardEncoder* encoder, GPIO_PinState lhsState, GPIO_PinState rhsState)
{
    uint8_t newPos = (uint8_t)lhsState << 1 | (uint8_t)rhsState;
    uint8_t lastPos = encoder->lastEncoderValue;
    bool wasHandled = false;

    // Проверка на вращение вправо
    for(int i = 0; i < COUNT_ENCODER_PAIRS; ++i) {
        if(lastPos == _encoderRightPairs[i].lastPos && newPos == _encoderRightPairs[i].newPos) {
            wasHandled = true;
            encoder->callback(WHEEL_RIGHT);
            break;
        }
    }

    // Проверка на вращение влево
    if(!wasHandled) {
        for(int i = 0; i < COUNT_ENCODER_PAIRS; ++i) {
            if(lastPos == _encoderLeftPairs[i].lastPos && newPos == _encoderLeftPairs[i].newPos) {
                wasHandled = true;
                encoder->callback(WHEEL_LEFT);
                break;
            }
        }
    }

    encoder->lastEncoderValue = newPos;
}

// Установка нового внутреннего состояния кнопки
static void setButtonState(HardButton* button, int newInternalState)
{
    button->internalPrevState = button->internalState;
    button->internalState = newInternalState;
}

// Активация кнопки (включение таймера, если это первая активная кнопка)
static void activateButton(HardButton* button)
{
    button->isActive = true;
    if(_activeButtonsCount++ == 0) {
        switchInterrupts(true);
    }
}

// Деактивация кнопки (выключение таймера, если не осталось активных кнопок)
static void deactivateButton(HardButton* button)
{
    button->isActive = false;
    if(--_activeButtonsCount == 0) {
        switchInterrupts(false);
    }
}

// Включение/выключение таймера для обработки кнопок
static void switchInterrupts(bool enable)
{
    if(enable) {
        HAL_TIM_Base_Start_IT(_timer);
    } else {
        HAL_TIM_Base_Stop_IT(_timer);
    }
}

// Слабые обработчики событий кнопок и энкодера (можно переопределить в пользовательском коде)
__weak void onLeftButton(int newState)
{
    // Do not modify! Override this function in user file
    UNUSED(newState);
}

__weak void onMidLeftButton(int newState)
{
    // Do not modify! Override this function in user file
    UNUSED(newState);
}

__weak void onMidButton(int newState)
{
    // Do not modify! Override this function in user file
    UNUSED(newState);
}

__weak void onMidRightButton(int newState)
{
    // Do not modify! Override this function in user file
    UNUSED(newState);
}

__weak void onRightButton(int newState)
{
    // Do not modify! Override this function in user file
    UNUSED(newState);
}

__weak void onWheel(int newDirection)
{
    // Do not modify! Override this function in user file
    UNUSED(newDirection);
}
