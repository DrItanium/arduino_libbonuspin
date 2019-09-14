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
#include <dht11.h>

namespace bonuspin {
namespace keyestudio {
namespace multipurpose_shield {
namespace v2 {
    constexpr auto Button1 = A1;
    constexpr auto Button2 = A2;
    constexpr auto Button3 = A3;
    constexpr auto Potentiometer = A0;
    constexpr auto PassiveBuzzer = 3;
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

class V1 {
    public:
        static constexpr auto SW1 = 2;
        static constexpr auto SW2 = 3;
        static constexpr auto LED4 = 12;
        static constexpr auto LED3 = 13;
        static constexpr auto LEDRed = 9;
        static constexpr auto LEDGreen = 10;
        static constexpr auto LEDBlue = 11;
        static constexpr auto IRReciever = 6;
        static constexpr auto PassiveBuzzer = 5;
        static constexpr auto Photocell = A1;
        static constexpr auto Potentiometer = A0;
        static constexpr auto LM35 = A2;
        static constexpr auto DHT11 = 4;

        inline V1() noexcept {
            pinMode(LED4, OUTPUT);
            pinMode(LED3, OUTPUT);
            pinMode(LEDRed, OUTPUT);
            pinMode(LEDGreen, OUTPUT);
            pinMode(LEDBlue, OUTPUT);
            digitalWrite(LED4, LOW);
            digitalWrite(LED3, LOW);
            digitalWrite(LEDRed, LOW);
            digitalWrite(LEDBlue, LOW);
            digitalWrite(LEDGreen, LOW);
        }


        inline decltype(analogRead(Potentiometer)) readPot() noexcept {
            return analogRead(Potentiometer);
        }
        inline int readPot(int mapRangeStart, int mapRangeEnd) noexcept {
            return map(readPot(), 0, 1023, mapRangeStart, mapRangeEnd);
        }
        inline int updateDHT11() noexcept {
            return _dht.read(DHT11);
        }
        int getHumdity() const noexcept { return _dht.humidity; }
        int getTemperature() const noexcept { return _dht.temperature; }
        int readLM35() noexcept { return analogRead(LM35); }
        int getLightLevel() noexcept { return analogRead(Photocell); }
        template<unsigned int numSamples = 16>
        int getAverageLightLevel() noexcept {
            static_assert(numSamples > 0, "Can't have zero samples");
            if (numSamples == 1) {
                return getLightLevel();
            } else {
                int ll = 0;
                for (int i = 0; i < numSamples; ++i) {
                    ll += getLightLevel();
                }
                return ll / numSamples;
            }
        }
        void emitColor(uint32_t c) const noexcept {
            emitColor(((c & 0xFF0000) >> 16),
                    ((c & 0x00FF00) >> 8),
                    lowByte(c));
        }
        void emitColor(uint8_t red, uint8_t green, uint8_t blue) const noexcept {
            analogWrite(LEDRed, red);
            analogWrite(LEDGreen, green);
            analogWrite(LEDBlue, blue);
        }



    private:
        dht11 _dht;
};

} // end namespace multipurpose_shield
} // end namespace keyestudio
} // end bonuspin

inline void digitalWrite(bonuspin::keyestudio::multipurpose_shield::v2::leds l, decltype(HIGH) value) {
    digitalWrite(static_cast<int>(l), value);
}
inline void pinMode(bonuspin::keyestudio::multipurpose_shield::v2::leds l, decltype(OUTPUT) kind) noexcept {
    pinMode(static_cast<int>(l), kind);
}
#endif // end LIB_BONUSPIN_EASYMODULE_SHIELD_H__