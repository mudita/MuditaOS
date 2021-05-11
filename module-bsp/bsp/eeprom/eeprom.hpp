// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>

extern "C"
{
#include "FreeRTOS.h"
}

namespace bsp::eeprom
{
    typedef std::uint16_t addr_t;

    void init();

    bool isPresent(int busid);

    int eeprom_write( int busid, addr_t mem_addr, const char *buf, size_t len );

    int eeprom_read( int busid, addr_t mem_addr, char* buf, size_t len );

    int eeprom_total_size( int busid );

    int eeprom_block_size( int busid );

} // namespace bsp::eeprom
