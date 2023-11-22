// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "data/RelaxationCommon.hpp"
#include "RelaxationItem.hpp"
#include <common/options/OptionBellMenu.hpp>
namespace gui::option
{
    class RelaxationOption : public OptionBellMenu
    {
      private:
        app::relaxation::MusicType musicType{app::relaxation::MusicType::Undefined};

      public:
        RelaxationOption(app::relaxation::MusicType musicType,
                         const UTF8 &text,
                         std::function<bool(Item &)> activatedCallback,
                         std::function<bool(Item &)> focusChangedCallback,
                         AppWindow *app);

        [[nodiscard]] auto build() const -> ListItem * override;
    };
} // namespace gui::option
