// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <time/time_locale.hpp>
#include <utf8/UTF8.hpp>
#include <widgets/BellSideListItem.hpp>

#include <functional>

namespace gui
{
    class Spinner;
    class Label;

    class TimeFormatSetListItem : public gui::BellSideListItem
    {
      public:
        TimeFormatSetListItem() = delete;
        TimeFormatSetListItem(
            gui::Length x, gui::Length y, gui::Length w, gui::Length h, const UTF8 &topDesc, const UTF8 &botDesc);

        auto getTimeFmt() const noexcept -> utils::time::Locale::TimeFormat;
        auto setTimeFmt(utils::time::Locale::TimeFormat fmt) noexcept -> void;

        /// called before next SideListItem is activated
        /// @param `this` : item
        std::function<void(Item &)> onNextCallback;

      private:
        Label *bottomDescription{};
        Spinner *timeFormat{};
    };

} // namespace gui
