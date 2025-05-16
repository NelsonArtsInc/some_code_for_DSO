/*
 * FAStack.hpp
 *
 *  Created on: May 5, 2025
 *      Author: Maktub
 */
#pragma once

#include <memory>
#include <functional>
#include <stack>
#include "Common/NonCopyable.hpp"

namespace Core::FA {

template<typename T>
class Stack : private Common::NonCopyable
{
    const std::shared_ptr<std::stack<T>> _states = std::make_shared<std::stack<T>>();

    void popInternal()
    {
        auto currentState = _states->top();
        _states->pop();
//        currentState.deactivate();
    }

public:
    Stack() = default;

    explicit Stack(const T& defaultState)
    {
        push(defaultState);
    }

    T *operator->()
    {
        return &top();
    }

    T& top() const
    {
        if (_states->size() == 0) {
            // ASSERT
        }
        return _states->top();
    }

    void replace(const T& newState)
    {
        popInternal();
        push(newState);
    }

    void push(const T& newState)
    {
        _states->push(newState);
//        newState.activate();
        // state changed
    }

    void pop()
    {
        if (_states->empty()) {
            // ASSERT
        	return;
        }

        popInternal();
        // state changed
    }

    void reset()
    {
        while (_states->size() > 1) {
            pop();
        }
    }

    int size()
    {
        return (int)_states->size();
    }
};

}
