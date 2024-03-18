// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <common/widgets/ListViewWithLabels.hpp>
#include <common/options/OptionBellMenu.hpp>

namespace gui::option
{
    class LabelOptionWithTick : public OptionBellMenu
    {
      public:
        enum class TickState
        {
            Show,
            Hide
        };

        LabelOptionWithTick(ListLabel label,
                            const UTF8 &text,
                            TickState tickState,
                            std::function<bool(Item &)> activatedCallback,
                            std::function<bool(Item &)> focusChangedCallback,
                            AppWindow *app);

        auto build() const -> ListItem * override;

      private:
        auto prepareLabelOption(ListItem *item) const -> void;
        auto getAdjustedText(TextFixedSize *textItem) const -> UTF8;

        ListLabel label;
        TickState tickState;
    };
} // namespace gui::option
