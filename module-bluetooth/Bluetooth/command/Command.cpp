// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Command.hpp"
#include <log/log.hpp>

namespace bluetooth
{

    Command::Command(CommandPack &&pack) : pack(std::move(pack))
    {}

    auto Command::getType() const noexcept -> Command::Type
    {
        return pack.commandType;
    }

    auto Command::getData() -> DataVariant
    {
        if (pack.data == nullptr) {
            LOG_ERROR("Terrible,terrible damage!");
            return DataVariant{};
        }
        return pack.data->getData();
    }
} // namespace bluetooth
