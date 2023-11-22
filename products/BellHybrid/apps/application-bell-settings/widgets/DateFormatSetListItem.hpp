// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <time/time_locale.hpp>
#include <utf8/UTF8.hpp>
#include <widgets/BellSideListItem.hpp>
#include <widgets/spinners/Spinners.hpp>

#include <functional>

namespace gui
{
    class Label;

    class DateFormatSetListItem : public gui::BellSideListItem
    {
      public:
        DateFormatSetListItem() = delete;
        DateFormatSetListItem(
            gui::Length x, gui::Length y, gui::Length w, gui::Length h, const UTF8 &topDesc, const UTF8 &botDesc = "");

        auto getDateFmt() const noexcept -> utils::time::Locale::DateFormat;
        auto setDateFmt(utils::time::Locale::DateFormat fmt) noexcept -> void;

        /// called before next SideListItem is activated
        /// @param `this` : item
        std::function<void(Item &)> onNextCallback;

      private:
        Label *bottomDescription{};
        StringSpinner *dateFormat;
    };

} // namespace gui
