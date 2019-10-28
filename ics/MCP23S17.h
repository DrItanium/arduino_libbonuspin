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

template<int chipEnable, byte address>
class MCP23S17 {
    static_assert((address & 0b111) == address, "Provided address is too large!");
    public:
        using Self = MCP23S17<chipenable, address>;
        using CSEnabler = HoldPinLow<chipEnable>;
        static constexpr auto ChipEnablePin = chipEnable;
        static constexpr auto BusAddress = address;
        constexpr auto getChipEnablePin() const noexcept { return chipEnable; }
        constexpr auto getSPIAddress() const noexcept { return address; }
    public:
        //MCP23S17()
};


} // end namespace bonuspin

#endif // end LIB_ICS_MCP23S17_H__
