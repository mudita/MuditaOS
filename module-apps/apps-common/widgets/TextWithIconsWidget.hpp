// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <BoxLayout.hpp>
#include <ImageBox.hpp>
namespace app
{
    class ApplicationCommon;
}

namespace gui
{

    class TextWithIconsWidget : public HBox
    {
        [[maybe_unused]] app::ApplicationCommon *app = nullptr;

      public:
        explicit TextWithIconsWidget(gui::Item *parent);

        void addIcon(ImageBox *icon);
        void addText(const std::string &text, const UTF8 &font);

        std::vector<ImageBox *> icons;
    };

    class TextWithSnippet : public HBox
    {
        constexpr static auto defaultSnippet = "Rectangle";

      public:
        TextWithSnippet(const std::string &text, const UTF8 &font, const UTF8 &snippet = defaultSnippet);
    };

} /* namespace gui */
