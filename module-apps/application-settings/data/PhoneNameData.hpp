// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <gui/SwitchData.hpp>

namespace gui
{
    class PhoneNameData : public SwitchData
    {
      public:
        explicit PhoneNameData(std::string name) : name(std::move(name))
        {}
        [[nodiscard]] auto getName() const -> const std::string &
        {
            return name;
        }

      private:
        const std::string name;
    };
} // namespace gui
