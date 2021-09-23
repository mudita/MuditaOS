// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "IntervalChimeWindow.hpp"
#include "ReadyGoingWindow.hpp"

#include "log.hpp"
#include <i18n/i18n.hpp>

namespace gui
{
    IntervalChimeWindow::IntervalChimeWindow(
        app::ApplicationCommon *app,
        std::unique_ptr<app::meditation::IntervalChimeContract::Presenter> &&windowPresenter)
        : MeditationWindow(app, gui::name::window::interval_chime), presenter{std::move(windowPresenter)}
    {
        buildInterface();
        presenter->attach(this);
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
        MeditationWindow::onBeforeShow(mode, data);

        if (mode == ShowMode::GUI_SHOW_INIT) {
            updateDisplay();
        }
    }

    bool IntervalChimeWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(gui::KeyCode::KEY_LEFT) || inputEvent.isShortRelease(gui::KeyCode::KEY_UP)) {
            presenter->decrease();
            return true;
        }

        if (inputEvent.isShortRelease(gui::KeyCode::KEY_RIGHT) || inputEvent.isShortRelease(gui::KeyCode::KEY_DOWN)) {
            presenter->increase();
            return true;
        }

        if (inputEvent.isShortRelease(gui::KeyCode::KEY_ENTER)) {
            gotoWindow(gui::name::window::ready_going);
            return true;
        }

        return MeditationWindow::onInput(inputEvent);
    }

    void IntervalChimeWindow::updateDisplay()
    {
        text->setText(presenter->getIntervalString());
    }

    void IntervalChimeWindow::buildMeditationItem(MeditationItem &item)
    {
        presenter->request(item);
    }

    void IntervalChimeWindow::onMeditationItemAvailable(MeditationItem *item)
    {
        if (item != nullptr) {
            presenter->activate(*item);
        }
    }
} // namespace gui
