// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AppWindow.hpp"
#include "RichTextParser.hpp"
#include "Text.hpp"

namespace locks
{
    class Lock;
}

namespace gui
{
    class LockWindow : public AppWindow
    {
      public:
        enum class TextType
        {
            Title,
            Primary,
            Secondary
        };

        enum class Token
        {
            Sim,
            Attempts,
            Mins,
            CmeCode,
            PinType
        };

        LockWindow(app::Application *app, std::string name) : AppWindow(app, std::move(name))
        {}
        void build();
        void buildInfoTexts();
        void buildPinLabels(const std::function<Rect *()> &itemBuilder,
                            unsigned int pinSize,
                            unsigned int offsetX,
                            unsigned int offsetY,
                            unsigned int boxWidth);
        virtual void restore() noexcept;
        void setBottomBarWidgetsActive(bool left, bool center, bool right);
        void setText(const std::string &value,
                     TextType type,
                     text::RichTextParser::TokenMap tokens = text::RichTextParser::TokenMap{});
        void setTitleBar(bool isVisible);

        std::unique_ptr<locks::Lock> lock = nullptr;
        gui::HBox *pinLabelsBox          = nullptr;
        gui::Text *primaryText           = nullptr;
        gui::Text *secondaryText         = nullptr;

      protected:
        virtual void buildBottomBar();
        virtual void buildTitleBar() = 0;
    };
} // namespace gui
