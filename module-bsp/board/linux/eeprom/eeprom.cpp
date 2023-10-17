// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <cstdint>
#include "bsp/eeprom/eeprom.hpp"

namespace bsp::eeprom
{
    void init()
    {}

    bool isPresent(int busid)
    {
        return true;
    }

    int eeprom_write(int busid, addr_t mem_addr, const char *buf, size_t len)
    {
        return true;
    }

    int eeprom_read(int busid, addr_t mem_addr, char *buf, size_t len)
    {
        return 0;
    }

    int eeprom_total_size(int busid)
    {
        return 0;
    }

    int eeprom_block_size(int busid)
    {
        return 0;
    }

} // namespace bsp::eeprom
