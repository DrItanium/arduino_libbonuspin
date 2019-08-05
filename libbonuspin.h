/**
 * @file 
 * Additional classes and functions that make working with pins easier
 * @copyright
 * Copyright (c) 2019 Joshua Scoggins 
 * 
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */
#ifndef LIB_BONUSPIN_H__
#define LIB_BONUSPIN_H__
#include "Arduino.h"
namespace bonuspin
{
/**
 * RAII-style class that handles holding a digital pin to a specific value for the
 * lifetime of this object; this object stores nothing so it should not consume
 * space at runtime.
 * @tparam pin The pin to be held
 * @tparam holdPinAs the value to hold the pin to during the lifetime of the object
 * @tparam restorePinTo the value to return the pin to when this object goes out of scope.
 */
template<int pin, decltype(LOW) holdPinAs, decltype(HIGH) restorePinTo>
class DigitalPinHolder final 
{
    public:
        inline constexpr DigitalPinHolder() {
            digitalWrite(pin, holdPinAs);
        }
        inline constexpr ~DigitalPinHolder() {
            digitalWrite(pin, restorePinTo);
        }
        inline constexpr decltype(pin) getPin() const noexcept { return pin; }
        inline constexpr decltype(holdPinAs) getHeldPinValue() const noexcept { return holdPinAs; }
        inline constexpr decltype(restorePinTo) getPinValueOnDestruction() const noexcept { return restorePinTo; }
        DigitalPinHolder(const DigitalPinHolder&) = delete;
        DigitalPinHolder(DigitalPinHolder&&) = delete;
        DigitalPinHolder& operator=(const DigitalPinHolder&) = delete;
        DigitalPinHolder& operator=(DigitalPinHolder&&) = delete;
};

/**
 * Base case for shiftOutMultiple, is a wrapper around shiftOut.
 * @param dataPin the pin to send data out on
 * @param clockPin the pin that clocks the data to be sent out
 * @param order start with the most or least significant bit first
 * @param value The value to be written out
 * @tparam T the type of the value to be written out
 */
template<typename T>
inline void shiftOutMultiple(int dataPin, int clockPin, decltype(MSBFIRST) order, T value) noexcept {
    shiftOut(dataPin, clockPin, order, value);
}
/**
 * Call shiftOut for each provided argument; If you have a lot of data to clock
 * out, this function is for you. This version requires a minimum of two
 * arguments.
 */
template<typename T, typename ... Args>
void shiftOutMultiple(int dataPin, int clockPin, decltype(MSBFIRST) order, T value, Args&& ... rest) noexcept {
    shiftOut(dataPin, clockPin, order, value);
    if (sizeof...(args) > 0) {
        shiftOutMultiple(dataPin, clockPin, order, args...);
    }
}
} // end bitmanip
#endif // end LIB_BONUSPIN_H__
