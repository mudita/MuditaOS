// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <locks/widgets/LockBox.hpp>

#include <AppWindow.hpp>
#include <RichTextParser.hpp>
#include <Text.hpp>
#include <ImageBox.hpp>

namespace locks
{
    class Lock;
}

namespace gui
{
    class LockInputWindow : public AppWindow
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
            Time,
            CmeCode,
            PinType
        };

        void buildPinLabels(const std::function<Rect *()> &itemBuilder, unsigned int pinSize);
        void setText(const std::string &value,
                     TextType type,
                     text::RichTextParser::TokenMap tokens = text::RichTextParser::TokenMap{});
        void setTitleBar(bool titleVisible, bool iceVisible);
        void setImage(const UTF8 &imageName);
        void setBottomBarWidgetsActive(bool left, bool center, bool right);

        [[nodiscard]] auto getToken(Token token) const -> std::string;
        status_bar::Configuration configureStatusBar(status_bar::Configuration appConfiguration) override;

        std::unique_ptr<locks::Lock> lock = nullptr;
        gui::HBox *pinLabelsBox           = nullptr;

      protected:
        LockInputWindow(app::Application *app, std::string name) : AppWindow(app, std::move(name))
        {}

        void build();
        void restore() noexcept;

        virtual void setVisibleState();
        std::unique_ptr<LockBox> lockBox = nullptr;

        [[nodiscard]] auto isInInputState() const noexcept -> bool;
        [[nodiscard]] auto isInInvalidInputState() const noexcept -> bool;
        [[nodiscard]] auto isIceVisible() const noexcept -> bool;

      private:
        gui::VBox *body          = nullptr;
        gui::ImageBox *infoImage = nullptr;
        gui::Text *primaryText   = nullptr;
        gui::Text *secondaryText = nullptr;

        void buildBody();
        void buildImage();
        void buildInfoTexts();
        void buildPinBody();
        void buildBottomBar();
        void buildIceBox();
    };
} // namespace gui
