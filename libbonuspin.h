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

template<int pin>
using HoldPinLow = DigitalPinHolder<pin, LOW, HIGH>;
template<int pin>
using HoldPinHigh = DigitalPinHolder<pin, HIGH, LOW>;

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
 * @todo add support for the OE line to be controlled if desired
 */
template<int ST_CP, int SH_CP, int DS>
class SN74HC595 {
    public:
        static_assert(ST_CP != DS, "The latch and data pins are defined as the same pins!");
        static_assert(ST_CP != SH_CP, "The clock and latch pins are defined as the same pins!!");
        static_assert(SH_CP != DS, "The clock and data pins are defined as the same pins!");
        using Self = SN74HC595<ST_CP, SH_CP, DS>;
        using LatchHolder = HoldPinLow<ST_CP>;
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

template<int selA, int selB, int selC, int enablePin>
class HC138 {
    public:
        static_assert(selA != selB, "SelA and SelB are the same!");
        static_assert(selA != selC, "SelA and SelC are the same!");
        static_assert(selA != enablePin, "SelA and enablePin are the same!");
        static_assert(selB != selC, "SelB and SelC are the same!");
        static_assert(selB != enablePin, "SelB and enablePin are the same!");
        static_assert(selC != enablePin, "SelC and enablePin are the same!");
        using DigitalPinSignal = decltype(HIGH);
        using TemporaryDisabler = HoldPinLow<enablePin>;
        using TemporaryEnabler = HoldPinHigh<enablePin>;
        using Self = HC138<selA, selB, selC, enablePin>;
    public:
        HC138() {
            setupPins();
        }
        constexpr decltype(selA) getSelAPin() const noexcept { return selA; }
        constexpr decltype(selB) getSelBPin() const noexcept { return selB; }
        constexpr decltype(selC) getSelCPin() const noexcept { return selC; }
        constexpr decltype(enablePin) getEnablePin() const noexcept { return enablePin; }
        void setupPins() {
            pinMode(selA, OUTPUT);
            pinMode(selB, OUTPUT);
            pinMode(selC, OUTPUT);
            pinMode(enablePin, OUTPUT);

            digitalWrite(selA, HIGH);
            digitalWrite(selB, HIGH);
            digitalWrite(selB, HIGH);
            digitalWrite(enablePin, LOW); // turn off the connection to the chip for the 
            // time being

        }
        template<DigitalPinSignal a, DigitalPinSignal b, DigitalPinSignal c>
        void generateSignal() {
            // turn off the chip while we send our signal and then reactivate
            // it once we have the pins setup for the right stuff
            TemporaryDisabler disabler;
            digitalWrite(selA, a);
            digitalWrite(selB, b);
            digitalWrite(selC, c);
        }
        template<byte signal>
        void enableLine() {
            switch (signal) {
                case 0:
                    generateSignal<LOW, LOW, LOW>();
                    break;
                case 1:
                    generateSignal<HIGH, LOW, LOW>();
                    break;
                case 2:
                    generateSignal<LOW, HIGH, LOW>();
                    break;
                case 3:
                    generateSignal<HIGH, HIGH, LOW>();
                    break;
                case 4:
                    generateSignal<LOW, LOW, HIGH>();
                    break;
                case 5: 
                    generateSignal<HIGH, LOW, HIGH>();
                    break;
                case 6:
                    generateSignal<LOW, HIGH, HIGH>();
                    break;
                case 7:
                    generateSignal<HIGH, HIGH, HIGH>();
                    break;
                default:
                    enableLine<signal & 0x7>();
                    break;
            }
        }
        /**
         * Non compile time deduced version of enableLine.
         * @param signal the line to enable
         */
        void enableLine(byte signal) {
            switch (signal) {
                case 0:
                    generateSignal<LOW, LOW, LOW>();
                    break;
                case 1:
                    generateSignal<HIGH, LOW, LOW>();
                    break;
                case 2:
                    generateSignal<LOW, HIGH, LOW>();
                    break;
                case 3:
                    generateSignal<HIGH, HIGH, LOW>();
                    break;
                case 4:
                    generateSignal<LOW, LOW, HIGH>();
                    break;
                case 5: 
                    generateSignal<HIGH, LOW, HIGH>();
                    break;
                case 6:
                    generateSignal<LOW, HIGH, HIGH>();
                    break;
                case 7:
                    generateSignal<HIGH, HIGH, HIGH>();
                    break;
                default:
                    enableLine(signal & 0x7);
                    break;
            }
        }
        void enableChip() {
            digitalWrite(enablePin, HIGH);
        }
        void disableChip() {
            digitalWrite(enablePin, LOW);
        }


};

template<int input, int clock, int shld, int enable>
class HC165 {
    public:
        static_assert(input != clock, "input and clock pins are equal");
        static_assert(input != shld, "input and shld pins are equal");
        static_assert(input != enable, "input and enable pins are equal");
        static_assert(clock != shld, "clock and shld pins are the same!");
        static_assert(clock != enable, "clock and enable pins are the same!");
        static_assert(shld != enable, "shld and enable pins are the same!");
        using ChipEnabler = HoldPinHigh<enable>;
        using ParallelLoadAction = HoldPinLow<shld>;
        using ClockPulser = HoldPinHigh<enable>;
        static constexpr auto pulseWidthUSec = 5;
    public:
        HC165() {
            setupPins();
        }
        constexpr decltype(input) getInputPin() const noexcept { return input; }
        constexpr decltype(clock) getClockPin() const noexcept { return clock; }
        constexpr decltype(shld) getSHLDPin() const noexcept { return shld; }
        constexpr decltype(enable) getEnablePin() const noexcept { return enable; }

        void setupPins() {
            pinMode(input, INPUT);
            pinMode(clock, OUTPUT);
            pinMode(shld, OUTPUT);
            pinMode(enable, OUTPUT);

            digitalWrite(clock, LOW);
            digitalWrite(shld, HIGH);
        }
        void parallelLoad() {
            ChipEnabler activateChip;
            {
                ParallelLoadAction pload;
                delayMicroseconds(pulseWidthUSec);
            }
        }
        void pulseClock() {
            ClockPulser pulser;
            delayMicroseconds(pulseWidthUSec);
        }

        byte shiftIn() noexcept {
            parallelLoad();
            auto bytesVal = 0;
            for (auto i = 0; i < 8; ++i) {
                /// MSBFIRST
                bytesVal |= (digitalRead(input) << (7 - i));
                pulseClock();
            }
            return bytesVal;
        }

};
} // end namespace bonuspin
#endif // end LIB_BONUSPIN_H__
