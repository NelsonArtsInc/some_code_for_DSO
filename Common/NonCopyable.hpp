/*
 * NonCopyable.hpp
 *
 *  Created on: May 7, 2025
 *      Author: Maktub
 */

#pragma once

namespace Common {

struct NonCopyable
{
    NonCopyable(const NonCopyable&) = delete;
    const NonCopyable& operator=(const NonCopyable&) = delete;

protected:
    NonCopyable() = default;
};


}
