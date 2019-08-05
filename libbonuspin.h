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
        inline DigitalPinHolder() {
            digitalWrite(pin, holdPinAs);
        }
        inline ~DigitalPinHolder() {
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
    if (sizeof...(rest) > 0) {
        shiftOutMultiple(dataPin, clockPin, order, rest...);
    }
}

/**
 * Makes working with SN74HC595 chips easier
 * @tparam latchPin the latch pin index
 * @tparam clockPin the clock pin index
 * @tparam dataPin the data pin index
 */
template<int latchPin, int clockPin, int dataPin>
class SN74HC595 {
    public:
        static_assert(latchPin != dataPin, "The latch and data pins are defined as the same pins!");
        static_assert(latchPin != clockPin, "The clock and latch pins are defined as the same pins!!");
        static_assert(clockPin != dataPin, "The clock and data pins are defined as the same pins!");
        using Self = SN74HC595<latchPin, clockPin, dataPin>;
        using LatchHolder = DigitalPinHolder<latchPin, LOW, HIGH>;
    public:
        /**
         * Setup the pins associated with this device
         */
        SN74HC595() {
            pinMode(latchPin, OUTPUT);
            pinMode(clockPin, OUTPUT);
            pinMode(dataPin, OUTPUT);
        }

        ~SN74HC595() = default;
        constexpr auto getLatchPin() const noexcept { return latchPin; }
        constexpr auto getClockPin() const noexcept { return clockPin; }
        constexpr auto getDataPin() const noexcept { return dataPin; }
        /**
         * Hold the latch low and shift out a single byte of data!
         */
        void shiftOut(byte value) noexcept {
            LatchHolder latch;
            ::shiftOut(dataPin, clockPin, MSBFIRST, value);
        }
        /**
         * Hold the latch low and shift out two bytes of data!
         */
        void shiftOut(uint16_t value) noexcept {
            LatchHolder latch;
            ::shiftOut(dataPin, clockPin, MSBFIRST, value >> 8);
            ::shiftOut(dataPin, clockPin, MSBFIRST, value);
        }
        /**
         * Hold the latch low and shift 4 bytes of data!
         */
        void shiftOut(uint32_t value) noexcept {
            LatchHolder latch;
            ::shiftOut(dataPin, clockPin, MSBFirst, value >> 24);
            ::shiftOut(dataPin, clockPin, MSBFIRST, value >> 16);
            ::shiftOut(dataPin, clockPin, MSBFIRST, value >> 8);
            ::shiftOut(dataPin, clockPin, MSBFIRST, value);
        }
        /**
         * Hold the latch low and shift 4 bytes of data!
         */
        void shiftOut(uint64_t value) noexcept {
            LatchHolder latch;
            ::shiftOut(dataPin, clockPin, MSBFirst, value >> 56);
            ::shiftOut(dataPin, clockPin, MSBFIRST, value >> 48);
            ::shiftOut(dataPin, clockPin, MSBFIRST, value >> 40);
            ::shiftOut(dataPin, clockPin, MSBFIRST, value >> 32);
            ::shiftOut(dataPin, clockPin, MSBFirst, value >> 24);
            ::shiftOut(dataPin, clockPin, MSBFIRST, value >> 16);
            ::shiftOut(dataPin, clockPin, MSBFIRST, value >> 8);
            ::shiftOut(dataPin, clockPin, MSBFIRST, value);
        }
        template<typename T, typename ... Args>
        void shiftOutMultiple(T current, Args&& ... rest) noexcept {
            shiftOut(current);
            if (sizeof...(rest) > 0) {
                shiftOutMultiple(rest...);
            }
        }
        template<typename T>
        Self& operator<<(T value) noexcept {
            shiftOut(value);
            return *this;
        }
};
} // end bitmanip
#endif // end LIB_BONUSPIN_H__
