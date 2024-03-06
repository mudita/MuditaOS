// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <common/widgets/LabelOption.hpp>

#include <utility>
namespace gui::option
{

    LabelOption::LabelOption(ItemsType type,
                             const UTF8 &text,
                             std::function<bool(Item &)> activatedCallback,
                             std::function<bool(Item &)> focusChangedCallback,
                             gui::AppWindow *app)
        : OptionBellMenu(text, std::move(activatedCallback), std::move(focusChangedCallback), app), type(type)
    {}

    auto LabelOption::build() const -> ListItem *
    {
        auto labelItem = new LabelListItem(type);
        OptionBellMenu::prepareListItem(labelItem);
        return labelItem;
    }

} // namespace gui::option
