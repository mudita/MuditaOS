// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "defs.hpp"
#include <string>

namespace purefs::blkdev
{
    //! Disk partition table structure
    struct partition
    {
        int phycical_number{};     //! Partition number
        int logical_number{};      //! Partition logical one in the mudita
        sector_t start_sector{};   //! First sector
        std::size_t num_sectors{}; //! Number of sectors
        unsigned char bootable{};  //! Partition is bootable
        unsigned short type{};     //! Partition code
        std::string name;          //! Partition name in block manager
    };
} // namespace purefs::blkdev
