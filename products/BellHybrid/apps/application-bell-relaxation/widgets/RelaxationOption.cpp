// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RelaxationOption.hpp"

#include <utility>
namespace gui::option
{

    RelaxationOption::RelaxationOption(app::relaxation::MusicType musicType,
                                       const UTF8 &text,
                                       std::function<bool(Item &)> activatedCallback,
                                       std::function<bool(Item &)> focusChangedCallback,
                                       gui::AppWindow *app)
        : OptionBellMenu(text, std::move(activatedCallback), std::move(focusChangedCallback), app), musicType(musicType)
    {}

    auto RelaxationOption::build() const -> ListItem *
    {
        auto relaxationItem = new RelaxationItem(musicType);
        OptionBellMenu::prepareListItem(relaxationItem);
        return relaxationItem;
    }

} // namespace gui::option
