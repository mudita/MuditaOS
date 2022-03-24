// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CPUModeTestWindow.hpp"

#include <system/messages/SentinelRegistrationMessage.hpp>
#include <system/messages/RequestCpuFrequencyMessage.hpp>
#include <application-settings/windows/WindowNames.hpp>
#include <Application.hpp>
#include <TextFixedSize.hpp>
#include <BoxLayout.hpp>

#include <magic_enum.hpp>

namespace magic_enum
{
    template <> struct customize::enum_range<bsp::CpuFrequencyMHz>
    {
        static constexpr int min = 0;
        static constexpr int max = 528;
    };
} // namespace magic_enum

namespace gui
{
    CPUModeTestWindow::CPUModeTestWindow(app::ApplicationCommon *app) : AppWindow(app, window::name::cpu_test_window)
    {
        cpuModeTester = std::make_shared<sys::CpuSentinel>(name, app, [&](bsp::CpuFrequencyMHz freq) {
            auto freqToPrint = std::to_string(magic_enum::enum_integer(freq));
            currentFreqValue->setMinimumWidthToFitText(freqToPrint);
            currentFreqValue->setMaximumWidth(currentFreqValue->widgetMinimumArea.w);
            currentFreqValue->informContentChanged();
            currentFreqValue->setText(freqToPrint);

            application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
        });

        auto sentinelRegistrationMsg = std::make_shared<sys::SentinelRegistrationMessage>(cpuModeTester);
        application->bus.sendUnicastSync(sentinelRegistrationMsg, service::name::system_manager, 30);

        AppWindow::buildInterface();

        navBar->setText(nav_bar::Side::Right, utils::translate(style::strings::common::back));

        setTitle(name);

        body = new VBox(this,
                        style::window::default_left_margin,
                        style::window::default_vertical_pos,
                        style::window::default_body_width,
                        style::window::default_body_height);

        body->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        body->setEdges(RectangleEdge::None);

        createCurrentFreqBox();
        createPermanentFreqSettingBox();
        createNewFreqBox();

        body->resizeItems();
        setFocusItem(body);
    }

    void CPUModeTestWindow::onClose(Window::CloseReason reason)
    {
        if (reason != Window::CloseReason::Popup && reason != Window::CloseReason::PhoneLock) {
            auto sentinelRemovalMessage = std::make_shared<sys::SentinelRemovalMessage>(name);
            application->bus.sendUnicastSync(sentinelRemovalMessage, service::name::system_manager, 30);
        }
    }

    void CPUModeTestWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        auto freqToPrint = std::to_string(magic_enum::enum_integer(cpuModeTester->GetFrequency()));

        currentFreqValue->setMinimumWidthToFitText(freqToPrint);
        currentFreqValue->setMinimumHeightToFitText();
        currentFreqValue->setMaximumWidth(currentFreqValue->widgetMinimumArea.w);
        currentFreqValue->informContentChanged();
        currentFreqValue->setText(freqToPrint);

