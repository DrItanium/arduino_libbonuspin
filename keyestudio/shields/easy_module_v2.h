/**
 * @file 
 * Classes to make working with the keyestudio v1 and v2 easy module shields
 * easier.
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
#ifndef LIB_BONUSPIN_EASYMODULE_SHIELD_H__
#define LIB_BONUSPIN_EASYMODULE_SHIELD_H__
#include "Arduino.h"
#include "libbonuspin.h"

namespace bonuspin {
    namespace keyestudio {
        namespace shields {

            class EasyModuleV2 : public bonuspin::HasPotentiometer<A0> {
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
                static constexpr auto LED4_ST_CP = 4;
                static constexpr auto LED4_SH_CP = 5;
                static constexpr auto LED4_DS = 2;
                /**
                 * Makes working with SN74HC595 chips that interface with the keyestudio
                 * multipurpose shield v2's 4 digit led display much much easier!
                 */
                class FourDigitLEDDisplay : private bonuspin::SN74HC595<LED4_ST_CP,LED4_SH_CP,LED4_DS> {
                    private:
                        using Parent = bonuspin::SN74HC595<LED4_ST_CP,LED4_SH_CP,LED4_DS>;
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
                public:
                    static constexpr auto Button1 = A1;
                    static constexpr auto Button2 = A2;
                    static constexpr auto Button3 = A3;
                    static constexpr auto Potentiometer = A0;
                    static constexpr auto PassiveBuzzer = 3;

                    static constexpr auto LED1 = 13;
                    static constexpr auto LED2 = 12;
                    static constexpr auto LED3 = 11;
                    static constexpr auto LED4 = 10;
                    static constexpr auto LED5 = 9;
                    static constexpr auto LED6 = 8;


                    EasyModuleV2() noexcept {
                        bonuspin::setupDigitalPin<LED1>();
                        bonuspin::setupDigitalPin<LED2>();
                        bonuspin::setupDigitalPin<LED3>();
                        bonuspin::setupDigitalPin<LED4>();
                        bonuspin::setupDigitalPin<LED5>();
                        bonuspin::setupDigitalPin<LED6>();
                    }

                    void printout(uint16_t val) { _disp.printOut(val); }
                    void printout(int16_t val) { _disp.printOut(val); }
                    EasyModuleV2& operator<<(uint16_t val) { 
                        printout(val);
                        return *this;
                    }
                    EasyModuleV2& operator<<(int16_t val) {
                        printout(val);
                        return *this;
                    }
                    template<decltype(LED1) ledPin>
                    void ledWrite(int value) const noexcept {
                        digitalWrite(ledPin, value);
                    }
                    void writePatternToLeds(uint8_t pattern) const noexcept {
                        ledWrite<LED6>(pattern & 0b100000 ? HIGH : LOW);
                        ledWrite<LED5>(pattern & 0b010000 ? HIGH : LOW);
                        ledWrite<LED4>(pattern & 0b001000 ? HIGH : LOW);
                        ledWrite<LED3>(pattern & 0b000100 ? HIGH : LOW);
                        ledWrite<LED2>(pattern & 0b000010 ? HIGH : LOW);
                        ledWrite<LED1>(pattern & 0b000001 ? HIGH : LOW);
                    }
                private:
                    FourDigitLEDDisplay _disp;

            };

        } // end namespace shields 
    } // end namespace keyestudio
} // end bonuspin

#endif // end LIB_BONUSPIN_EASYMODULE_SHIELD_H__
