// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AppWindow.hpp"
#include "Text.hpp"

#include "RichTextParser.hpp"
#include "PinLock.hpp"

namespace gui
{
    class PinLockBaseWindow : public AppWindow
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
            CmeCode
        };

        PinLockBaseWindow(app::Application *app, std::string name) : AppWindow(app, name)
        {}
        virtual void invalidate() noexcept;
        void build();
        void buildInfoTexts();
        void buildPinLabels(std::function<Rect *()> itemBuilder,
                            unsigned int pinSize,
                            unsigned int offsetX,
                            unsigned int offsetY,
                            unsigned int boxWidth);
        void buildImages(const std::string &lockImg, const std::string &infoImg);
        void setBottomBarWidgetsActive(bool left, bool center, bool right);
        void setImagesVisible(bool lockImg, bool infoImg);
        void setTitleBar(bool isVisible, bool isIceActive);
        void setText(const std::string &value,
                     TextType type,
                     bool isReach                          = false,
                     text::RichTextParser::TokenMap tokens = text::RichTextParser::TokenMap{});
        void restore() noexcept;
        [[nodiscard]] auto getToken(Token token) const -> std::string;

        gui::HBox *iceBox        = nullptr;
        gui::Text *title         = nullptr;
        gui::Text *primaryText   = nullptr;
        gui::Text *secondaryText = nullptr;
        gui::Image *lockImage    = nullptr;
        gui::Image *infoImage    = nullptr;
        gui::HBox *pinLabelsBox  = nullptr;

      private:
        void buildBottomBar();
        void buildTopBar();
        void buildTitleBar();
        [[nodiscard]] auto getText(TextType type) noexcept -> gui::Text *;
    };
} // namespace gui
