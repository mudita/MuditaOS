// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "IntervalChimeWindow.hpp"
#include "ReadyGoingWindow.hpp"
#include "../data/MeditationSwitchData.hpp"

#include "log.hpp"
#include <i18n/i18n.hpp>

namespace gui
{
    IntervalChimeWindow::IntervalChimeWindow(app::ApplicationCommon *app)
        : MeditationWindow(app, gui::name::window::interval_chime)
    {
        buildInterface();
    }

    void IntervalChimeWindow::rebuild()
    {}

    void IntervalChimeWindow::buildInterface()
    {
        MeditationWindow::buildInterface();

        previousImage = new gui::Image(
            this, icStyle::left_arrow::x, icStyle::left_arrow::y, 0, 0, icStyle::left_arrow::imageSource);
        nextImage = new gui::Image(
            this, icStyle::right_arrow::x, icStyle::right_arrow::y, 0, 0, icStyle::right_arrow::imageSource);

        auto newLabel = [](Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
            auto label = new gui::Label(parent, x, y, w, h);
            label->setFilled(false);
            label->setBorderColor(gui::ColorNoColor);
            label->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
            return label;
        };

        title = newLabel(this, icStyle::title::x, icStyle::title::y, icStyle::title::w, icStyle::title::h);
        title->setFont(icStyle::title::font);
        title->setText(utils::translate("app_meditation_interval_chime"));

        text = newLabel(this, icStyle::text::x, icStyle::text::y, icStyle::text::w, icStyle::text::h);
        text->setFont(icStyle::text::font);

        minute = newLabel(this, icStyle::minute::x, icStyle::minute::y, icStyle::minute::w, icStyle::minute::h);
        minute->setFont(icStyle::minute::font);
        minute->setText(utils::translate("app_meditation_bell_minutes"));
        minute->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Top));
    }

    void IntervalChimeWindow::destroyInterface()
    {
        erase();
        previousImage = nullptr;
        nextImage     = nullptr;
        title         = nullptr;
        text          = nullptr;
        minute        = nullptr;
    }

    void IntervalChimeWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        LOG_DEBUG("onBeforeShow: mode=%s", mode == ShowMode::GUI_SHOW_INIT ? "GUI_SHOW_INIT" : "GUI_SHOW_RETURN");

        MeditationWindow::onBeforeShow(mode, data);

        if (mode == ShowMode::GUI_SHOW_INIT) {
            updateDisplay();
        }
    }

    bool IntervalChimeWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(gui::KeyCode::KEY_RF)) {
            application->returnToPreviousWindow();
            return true;
        }

        if (inputEvent.isShortRelease(gui::KeyCode::KEY_LEFT) || inputEvent.isShortRelease(gui::KeyCode::KEY_UP)) {
            previousInterval();
            return true;
        }

        if (inputEvent.isShortRelease(gui::KeyCode::KEY_RIGHT) || inputEvent.isShortRelease(gui::KeyCode::KEY_DOWN)) {
            nextInterval();
            return true;
        }

        if (inputEvent.isShortRelease(gui::KeyCode::KEY_ENTER)) {
            gotoWindow(gui::name::window::ready_going);
            return true;
        }

        return MeditationWindow::onInput(inputEvent);
    }

    void IntervalChimeWindow::previousInterval()
    {
        LOG_DEBUG("previous");

        if (static_cast<uint32_t>(intervalType) > static_cast<uint32_t>(IntervalType::IntervalNone)) {
            intervalType = IntervalType(static_cast<uint32_t>(intervalType) - 1);
        }
        else {
            intervalType = IntervalType::Interval_15;
        }
        item.setInterval(intervalToSecs());
        updateDisplay();
    }

    void IntervalChimeWindow::nextInterval()
    {
        LOG_DEBUG("next");

        if (static_cast<uint32_t>(intervalType) < static_cast<uint32_t>(IntervalType::Interval_15)) {
            intervalType = IntervalType(static_cast<uint32_t>(intervalType) + 1);
        }
        else {
            intervalType = IntervalType::IntervalNone;
        }
        item.setInterval(intervalToSecs());
        updateDisplay();
    }

    void IntervalChimeWindow::updateDisplay()
    {
        text->setText(getIntervalString());
    }

    std::string IntervalChimeWindow::getIntervalString() const
    {
        if (intervalType == IntervalType::IntervalNone) {
            return utils::translate("app_meditation_bell_interval_none");
        }

        const std::string toReplace = "%0";
        std::string temp            = utils::translate("app_meditation_bell_interval_every_x_minutes");

        switch (intervalType) {
        case IntervalType::IntervalNone:
            break;
        case IntervalType::Interval_1:
            temp.replace(temp.find(toReplace), toReplace.size(), std::to_string(1));
            break;
        case IntervalType::Interval_2:
            temp.replace(temp.find(toReplace), toReplace.size(), std::to_string(2));
            break;
        case IntervalType::Interval_5:
            temp.replace(temp.find(toReplace), toReplace.size(), std::to_string(5));
            break;
        case IntervalType::Interval_10:
            temp.replace(temp.find(toReplace), toReplace.size(), std::to_string(10));
            break;
        case IntervalType::Interval_15:
            temp.replace(temp.find(toReplace), toReplace.size(), std::to_string(15));
            break;
        }
        return temp;
    }

    std::chrono::seconds IntervalChimeWindow::intervalToSecs() const noexcept
    {
        switch (intervalType) {
        case IntervalType::IntervalNone:
            return std::chrono::seconds::zero();
        case IntervalType::Interval_1:
            return std::chrono::seconds{60};
        case IntervalType::Interval_2:
            return std::chrono::seconds{2 * 60};
        case IntervalType::Interval_5:
            return std::chrono::seconds{5 * 60};
        case IntervalType::Interval_10:
            return std::chrono::seconds{10 * 60};
        case IntervalType::Interval_15:
            return std::chrono::seconds{15 * 60};
        }
        return std::chrono::seconds::zero();
    }
} // namespace gui
