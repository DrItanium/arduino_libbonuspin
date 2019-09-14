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
#include <IRremote.h>

namespace bonuspin {
    namespace keyestudio {
        namespace shields {
            class EasyModuleV1 : public bonuspin::HasPotentiometer<A0> {
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

                    inline EasyModuleV1() noexcept : _ir(IRReciever) {
                        bonuspin::setupDigitalPin<LED4>();
                        bonuspin::setupDigitalPin<LED3>();
                        bonuspin::setupDigitalPin<LEDRed>();
                        bonuspin::setupDigitalPin<LEDGreen>();
                        bonuspin::setupDigitalPin<LEDBlue>();
                        _ir.enableIRIn();
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

                    int getIRValue() {
                        if (_ir.decode(&_results)) {
                            auto value = _results.value;
                            _ir.resume();
                            return value;
                        } else {
                            return -1;
                        }
                    }

                private:
                    IRrecv _ir;
                    dht11 _dht;
                    decode_results _results;
            };

        } // end namespace shields
    } // end namespace keyestudio
} // end bonuspin

#endif // end LIB_BONUSPIN_EASYMODULE_SHIELD_H__
