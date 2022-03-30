// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <string>
#include <string_view>
namespace bluetooth
{

    class OperatorName
    {
      public:
        explicit OperatorName(const std::string &name) : name(name){};
        OperatorName() = default;
        auto getName() const -> const std::string_view
        {
            return name;
        }

      private:
        std::string name{""};
    };
} // namespace bluetooth
