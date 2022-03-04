// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "OnBoardingOnOffWindow.hpp"
#include "ApplicationBellOnBoarding.hpp"

#include <InputEvent.hpp>
#include <Image.hpp>
#include <ImageBox.hpp>
#include <TextFixedSize.hpp>
#include <Application.hpp>

namespace style::onboarding
{
    namespace row
    {
        constexpr auto marginInternal = 39;
        constexpr auto marginExternal = 137;
        constexpr auto minimumSizeX   = style::window_width;
        constexpr auto minimumSizeY   = 64;
    }; // namespace row

    namespace arrow
    {
        constexpr auto marginLeft = 24;
    }; // namespace arrow

    namespace text
    {
        constexpr auto marginLeft   = 15;
        constexpr auto minimumSizeX = 81;
        constexpr auto minimumSizeY = 62;
    } // namespace text
};    // namespace style::onboarding

namespace gui
{

    OnBoardingOnOffWindow::OnBoardingOnOffWindow(app::ApplicationCommon *app,
                                                 std::unique_ptr<AbstractPowerOffPresenter> presenter,
                                                 const std::string &name)
        : WindowWithTimer(app, name, displayDuration), presenter{std::move(presenter)}
    {
        buildInterface();
    }

    void OnBoardingOnOffWindow::buildInterface()
    {
        AppWindow::buildInterface();
        buildLayout();
        registerCallbacks();
    }

    void OnBoardingOnOffWindow::buildLayout()
    {
        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        navBar->setVisible(false);

        auto mainVBox = new VBox(this, 0, 0, style::window_width, style::window_height);
        mainVBox->setEdges(RectangleEdge::None);

        auto onHBox = new HBox(mainVBox, 0, 0, 0, 0);
        onHBox->setMinimumSize(style::onboarding::row::minimumSizeX, style::onboarding::row::minimumSizeY);
        onHBox->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center});
        onHBox->setEdges(RectangleEdge::None);
        onHBox->setMargins({0, style::onboarding::row::marginExternal, 0, style::onboarding::row::marginInternal});

        auto offHBox = new HBox(mainVBox, 0, 0, 0, 0);
        offHBox->setMinimumSize(style::onboarding::row::minimumSizeX, style::onboarding::row::minimumSizeY);
        offHBox->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center});
        offHBox->setEdges(RectangleEdge::None);
        offHBox->setMargins({0, style::onboarding::row::marginInternal, 0, style::onboarding::row::marginExternal});

        auto sunImg = new ImageBox(onHBox, 0, 0, 0, 0, new Image("button_icon_sun", ImageTypeSpecifier::W_G));
        sunImg->setMinimumSizeToFitImage();
        sunImg->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center});

        auto onArrow = new ImageBox(onHBox, 0, 0, 0, 0, new Image("arrow_right_long", ImageTypeSpecifier::W_G));
        onArrow->setMinimumSizeToFitImage();
        onArrow->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center});
        onArrow->setMargins({style::onboarding::arrow::marginLeft, 0, 0, 0});

        auto onText = new TextFixedSize(onHBox, 0, 0, 0, 0);
        onText->setFont(style::window::font::large);
        onText->setText(utils::translate("app_settings_toggle_on"));
        onText->setMinimumSize(style::onboarding::text::minimumSizeX, style::onboarding::text::minimumSizeY);
        onText->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center});
        onText->setEdges(RectangleEdge::None);
        onText->drawUnderline(false);
        onText->setMargins({style::onboarding::text::marginLeft, 0, 0, 0});

        auto backImg = new ImageBox(offHBox, 0, 0, 0, 0, new Image("arrow_back", ImageTypeSpecifier::W_G));
        backImg->setMinimumSizeToFitImage();
        backImg->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center});

        auto offArrow = new ImageBox(offHBox, 0, 0, 0, 0, new Image("arrow_right_long", ImageTypeSpecifier::W_G));
        offArrow->setMinimumSizeToFitImage();
        offArrow->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center});
        offArrow->setMargins({style::onboarding::arrow::marginLeft, 0, 0, 0});

        auto offText = new TextFixedSize(offHBox, 0, 0, 0, 0);
        offText->setFont(style::window::font::large);
        offText->setText(utils::translate("app_settings_toggle_off"));
        offText->setMinimumSize(style::onboarding::text::minimumSizeX, style::onboarding::text::minimumSizeY);
        offText->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center});
        offText->setEdges(RectangleEdge::None);
        offText->drawUnderline(false);
        offText->setMargins({style::onboarding::text::marginLeft, 0, 0, 0});

        onHBox->resizeItems();
        mainVBox->resizeItems();
        onHBox->resizeItems();
    }

    void OnBoardingOnOffWindow::registerCallbacks()
    {
        timerCallback = [this](Item &, sys::Timer &timer) {
            powerOff();
            return true;
        };
    }

    bool OnBoardingOnOffWindow::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(KeyCode::KEY_LF)) {
            application->switchWindow(gui::window::name::onBoardingLanguageWindow);
            return true;
        }
        else if (inputEvent.isShortRelease(KeyCode::KEY_RF)) {
            powerOff();
            return true;
        }
        return false;
    }

    void OnBoardingOnOffWindow::powerOff()
    {
        application->switchWindow(gui::window::name::onBoardingStartupWindow);
        presenter->powerOff();
    }
} // namespace gui
