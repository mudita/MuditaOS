// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BGSoundsTimerSelectWindow.hpp"
#include <application-bell-background-sounds/ApplicationBellBackgroundSounds.hpp>
#include <apps-common/widgets/BellBaseLayout.hpp>
#include <data/BGSoundsStyle.hpp>
#include <gui/input/InputEvent.hpp>
#include <i18n/i18n.hpp>
#include <SideListView.hpp>
#include <Utils.hpp>
namespace
{
    using minutes = std::chrono::minutes;
    constexpr minutes offValue{minutes::zero()};

    const std::string &getOffValueText()
    {
        static const std::string offValueText = utils::translate("app_settings_toggle_off");
        return offValueText;
    }

    UTF8 timerValueToUTF8(minutes value)
    {
        if (value == offValue) {
            return getOffValueText();
        }
        return utils::to_string(value.count());
    }

    minutes UTF8ToTimerValue(const UTF8 &str)
    {
        if (str == getOffValueText()) {
            return offValue;
        }
        return minutes(utils::getNumericValue<int>(str));
    }

    std::vector<UTF8> toUTF8Range(const std::vector<minutes> &spinnerRange)
    {
        std::vector<UTF8> range;
        range.reserve(spinnerRange.size());
        for (auto value : spinnerRange) {
            range.push_back(timerValueToUTF8(value));
        }
        return range;
    }
} // namespace
namespace gui
{

    BGSoundsTimerSelectWindow::BGSoundsTimerSelectWindow(
        app::ApplicationCommon *app, std::unique_ptr<app::bgSounds::BGSoundsTimerSelectContract::Presenter> &&presenter)
        : AppWindow(app, gui::window::name::bgSoundsTimerSelect), presenter(std::move(presenter))
    {
        this->presenter->attach(this);
        buildInterface();
    }

    void BGSoundsTimerSelectWindow::buildInterface()
    {
        AppWindow::buildInterface();
        statusBar->setVisible(false);

        body = new BellBaseLayout(this, 0, 0, style::window_width, style::window_height, true);
        createTopMessage();
        createSpinner();
        createBottomDescription();
        registerCallbacks();
    }

    void BGSoundsTimerSelectWindow::createTopMessage()
    {
        auto topMessage = new TextFixedSize(body->firstBox);
        topMessage->setMaximumSize(style::bell_base_layout::w, style::bell_base_layout::outer_layouts_h);
        topMessage->setFont(style::window::font::largelight);
        topMessage->setEdges(gui::RectangleEdge::None);
        topMessage->activeItem = false;
        topMessage->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        topMessage->setText(utils::translate("app_bell_background_sounds_timer_title"));
        topMessage->drawUnderline(false);
    }

    void BGSoundsTimerSelectWindow::createSpinner()
    {
        auto range = presenter->getTimerValuesRange();

        spinner = new UTF8Spinner(toUTF8Range(range), Boundaries::Fixed);
        spinner->setMaximumSize(style::bell_base_layout::w, style::bell_base_layout::h);
        spinner->setFont(bgSoundsStyle::timerValueFont);
        spinner->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        spinner->setEdges(RectangleEdge::None);
        spinner->setFocusEdges(RectangleEdge::None);
        auto currentValue = timerValueToUTF8(presenter->getCurrentTimerValue());
        spinner->setCurrentValue(std::move(currentValue));
        spinner->onValueChanged = [this](const auto &) {
            body->setMinMaxArrowsVisibility(spinner->isAtMin(), spinner->isAtMax());
        };
        body->getCenterBox()->addWidget(spinner);
        body->setMinMaxArrowsVisibility(spinner->isAtMin(), spinner->isAtMax());
    }

    void BGSoundsTimerSelectWindow::createBottomDescription()
    {
        bottomDescription = new gui::Text();
        bottomDescription->setMinimumSize(style::bell_base_layout::outer_layouts_w,
                                          style::bell_base_layout::outer_layouts_h);
        bottomDescription->setFont(bgSoundsStyle::descriptionFont);
        bottomDescription->setEdges(RectangleEdge::All);
        bottomDescription->activeItem = false;
        bottomDescription->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        bottomDescription->setText(utils::translate("common_minutes_lower"));
        body->lastBox->addWidget(bottomDescription);
        updateBottomDescription();
    }
    void BGSoundsTimerSelectWindow::updateBottomDescription()
    {
        const bool isDescriptionVisible = UTF8ToTimerValue(spinner->getCurrentValue()) != offValue;
        bottomDescription->setVisible(isDescriptionVisible);
    }

    void BGSoundsTimerSelectWindow::registerCallbacks()
    {
        dimensionChangedCallback = [&](Item &, const BoundingBox &newDim) -> bool {
            body->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };

        focusChangedCallback = [&](Item &) {
            setFocusItem(focus ? body : nullptr);
            if (focus) {
                setFocusItem(body);
            }
            else {
                setFocusItem(nullptr);
            }
            return true;
        };
    }

    void BGSoundsTimerSelectWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (data && typeid(*data) == typeid(BGSoundsSwitchData)) {
            auto *audioSwitchData = static_cast<BGSoundsSwitchData *>(data);
            audioContext          = audioSwitchData->getAudioContext();
        }
    }

    bool BGSoundsTimerSelectWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (spinner->onInput(inputEvent)) {
            updateBottomDescription();
            return true;
        }
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER)) {
            auto currentValue = UTF8ToTimerValue(spinner->getCurrentValue());
            presenter->setTimerValue(currentValue);

            auto audioSwitchData                        = std::make_unique<BGSoundsSwitchData>(std::move(audioContext));
            audioSwitchData->ignoreCurrentWindowOnStack = true;
            application->switchWindow(gui::window::name::bgSoundsProgress, std::move(audioSwitchData));

            return true;
        }
        return AppWindow::onInput(inputEvent);
    }

} // namespace gui
