// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <common/widgets/LabelListItem.hpp>
#include <common/widgets/ListViewWithLabels.hpp>
#include <common/options/OptionBellMenu.hpp>
namespace gui::option
{
    class LabelOption : public OptionBellMenu
    {
      private:
        ItemsType type{};

      public:
        LabelOption(ItemsType type,
                    const UTF8 &text,
                    std::function<bool(Item &)> activatedCallback,
                    std::function<bool(Item &)> focusChangedCallback,
                    AppWindow *app);

        [[nodiscard]] auto build() const -> ListItem * override;
    };
} // namespace gui::option
