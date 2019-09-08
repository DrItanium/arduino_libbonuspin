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
 * @tparam ST_CP the pin connected to ST_CP of 74HC595
 * @tparam SH_CP the pin connected to SH_CP of 74HC595
 * @tparam DS the pin connected to DS of 74HC595
 */
template<int ST_CP, int SH_CP, int DS>
class SN74HC595 {
    public:
        static_assert(ST_CP != DS, "The latch and data pins are defined as the same pins!");
        static_assert(ST_CP != SH_CP, "The clock and latch pins are defined as the same pins!!");
        static_assert(SH_CP != DS, "The clock and data pins are defined as the same pins!");
        using Self = SN74HC595<ST_CP, SH_CP, DS>;
        using LatchHolder = DigitalPinHolder<ST_CP, LOW, HIGH>;
    public:
        /**
         * Setup the pins associated with this device
         */
        SN74HC595() {
            setupPins();
        }

        ~SN74HC595() = default;
        constexpr decltype(ST_CP) getLatchPin() const noexcept { return ST_CP; }
        constexpr decltype(SH_CP) getClockPin() const noexcept { return SH_CP; }
        constexpr decltype(DS) getDataPin() const noexcept { return DS; }
        /**
         * Provided in case pins get reset in between init and the setup
         * function
         */
        void setupPins() {
            pinMode(ST_CP, OUTPUT);
            pinMode(SH_CP, OUTPUT);
            pinMode(DS, OUTPUT);
        }
        /**
         * Hold the latch low and shift out a single byte of data!
         */
        void shiftOut(byte value) noexcept {
            LatchHolder latch;
            ::shiftOut(DS, SH_CP, MSBFIRST, value);
        }
        /**
         * Hold the latch low and shift out two bytes of data!
         */
        void shiftOut(uint16_t value) noexcept {
            LatchHolder latch;
            ::shiftOut(DS, SH_CP, MSBFIRST, value >> 8);
            ::shiftOut(DS, SH_CP, MSBFIRST, value);
        }
        void shiftOut(uint8_t lower, uint8_t upper) noexcept {
            LatchHolder latch;
            ::shiftOut(DS, SH_CP, MSBFIRST, upper);
            ::shiftOut(DS, SH_CP, MSBFIRST, lower);

        }
        /**
         * Hold the latch low and shift out two bytes of data!
         */
        void shiftOut(int16_t value) noexcept {
            LatchHolder latch;
            ::shiftOut(DS, SH_CP, MSBFIRST, (value >> 8) & 0x00FF);
            ::shiftOut(DS, SH_CP, MSBFIRST, value);
        }
        /**
         * Hold the latch low and shift 4 bytes of data!
         */
        void shiftOut(uint32_t value) noexcept {
            LatchHolder latch;
            ::shiftOut(DS, SH_CP, MSBFIRST, (value >> 24) & 0xFF);
            ::shiftOut(DS, SH_CP, MSBFIRST, (value >> 16) & 0xFF);
            ::shiftOut(DS, SH_CP, MSBFIRST, (value >> 8) & 0xFF);
            ::shiftOut(DS, SH_CP, MSBFIRST, value & 0xFF);
        }
        /**
         * Hold the latch low and shift 4 bytes of data!
         */
        void shiftOut(int32_t value) noexcept {
            LatchHolder latch;
            ::shiftOut(DS, SH_CP, MSBFIRST, (value >> 24) & 0xFF);
            ::shiftOut(DS, SH_CP, MSBFIRST, (value >> 16) & 0xFF);
            ::shiftOut(DS, SH_CP, MSBFIRST, (value >> 8) & 0xFF);
            ::shiftOut(DS, SH_CP, MSBFIRST, value & 0xFF);
        }
        /**
         * Hold the latch low and shift 8 bytes of data!
         */
        void shiftOut(uint64_t value) noexcept {
            LatchHolder latch;
            ::shiftOut(DS, SH_CP, MSBFIRST, (value >> 56) & 0xFF);
            ::shiftOut(DS, SH_CP, MSBFIRST, (value >> 48) & 0xFF);
            ::shiftOut(DS, SH_CP, MSBFIRST, (value >> 40) & 0xFF);
            ::shiftOut(DS, SH_CP, MSBFIRST, (value >> 32) & 0xFF);
            ::shiftOut(DS, SH_CP, MSBFIRST, (value >> 24) & 0xFF);
            ::shiftOut(DS, SH_CP, MSBFIRST, (value >> 16) & 0xFF);
            ::shiftOut(DS, SH_CP, MSBFIRST, (value >> 8) & 0xFF);
            ::shiftOut(DS, SH_CP, MSBFIRST, value);
        }
        /**
         * Hold the latch low and shift 8 bytes of data!
         */
        void shiftOut(int64_t value) noexcept {
            LatchHolder latch;
            ::shiftOut(DS, SH_CP, MSBFIRST, (value >> 56) & 0xFF);
            ::shiftOut(DS, SH_CP, MSBFIRST, (value >> 48) & 0xFF);
            ::shiftOut(DS, SH_CP, MSBFIRST, (value >> 40) & 0xFF);
            ::shiftOut(DS, SH_CP, MSBFIRST, (value >> 32) & 0xFF);
            ::shiftOut(DS, SH_CP, MSBFIRST, (value >> 24) & 0xFF);
            ::shiftOut(DS, SH_CP, MSBFIRST, (value >> 16) & 0xFF);
            ::shiftOut(DS, SH_CP, MSBFIRST, (value >> 8) & 0xFF);
            ::shiftOut(DS, SH_CP, MSBFIRST, value);
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
// Le sigh... I want C++17 features...
namespace keyestudio {
namespace multipurpose_shield {
namespace v2 {
    enum class leds : int {
        D1 = 13,
        D2 = 12,
        D3 = 11,
        D4 = 10,
        D5 = 9,
        D6 = 8,
    };
    inline void setupLedArray() noexcept {
        pinMode(static_cast<int>(leds::D1), OUTPUT);
        pinMode(static_cast<int>(leds::D2), OUTPUT);
        pinMode(static_cast<int>(leds::D3), OUTPUT);
        pinMode(static_cast<int>(leds::D4), OUTPUT);
        pinMode(static_cast<int>(leds::D5), OUTPUT);
        pinMode(static_cast<int>(leds::D6), OUTPUT);
    }
    constexpr int LED4_ST_CP = 4;
    constexpr int LED4_SH_CP = 5;
    constexpr int LED4_DS = 2;
    /**
     * LED Segment displays
     */
    static constexpr byte LED4_FIELDS[4] = {
        0x01,
        0x02,
        0x04,
        0x08,
    };
    static constexpr byte LED4_BASE16_SEGMENTS[16] = {
        0xC0, 0xF9, 0xA4, 0xB0,
        0x99, 0x92, 0x82, 0xF8, 
        0x80, 0x90, 0x88, 0x83,
        0xC6, 0xA1, 0x86, 0x8E,
    };
/**
 * Makes working with SN74HC595 chips that interface with the keyestudio
 * multipurpose shield v2's 4 digit led display much much easier!
 */
class FourDigitLEDDisplay : private SN74HC595<LED4_ST_CP,LED4_SH_CP,LED4_DS> {
    private:
        using Parent = SN74HC595<LED4_ST_CP,LED4_SH_CP,LED4_DS>;
    private:
        void emitToDisplay(byte value, byte digit) {
            shiftOut(LED4_FIELDS[digit], LED4_BASE16_SEGMENTS[value]);
        }
        void emitToDisplay(byte highest, byte higher, byte lower, byte lowest) noexcept {
            emitToDisplay(highest, 0);
            emitToDisplay(higher, 1);
            emitToDisplay(lower, 2);
            emitToDisplay(lowest, 3);
        }
    public:
        FourDigitLEDDisplay() : Parent() { }
        ~FourDigitLEDDisplay() = default;
        inline void printOut(uint16_t val) {
            auto lowestQuarter = 0x000F & val;
            auto lowerQuarter = 0x000F & (val >> 4);
            auto higherQuarter = 0x000F & (val >> 8);
            auto highestQuarter = 0x000F & (val >> 12);
            emitToDisplay(highestQuarter, higherQuarter, lowerQuarter, lowestQuarter);
        }
        inline void printOut(int16_t val) {
            auto lowestQuarter = 0x000F & val;
            auto lowerQuarter = 0x000F & (val >> 4);
            auto higherQuarter = 0x000F & (val >> 8);
            auto highestQuarter = 0x000F & (val >> 12);
            emitToDisplay(highestQuarter, higherQuarter, lowerQuarter, lowestQuarter);
        }

        inline FourDigitLEDDisplay& operator<<(uint16_t value) noexcept {
            printOut(value);
            return *this;
        }
        inline FourDigitLEDDisplay& operator<<(int16_t value) noexcept {
            printOut(value);
            return *this;
        }
};
} // end namespace v2
} // end namespace multipurpose_shield
} // end namespace keyestudio
} // end bitmanip
#endif // end LIB_BONUSPIN_H__
