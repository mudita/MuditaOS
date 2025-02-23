// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once
#include <memory>

namespace purefs::blkdev
{
    class disk;
}

namespace purefs::subsystem::internal
{
    auto create_default_block_device() -> std::shared_ptr<blkdev::disk>;
    auto create_default_nvm_device() -> std::shared_ptr<blkdev::disk>;
} // namespace purefs::subsystem::internal
