// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "M24256.hpp"
#include "bsp/eeprom/eeprom.hpp"
#include "board/BoardDefinitions.hpp"
#include "drivers/i2c/DriverI2C.hpp"

#include "fsl_common.h"
#include <log/log.hpp>

#include "task.h"

namespace bsp::eeprom
{
    namespace
    {
        std::shared_ptr<drivers::DriverI2C> i2c;

        drivers::I2CAddress addr = {
            .deviceAddress = static_cast<std::uint32_t>(M24256_MEM_DEVICE_ADDR), .subAddress = 0, .subAddressSize = 2};

    } // namespace

    void init()
    {
        drivers::DriverI2CParams i2cParams;
        i2cParams.baudrate = static_cast<std::uint32_t>(BoardDefinitions::EEPROM_I2C_BAUDRATE);
        i2c = drivers::DriverI2C::Create(static_cast<drivers::I2CInstances>(BoardDefinitions::EEPROM_I2C), i2cParams);
    }

    bool isPresent(int busid)
    {
        std::uint8_t readout;
        addr.deviceAddress |= static_cast<std::uint32_t>(busid) & M24256_DEV_ID_MASK;
        addr.subAddress = 0x0000;
        return i2c->Read(addr, &readout, 1) > 0;
    }

    int eeprom_write(int busid, addr_t mem_addr, const char *buf, size_t len)
    {
        size_t written = 0;
        char *ptr      = const_cast<char *>(buf);

        addr.deviceAddress |= static_cast<std::uint32_t>(busid) & M24256_DEV_ID_MASK;
        addr.subAddress = __builtin_bswap16(mem_addr);

        size_t bl_len   = static_cast<size_t>(eeprom_block_size(busid));
        size_t chunks   = len / bl_len;
        size_t reminder = static_cast<size_t>(len % bl_len);

        LOG_DEBUG("[EEPROM - R] chunks: %d, rem: %d", chunks, reminder);

        if (chunks > 0) {
            for (size_t i = 0; i < chunks; i++) {
                LOG_DEBUG("[EEPROM - W] writing chunk %d of %d", i, chunks);
                written += i2c->Write(addr, reinterpret_cast<std::uint8_t *>(ptr), static_cast<size_t>(bl_len));
                vTaskDelay(pdMS_TO_TICKS(10));
                ptr += bl_len;
                mem_addr += bl_len;
                addr.subAddress = __builtin_bswap16(mem_addr);
            }
        }
        // reminder
        if (reminder > 0) {
            LOG_DEBUG("[EEPROM - W] writing remaining %d bytes", reminder);
            written += i2c->Write(addr, reinterpret_cast<std::uint8_t *>(ptr), reminder);
            vTaskDelay(pdMS_TO_TICKS(10));
        }

        return static_cast<int>(written);
    }

    int eeprom_read(int busid, addr_t mem_addr, char *buf, size_t len)
    {
        size_t read = 0;
        char *ptr   = const_cast<char *>(buf);

        addr.deviceAddress |= static_cast<std::uint32_t>(busid) & M24256_DEV_ID_MASK;
        addr.subAddress = __builtin_bswap16(mem_addr);

        size_t bl_len   = static_cast<size_t>(eeprom_block_size(busid));
        size_t chunks   = len / bl_len;
        size_t reminder = static_cast<size_t>(len % bl_len);

        LOG_DEBUG("[EEPROM - R] chunks: %d, rem: %d", chunks, reminder);

        if (chunks > 0) {
            for (size_t i = 0; i < chunks; i++) {
                LOG_DEBUG("[EEPROM - R] reading chunk %d of %d", i, chunks);
                read += i2c->Read(addr, reinterpret_cast<std::uint8_t *>(ptr), static_cast<size_t>(bl_len));
                ptr += bl_len;
                mem_addr += bl_len;
                addr.subAddress = __builtin_bswap16(mem_addr);
            }
        }
        // reminder
        if (reminder > 0) {
            LOG_DEBUG("[EEPROM - R] reading remaining %d bytes", reminder);
            read += i2c->Read(addr, reinterpret_cast<std::uint8_t *>(ptr), reminder);
        }

        return static_cast<int>(read);
    }

    int eeprom_total_size(int busid)
    {
        // note that M24256 doesn't provide any ID or info register. So i assume that with T7 rev. this memory is @0x0h
        // address
        if (busid == M24256_SLAVE_ADDR)
            return M24256_TOTAL_SIZE;
        else
            return -EFAULT;
    }

    int eeprom_block_size(int busid)
    {
        // note that M24256 doesn't provide any ID or info register. So i assume that with T7 rev. this memory is @0x0h
        // address
        if (busid == M24256_SLAVE_ADDR)
            return M24256_PAGE_SIZE;
        else
            return -EFAULT;
    }

} // namespace bsp::eeprom
