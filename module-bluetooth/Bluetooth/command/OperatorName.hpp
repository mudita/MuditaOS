// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

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
