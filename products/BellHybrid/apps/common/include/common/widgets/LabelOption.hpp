// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <common/widgets/LabelListItem.hpp>
#include <common/options/OptionBellMenu.hpp>
namespace gui::option
{
    template <typename T>
    class LabelOption : public OptionBellMenu
    {
      private:
        std::optional<T> type{};

      public:
        LabelOption(std::optional<T> type,
                    const UTF8 &text,
                    std::function<bool(Item &)> activatedCallback,
                    std::function<bool(Item &)> focusChangedCallback,
                    gui::AppWindow *app)
            : OptionBellMenu(text, std::move(activatedCallback), std::move(focusChangedCallback), app), type(type)
        {}

        auto build() const -> ListItem *
        {
            auto labelItem = new LabelListItem<T>(type);
            OptionBellMenu::prepareListItem(labelItem);
            return labelItem;
        }
    };

} // namespace gui::option
