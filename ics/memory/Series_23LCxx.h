/**
 * @file 
 * Code for interfacing with Microchip 23LCXXXX series SPI sram chips.
 * Currently, only the 23LC1024 is actually tested and supported
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
#ifndef LIB_ICS_MEMORY_SERIES_23LCXX_H__
#define LIB_ICS_MEMORY_SERIES_23LCXX_H__
#include "Arduino.h"
#include <SPI.h>
#include "core/concepts.h"
namespace bonuspin {
namespace sram {
namespace microchip {
namespace series_23lcxx {
    enum class Opcodes_23LC1024 : uint8_t {
        RDSR = 0x05,
        RDMR = RDSR,
        WRSR = 0x01,
        WRMR = WRSR,
        READ = 0x03,
        WRITE = 0x02,
        EDIO = 0x3B,
        EQIO = 0x38,
        RSTIO = 0xFF,
    };

    class Device_23LC1024 final { };

    void sendOpcode(Opcodes_23LC1024 opcode) noexcept {
        SPI.transfer(uint8_t(opcode));
    }
    void transferAddress(uint32_t address, Device_23LC1024) noexcept {
        SPI.transfer(static_cast<uint8_t>(address >> 16));
        SPI.transfer(static_cast<uint8_t>(address >> 8));
        SPI.transfer(static_cast<uint8_t>(address));
    }
    /**
     * read a byte from the device, it is assumed that the cs pin is already
     * held low before entering this function.
     */
    uint8_t read8(uint32_t address, Device_23LC1024 dev) noexcept {
        sendOpcode(Opcodes_23LC1024::READ);
        transferAddress(address, dev);
        return SPI.transfer(0x00);
    }

    /**
     * read a byte from the device, it is assumed that the cs pin is already
     * held low!
     */
    void write8(uint32_t address, uint8_t value, Device_23LC1024 dev) noexcept {
        sendOpcode(Opcodes_23LC1024::WRITE);
        transferAddress(address, dev);
        SPI.transfer(value);
    }


} // end namespace series_23lcxx
} // end namespace microchip
} // end namespace sram
} // end namespace bonuspin
#endif // end LIB_ICS_MEMORY_SERIES_23LCXX_H__
