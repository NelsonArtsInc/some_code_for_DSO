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

#define COUNT_BUTTONS 5
#define BUTTON_THROTTLE_MS 2
#define BUTTON_PRESSED_MS 10
#define BUTTON_HOLDED_MS 350
#define BUTTON_RELEASED_MS 10
#define BUTTON_CLICKED_MS 120

enum ButtonInternalState
{
	_IDLE,
	_FRONT,
	_PRESSED,
	_HOLDED,
	_BACK,
	_RELEASED,
	_CLICKED
};

typedef struct
{
	// interface
	GPIO_TypeDef* GPIO_Port;
	uint16_t GPIO_Pin;
	GPIO_PinState idlePinState;
	void (*callback)(int);

	// private
	uint32_t timeStampMs;
	uint32_t pinChangedTimeStamp;
	uint16_t releaseCount;
	GPIO_PinState lastPinState;
	int internalState;
	int internalPrevState;
	bool isActive;
} HardButton;

typedef struct
{
	// interface
	GPIO_TypeDef* GPIO_Port_Lhs;
	uint16_t GPIO_Pin_Lhs;
	GPIO_PinState idleLhsPinState;
	GPIO_TypeDef* GPIO_Port_Rhs;
	uint16_t GPIO_Pin_Rhs;
	GPIO_PinState idleRhsPinState;
	void (*callback)(int);

	// private
	uint8_t lastEncoderValue;
} HardEncoder;

typedef struct
{
	uint8_t lastPos;
	uint8_t newPos;
} EncoderPair;

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

void handleGPIOInterrupt(uint16_t GPIO_Pin)
{
	uint32_t currentTick = getTick();
	if(GPIO_Pin == _encoder.GPIO_Pin_Lhs || GPIO_Pin == _encoder.GPIO_Pin_Rhs) {
		GPIO_PinState lhsState = HAL_GPIO_ReadPin(_encoder.GPIO_Port_Lhs, _encoder.GPIO_Pin_Lhs);
		GPIO_PinState rhsState = HAL_GPIO_ReadPin(_encoder.GPIO_Port_Rhs, _encoder.GPIO_Pin_Rhs);
		handleEncoderInterrupt(&_encoder, 0b01 & (~(lhsState ^ _encoder.idleLhsPinState)), 0b01 & (~(rhsState ^ _encoder.idleRhsPinState)));
		return;
	}
	for(int i = 0; i < COUNT_BUTTONS; ++i) {
		HardButton* currentButton = _buttons + i;
		if(currentButton->GPIO_Pin == GPIO_Pin) {
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

void forceHandleWeakControls()
{
	for(int i = 0; i < COUNT_BUTTONS; ++i) {
		handleGPIOInterrupt(_buttons[i].GPIO_Pin);
	}

	// Encoder handle
}

void handleControlsTimerInterrupt()
{
	for(int i = 0; i < COUNT_BUTTONS; ++i) {
		HardButton* currentButton = &_buttons[i];
		uint32_t currentTick = getTick();
		bool isDeactivateButton = false;
		switch(currentButton->internalState) {
			case _IDLE: {
				isDeactivateButton = true; // should never happens
				break;
			}
			case _FRONT: {
				if(currentTick - currentButton->timeStampMs >= BUTTON_PRESSED_MS) {
					setButtonState(currentButton, _PRESSED);
					currentButton->timeStampMs = getTick();
					currentButton->callback(PRESSED);
				}
				break;
			}
			case _PRESSED: {
				if(currentTick - currentButton->timeStampMs >= BUTTON_HOLDED_MS) {
					setButtonState(currentButton, _HOLDED);
					currentButton->timeStampMs = getTick();
					currentButton->callback(HOLDED);
				}
				break;
			}
			case _HOLDED: {
				currentButton->releaseCount = 0;
				isDeactivateButton = true;
				break;
			}
			case _BACK: {
				if(currentButton->internalPrevState == _FRONT) { // noise
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
						// Add more series if needed
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
//				ASSERT(!"Unknown button state");
			}
		}

		if(isDeactivateButton && currentButton->isActive) {
			deactivateButton(currentButton);
		}
	}
}

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

static void handleButtonInterrupt(HardButton* button, GPIO_PinState newState)
{
	button->timeStampMs = getTick();
	setButtonState(button, button->idlePinState != newState ? _FRONT : _BACK);

	if(!button->isActive) {
		activateButton(button);
	}
}

static void handleEncoderInterrupt(HardEncoder* encoder, GPIO_PinState lhsState, GPIO_PinState rhsState)
{
	uint8_t newPos = (uint8_t)lhsState << 1 | (uint8_t)rhsState;
	uint8_t lastPos = encoder->lastEncoderValue;
	bool wasHandled = false;

	for(int i = 0; i < COUNT_ENCODER_PAIRS; ++i) {
		if(lastPos == _encoderRightPairs[i].lastPos && newPos == _encoderRightPairs[i].newPos) {
			wasHandled = true;
			encoder->callback(WHEEL_RIGHT);
			break;
		}
	}

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

static void setButtonState(HardButton* button, int newInternalState)
{
	button->internalPrevState = button->internalState;
	button->internalState = newInternalState;
}

static void activateButton(HardButton* button)
{
	button->isActive = true;
	if(_activeButtonsCount++ == 0) {
		switchInterrupts(true);
	}
}

static void deactivateButton(HardButton* button)
{
	button->isActive = false;
	if(--_activeButtonsCount == 0) {
		switchInterrupts(false);
	}
}

static void switchInterrupts(bool enable)
{
	if(enable) {
		HAL_TIM_Base_Start_IT(_timer);
	} else {
		HAL_TIM_Base_Stop_IT(_timer);
	}
}

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
