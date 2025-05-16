/*
 * controls.h
 *
 *  Created on: May 5, 2025
 *      Author: Maktub
 */

#ifndef CORE_INC_CONTROLS_H_
#define CORE_INC_CONTROLS_H_

#include <stdint.h>

enum ButtonState
{
	PRESSED,
	HOLDED,
	RELEASED,
	CLICKED,
	DOUBLE_CLICKED,
	TRIPPLE_CLICKED,
	MULTIPLE_CLICKED, // add series of clicks in callback if needed

	COUNT_OF_BUTTON_STATES
};

enum WheelDirection
{
	WHEEL_LEFT = 0,
	WHEEL_RIGHT
};

void initializeControls();

void handleControlsTimerInterrupt();
void handleGPIOInterrupt(uint16_t GPIO_Pin);

// called after reusing buttons pins
void forceHandleWeakControls();

// Weak callbacks
void onLeftButton(int newState);
void onMidLeftButton(int newState);
void onMidButton(int newState);
void onMidRightButton(int newState);
void onRightButton(int newState);
void onWheel(int newDirection);

#endif
