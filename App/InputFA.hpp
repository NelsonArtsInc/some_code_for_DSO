/*
 * InputFA.hpp
 *
 *  Created on: May 7, 2025
 *      Author: Maktub
 */

#pragma once

#include <functional>
#include <vector>
#include <array>
#include <unordered_map>
#include "FAStack.hpp"
#include "stm32f1xx_api.h"

namespace Core::FA {

enum Buttons
{
	LEFT_BUTTON,
	MID_LEFT_BUTTON,
	MID_BUTTON,
	MID_RIGHT_BUTTON,
	RIGHT_BUTTON,

	COUNT_OF_BUTTONS
};

class InputState
{
	std::array<std::vector<std::function<void()>>, COUNT_OF_BUTTONS> _buttonHandlers;
	std::function<void(int)> _encoderHandler;
	bool _isActive = true;

public:
	void activate()
	{
		_isActive = true;
	}

	void deactivate()
	{
		_isActive = false;
	}

	void setButtonHandler(Buttons button, int stateId, const std::function<void()>& handler)
	{
		auto& handlers = _buttonHandlers[button];
		if(handlers.empty()) {
			handlers.resize(COUNT_OF_BUTTON_STATES);
		}
		handlers[stateId] = handler;
	}

	void setEncoderHandler(const std::function<void(int)>& handler)
	{
		_encoderHandler = handler;
	}

	bool handleButton(Buttons button, int stateId)
	{
		if(_isActive && !_buttonHandlers[button].empty() && _buttonHandlers[button][stateId] != nullptr) {
			_buttonHandlers[button][stateId]();
			return true;
		}
		return false;
	}

	bool handleEncoder(int direction)
	{
		if(_isActive && _encoderHandler) {
			_encoderHandler(direction);
			return true;
		}
		return false;
	}
};

using InputFA = Stack<InputState>;

}
