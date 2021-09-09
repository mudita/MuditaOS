// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <gui/Common.hpp>

#include <vector>
#include <cstdint>

namespace gui
{
    template <typename ValType> class StringPolicy
    {
      public:
        using Range = std::vector<ValType>;
        using Type  = ValType;

        StringPolicy(Range range, Boundaries boundaries) : range{range}, boundaries{boundaries}
        {}

        ValType get() const
        {
            return range[pos];
        }

        UTF8 str() const
        {
            return range[pos];
        }

        void set(ValType val)
        {
            for (auto i = 0U; i < range.size(); i++) {
                if (range[i] == val) {
                    pos = i;
                    break;
                }
            }
        }

        void next()
        {
            if (pos >= upRange()) {
                if (boundaries == Boundaries::Continuous) {
                    pos = 0;
                }
                else {
                    pos = upRange();
                }
            }
            else {
                pos++;
            }
        }

        void previous()
        {
            if (pos <= 0) {
                if (boundaries == Boundaries::Continuous) {
                    pos = upRange();
                }
                else {
                    pos = 0;
                }
            }
            else {
                pos--;
            }
        }

      private:
        std::uint32_t upRange() const
        {
            return range.size() - 1;
        }

        const Range range;
        std::uint32_t pos{};
        const Boundaries boundaries{};
    };

    template <typename ValType> class NumericPolicy
    {
      public:
        using Range = struct
        {
            ValType min;
            ValType max;
            ValType step;
        };
        using Type = ValType;

        NumericPolicy(Range range, Boundaries boundaries) : range{range}, boundaries{boundaries}
        {}

        ValType get() const
        {
            return currentValue;
        }

        UTF8 str() const
        {
            return std::to_string(currentValue);
        }

        void set(ValType val)
        {
            currentValue = val;
        }

        void next()
        {
            if (currentValue >= range.max) {
                if (boundaries == Boundaries::Continuous) {
                    currentValue = range.min;
                }
                else {
                    currentValue = range.max;
                }
            }
            else {
                currentValue += range.step;
            }
        }

        void previous()
        {
            if (currentValue <= range.min) {
                if (boundaries == Boundaries::Continuous) {
                    currentValue = range.max;
                }
                else {
                    currentValue = range.min;
                }
            }
            else {
                currentValue -= range.step;
            }
        }

      private:
        const Range range;
        ValType currentValue{};
        const Boundaries boundaries{};
    };

} // namespace gui
