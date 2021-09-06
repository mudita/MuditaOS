// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <optional>

namespace gui
{
    template <typename ValueT, typename StringT> class NumWithString
    {
      public:
        NumWithString() = default;

        NumWithString(ValueT value, StringT string) : value{value}, string{string}
        {}

        explicit NumWithString(StringT string) : string{string}
        {}
        explicit NumWithString(ValueT value) : value{value}
        {}

        bool operator==(const NumWithString &oth) const
        {
            return oth.toStr() == toStr();
        }

        operator StringT() const
        {
            return toStr();
        }

        std::optional<ValueT> getValue() const
        {
            return value;
        }

        std::optional<StringT> getSuffix() const
        {
            return string;
        }

      private:
        StringT toStr() const
        {
            StringT retStr;
            if (value) {
                retStr += std::to_string(*value) + " ";
            }
            if (string) {
                retStr += *string;
            }
            return retStr;
        }
        std::optional<ValueT> value;
        std::optional<StringT> string;
    };
} // namespace gui
