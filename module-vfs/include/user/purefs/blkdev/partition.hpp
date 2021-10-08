// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "defs.hpp"
#include <string>

namespace purefs::blkdev
{
    //! Disk partition table structure
    struct partition
    {
        int physical_number{};     //! Partition physical number in part table
        int mbr_number{-1};        //! If mbr partition
        sector_t start_sector{};   //! First sector
        std::size_t num_sectors{}; //! Number of sectors
        bool bootable{};           //! Partition is bootable
        unsigned char boot_unit{}; //! 7 bit boot unit field
        unsigned short type{};     //! Partition code
        std::string name;          //! Partition name in block manager
    };
} // namespace purefs::blkdev
