// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <ListItem.hpp>

namespace gui
{
    class Text;
    class VBox;
} // namespace gui

namespace app::meditation
{
    class SummaryListItem : public gui::ListItem
    {
      public:
        SummaryListItem(const std::string &title, const std::string &value);

      private:
        gui::VBox *body{};
        gui::Text *title{};
        gui::Text *value{};
    };
} // namespace app::meditation
