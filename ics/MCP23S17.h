/**
 * @file 
 * header only implementation for interfacing with the mcp23s17 digital io
 * expander chip which operates over spi
 * @copyright
 * copyright (c) 2019 joshua scoggins 
 * 
 * this software is provided 'as-is', without any express or implied
 * warranty. in no event will the authors be held liable for any damages
 * arising from the use of this software.
 * 
 * permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 * 1. the origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. if you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. this notice may not be removed or altered from any source distribution.
 */
#ifndef LIB_ICS_MCP23S17_H__
#define LIB_ICS_MCP23S17_H__
#include "Arduino.h"
#include "../core/concepts.h"
#include <spi.h>

namespace bonuspin 
{

template<int chipEnable, byte address, int reset = -1>
class MCP23S17 {
    static_assert((address & 0b111) == address, "Provided address is too large!");
    private:
        static constexpr auto generateByte(bool a, bool b, bool c, bool d, bool e, bool f, bool g, bool h) noexcept {
            byte output = 0;
            output |= (b ? 0b0000'0001 : 0);
            output |= (b ? 0b0000'0010 : 0);
            output |= (c ? 0b0000'0100 : 0);
            output |= (d ? 0b0000'1000 : 0);
            output |= (e ? 0b0001'0000 : 0);
            output |= (f ? 0b0010'0000 : 0);
            output |= (g ? 0b0100'0000 : 0);
            output |= (h ? 0b1000'0000 : 0);
            return output;
        }
        static constexpr auto generateIOConByte(bool intPolarity, bool odr, bool haen, bool disslw, bool seqop, bool mirror, bool bank) noexcept {
            return generateByte(false, intPolarity, odr, haen, disslw, seqop, mirror, bank);
        }
    public:
        using Self = MCP23S17<chipenable, address>;
        using CSEnabler = HoldPinLow<chipEnable>;
        static constexpr auto ChipEnablePin = chipEnable;
        static constexpr auto BusAddress = address;
        static constexpr auto ResetPin = reset;
        static constexpr auto HasResetPin = (ResetPin >= 0);
        constexpr auto getChipEnablePin() const noexcept { return ChipEnablePin; }
        constexpr auto getSPIAddress() const noexcept { return BusAddress; }
        constexpr auto getResetPin() const noexcept { return ResetPin; }
        constexpr auto hasResetPin() const noexcept { return ResetPin >= 0; }
    public:
        MCP23S17() {
            pinMode(ChipEnablePin, OUTPUT);
            digitalWrite(ChipEnablePin, HIGH);
            // we could do if constexpr here T_T if c++17 is default active
            if (hasResetPin()) {
                
            }
        }
        class ReadOperation final { };
        class WriteOperation final { };
        constexpr byte generateOpcode(ReadOperation) const noexcept {
            return 0b0100'0000 | (getSPIAddress() << 1) | 1;
        }
        constexpr byte generateOpcode(WriteOperation) const noexcept {
            return 0b0100'0000 | (getSPIAddress() << 1);
        }

        byte read(byte registerAddress) noexcept {
            CSEnabler enable;
            SPI.transfer(static_cast<uint8_t>(generateOpcode(ReadOperation{})));
            SPI.transfer(static_cast<uint8_t>(registerAddress));
            return SPI.transfer(0x00);
        }
        void write(byte registerAddress, byte value) noexcept {
            CSEnabler enable;
            SPI.transfer(static_cast<uint8_t>(generateOpcode(WriteOperation{})));
            SPI.transfer(static_cast<uint8_t>(registerAddress));
            SPI.transfer(static_cast<uint8_t>(value));
        }
    private:
};


} // end namespace bonuspin

#endif // end LIB_ICS_MCP23S17_H__
