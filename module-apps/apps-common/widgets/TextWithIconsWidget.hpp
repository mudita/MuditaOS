// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <BoxLayout.hpp>
#include <ImageBox.hpp>
#include <TextFixedSize.hpp>

namespace app
{
    class ApplicationCommon;
}

namespace gui
{

    class TextWithIconsWidget : public HBox
    {
        app::ApplicationCommon *app = nullptr;

      public:
        explicit TextWithIconsWidget(gui::Item *parent);

        void addIcon(ImageBox *icon);
        void addText(const std::string &text, const UTF8 &font);

        std::vector<ImageBox *> icons;
    };

    class TextWithSnippet : public HBox
    {
      private:
        constexpr static auto defaultSnippet = "label_rectangle_W_M";

      public:
        TextWithSnippet(Item *parent, const UTF8 &font, const UTF8 &snippet = defaultSnippet);
        TextWithSnippet(const std::string &text, const UTF8 &font, const UTF8 &snippet = defaultSnippet);

        TextFixedSize *textWidget = nullptr;
    };

} /* namespace gui */
