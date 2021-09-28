// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <ListItem.hpp>
#include <module-gui/gui/widgets/RichTextParser.hpp>

namespace gui
{
    class Text;
    class VBox;

    class AboutYourBellListItem : public ListItem
    {
      public:
        using TokenMap = std::optional<text::RichTextParser::TokenMap>;
        AboutYourBellListItem(const std::string &title, const std::string &value, TokenMap valueTokenMap = {});

      private:
        VBox *body{};
        Text *title{};
        Text *value{};
    };
} /* namespace gui */
