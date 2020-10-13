#include "MeditationTimerWindow.hpp"
#include "application-meditation/data/Style.hpp"
#include "Names.hpp"

#include "application-meditation/data/MeditationTimerData.hpp"
#include <cassert>
#include <i18/i18.hpp>

#include "RichTextParser.hpp"
#include "FontManager.hpp"

using namespace gui;

namespace meditationStyle::endWindow::text
{
    constexpr uint32_t x = style::window::default_left_margin;
    constexpr uint32_t y = 0;
    constexpr uint32_t w = style::window::default_body_width;
    constexpr uint32_t h = style::window_height;
} // namespace meditationStyle::endWindow::text

MeditationTimerWindow::MeditationTimerWindow(app::Application *app) : AppWindow{app, name::window::main_window}
{
    MeditationTimerWindow::buildInterface();
}

void MeditationTimerWindow::rebuild()
{
    destroyInterface();
    buildInterface();
}

void MeditationTimerWindow::buildInterface()
{
    auto app = dynamic_cast<app::ApplicationMeditation *>(application);
    assert(app != nullptr); // Pre-condition check.

    AppWindow::buildInterface();
    setTitle(utils::localize.get("app_meditation_title_main"));

    bottomBar->setText(BottomBar::Side::RIGHT, "stop");

    timer = new MeditationTimer(style::meditation::timer::X,
                                style::meditation::timer::Y,
                                style::meditation::timer::Width,
                                style::meditation::timer::Height,
                                app,
                                this);

    namespace textStyle = meditationStyle::endWindow::text;
    meditationInfo      = new Text(this, textStyle::x, textStyle::y, textStyle::w, textStyle::h);
    meditationInfo->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));

    auto onPreparation = [&]() -> void {
        setVisibleRunning();
        auto onMeditationEnd = [&]() -> void {
            setVisibleMeditationEnd();
            application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
            finished = true;
        };
        timer->registerTimeoutCallback(onMeditationEnd);
        timer->reset(std::chrono::seconds(meditationTimeInSeconds));
        timer->start();
        application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
    };

    timer->registerTimeoutCallback(onPreparation);
}

void MeditationTimerWindow::destroyInterface()
{
    erase();
    timer = nullptr;
}

void MeditationTimerWindow::onBeforeShow(ShowMode mode, SwitchData *data)
{
    auto timerData = reinterpret_cast<MeditationTimerData *>(data);
    assert(timerData);
    meditationTimeInSeconds = timerData->getMeditationTimeInSeconds();
    setVisiblePreparation();
    timer->setTimerVisible(timerData->getShowCounter());
    timer->reset(std::chrono::seconds(timerData->getPreparationTimeInSeconds()));
    timer->start();
}
#include <iostream>
auto MeditationTimerWindow::onInput(const InputEvent &inputEvent) -> bool
{
    std::cout << "HERE I WORk onInput" << std::endl;
    if (inputEvent.isShortPress()) {
        if (finished) {
            std::cout << "HERE I WORk onInput finished" << std::endl;
            application->switchWindow(app::window::name::meditation_main_window);
            return true;
        }
        if (inputEvent.is(KeyCode::KEY_RF) && bottomBar->isActive(BottomBar::Side::RIGHT)) {
            timer->stop();
            finished = true;
            setVisibleMeditationEnd();
            return true;
        }
        if (inputEvent.is(KeyCode::KEY_ENTER) && bottomBar->isActive(BottomBar::Side::CENTER)) {
            if (timer->running()) {
                timer->stop();
                bottomBar->setText(BottomBar::Side::CENTER, "start");
                setWidgetVisible(true, true, true);
            }
            else {
                timer->start();
                bottomBar->setText(BottomBar::Side::CENTER, "pause");
                setWidgetVisible(false, true, true);
            }
            return true;
        }
    }

    return AppWindow::onInput(inputEvent);
}

void MeditationTimerWindow::setWidgetVisible(bool tBar, bool bBar, bool counter)
{
    topBar->setActive(TopBar::Elements::TIME, tBar);
    title->setVisible(tBar);
    bottomBar->setActive(BottomBar::Side::CENTER, bBar);
    bottomBar->setActive(BottomBar::Side::LEFT, bBar);
    timer->setVisible(counter);
    //    timer->setTimerVisible(counter);
}
void MeditationTimerWindow::setVisibleRunning()
{
    setWidgetVisible(false, true, true);
    bottomBar->setText(BottomBar::Side::CENTER, "pause");
    meditationInfo->setVisible(false);
}
void MeditationTimerWindow::setVisiblePaused()
{
    setWidgetVisible(true, true, true);
    bottomBar->setText(BottomBar::Side::CENTER, "resume");
}
void MeditationTimerWindow::setVisiblePreparation()
{
    setWidgetVisible(false, false, false);
    TextFormat format(FontManager::getInstance().getFont(style::window::font::biglight));
    gui::text::RichTextParser parser;
    auto textParsed = parser.parse(utils::localize.get("app_meditation_put_down_phone_and_wait"), &format);
    meditationInfo->setText(std::move(textParsed));
    meditationInfo->setVisible(true);
}

void MeditationTimerWindow::setVisibleMeditationEnd()
{
    setWidgetVisible(false, false, false);
    TextFormat format(FontManager::getInstance().getFont(style::window::font::biglight));
    gui::text::RichTextParser parser;
    auto textParsed = parser.parse(utils::localize.get("app_meditation_thank_you_for_session"), &format);
    meditationInfo->setText(std::move(textParsed));
    meditationInfo->setVisible(true);
}