        currentFreqSpinner->setCurrentValue(freqToPrint);
    }

    void CPUModeTestWindow::createCurrentFreqBox()
    {
        auto currentFreqBody = new VBox(body);
        currentFreqBody->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        currentFreqBody->setEdges(RectangleEdge::None);
        currentFreqBody->setMinimumSize(style::window::default_body_width, 80);
        currentFreqBody->setMargins({0, 60, 0, 30});
        currentFreqBody->activeItem = false;

        auto currentFreqLabel = new TextFixedSize(currentFreqBody);
        currentFreqLabel->setAlignment({Alignment::Horizontal::Center});
        currentFreqLabel->setFont(style::window::font::small);
        currentFreqLabel->setMinimumWidth(style::window::default_body_width);
        currentFreqLabel->setMinimumHeightToFitText();
        currentFreqLabel->setMargins({0, 0, 0, 15});
        currentFreqLabel->setText("Current CPU Frequency [MHz]:");
        currentFreqLabel->setEdges(RectangleEdge::None);
        currentFreqLabel->drawUnderline(false);

        currentFreqValue = new TextFixedSize(currentFreqBody);
        currentFreqValue->setAlignment({Alignment::Horizontal::Center});
        currentFreqValue->setFont(style::window::font::mediumbold);
        currentFreqValue->setPenWidth(style::window::default_border_focus_w);
        currentFreqValue->setEdges(RectangleEdge::Bottom);
        currentFreqValue->drawUnderline(false);
        currentFreqValue->setEditMode(EditMode::Browse);
        currentFreqValue->activeItem = false;
    }

    void CPUModeTestWindow::createPermanentFreqSettingBox()
    {
        auto permanentFreqBody = new VBox(body);
        permanentFreqBody->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        permanentFreqBody->setEdges(RectangleEdge::None);
        permanentFreqBody->setMinimumSize(style::window::default_body_width, 80);
        permanentFreqBody->setMargins({0, 0, 0, 30});

        auto permanentFreqLabel = new TextFixedSize(permanentFreqBody);
        permanentFreqLabel->setAlignment({Alignment::Horizontal::Center});
        permanentFreqLabel->setFont(style::window::font::small);
        permanentFreqLabel->setMinimumWidth(style::window::default_body_width);
        permanentFreqLabel->setMinimumHeightToFitText();
        permanentFreqLabel->setMargins({0, 0, 0, 15});
        permanentFreqLabel->setText("Set Permanent Frequency:");
        permanentFreqLabel->setEdges(RectangleEdge::None);
        permanentFreqLabel->drawUnderline(false);
        permanentFreqLabel->activeItem = false;

        permanentFreqSpinner = new gui::TextSpinnerBox(permanentFreqBody, {"OFF", "ON"}, Boundaries::Continuous);
        permanentFreqSpinner->setMinimumSize(100, 30);
        auto ret =
            application->async_call<sys::IsCpuPernament, sys::IsCpuPernamentResponse>(service::name::system_manager);
        application->sync(ret);
        permanentFreqSpinner->setCurrentValue(ret.getResult().pernament ? "ON" : "OFF");

        permanentFreqBody->inputCallback = [&](Item &item, const InputEvent &event) {
            auto ret = permanentFreqSpinner->onInput(event);
            if (ret) {
                if (permanentFreqSpinner->getCurrentValue() == "ON") {
                    application->bus.sendUnicastSync(std::make_shared<sys::HoldCpuFrequencyPermanentlyMessage>(
                                                         magic_enum::enum_cast<bsp::CpuFrequencyMHz>(
                                                             std::stoi(currentFreqSpinner->getCurrentValue()))
                                                             .value()),
                                                     service::name::system_manager,
                                                     5000);
                }
                else {
                    application->bus.sendUnicastSync(std::make_shared<sys::ReleaseCpuPermanentFrequencyMessage>(),
                                                     service::name::system_manager,
                                                     5000);
                }
            }

            return ret;
        };
    }

    void CPUModeTestWindow::createNewFreqBox()
    {
        auto newFreqBody = new VBox(body);
        newFreqBody->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        newFreqBody->setEdges(RectangleEdge::None);
        newFreqBody->setMinimumSize(style::window::default_body_width, 80);

        auto newFreqLabel = new TextFixedSize(newFreqBody);
        newFreqLabel->setAlignment({Alignment::Horizontal::Center});
        newFreqLabel->setFont(style::window::font::small);
        newFreqLabel->setMinimumWidth(style::window::default_body_width);
        newFreqLabel->setMinimumHeightToFitText();
        newFreqLabel->setMargins({0, 0, 0, 15});
        newFreqLabel->setText("New Frequency to set [MHz]:");
        newFreqLabel->setEdges(RectangleEdge::None);
        newFreqLabel->drawUnderline(false);
        newFreqLabel->activeItem = false;

        std::vector<UTF8> freqOptions;
        for (auto val : magic_enum::enum_values<bsp::CpuFrequencyMHz>()) {
            if (val <= bsp::getPowerProfile().minimalFrequency) {
                continue;
            }

            freqOptions.push_back(std::to_string(static_cast<int>(val)));
        }

        currentFreqSpinner = new gui::TextSpinnerBox(newFreqBody, freqOptions, Boundaries::Continuous);
        currentFreqSpinner->setMinimumSize(100, 30);

        newFreqBody->inputCallback = [&](Item &item, const InputEvent &event) {
            auto ret = currentFreqSpinner->onInput(event);

            auto async = application->async_call<sys::IsCpuPernament, sys::IsCpuPernamentResponse>(
                service::name::system_manager);
            application->sync(async);
            if (async.getResult().pernament) {
                application->bus.sendUnicastSync(
                    std::make_shared<sys::HoldCpuFrequencyPermanentlyMessage>(
                        magic_enum::enum_cast<bsp::CpuFrequencyMHz>(std::stoi(currentFreqSpinner->getCurrentValue()))
                            .value()),
                    service::name::system_manager,
                    5000);
            }

            return ret;
        };
    }
} /* namespace gui */
