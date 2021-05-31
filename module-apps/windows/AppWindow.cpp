// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AppWindow.hpp"
#include "Application.hpp"
#include "InputEvent.hpp"
#include "StatusBar.hpp"
#include "status-bar/Time.hpp"
#include <Style.hpp>
#include <i18n/i18n.hpp>
#include <service-appmgr/Controller.hpp>
#include <service-audio/AudioServiceAPI.hpp>
#include <time/DateAndTimeSettings.hpp>

using namespace style::header;

namespace gui
{

    AppWindow::AppWindow(app::Application *app, std::string name) : Window(name), application{app}
    {
        setSize(style::window_width, style::window_height);
    }

    void AppWindow::destroyInterface()
    {
        erase(bottomBar);
        erase(statusBar);
        erase(title);
        title     = nullptr;
        bottomBar = nullptr;
        statusBar = nullptr;
    }

    void AppWindow::rebuild()
    {}

    void AppWindow::buildInterface()
    {
        bottomBar = new gui::BottomBar(this, 0, style::window_height - 51, style::window_width, 50);
        bottomBar->setActive(BottomBar::Side::LEFT, false);
        bottomBar->setActive(BottomBar::Side::CENTER, false);
        bottomBar->setActive(BottomBar::Side::RIGHT, false);

        title = new gui::Label(this, 0, 52, style::window_width, 52);
        title->setFilled(false);
        title->setFont(font::title);
        title->clear();
        title->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Top));
        title->setEdges(RectangleEdge::Bottom);
        title->setEllipsis(Ellipsis::Right);
        title->visible = false;

        auto config          = configureStatusBar(application->getStatusBarConfiguration());
        namespace status_bar = style::header::status_bar;
        statusBar            = new gui::status_bar::StatusBar(
            this, (style::window_width - status_bar::width) / 2, 0, status_bar::width, status_bar::height);
        statusBar->configure(std::move(config));
    }

    status_bar::Configuration AppWindow::configureStatusBar(status_bar::Configuration appConfiguration)
    {
        return appConfiguration;
    }

    void AppWindow::applyToStatusBar(StatusBarConfigurationChangeFunction configChange)
    {
        if (configChange) {
            auto newConfiguration = configChange(statusBar->getConfiguration());
            statusBar->configure(std::move(newConfiguration));
        }
    }

    bool AppWindow::updateSim()
    {
        if (statusBar == nullptr) {
            return false;
        }
        return statusBar->updateSim();
    }

    bool AppWindow::updateBatteryStatus()
    {
        if (statusBar == nullptr) {
            return false;
        }
        return statusBar->updateBattery();
    }
    // updates battery level in the window
    bool AppWindow::updateSignalStrength()
    {
        if (statusBar == nullptr) {
            return false;
        }
        return statusBar->updateSignalStrength();
    }

    bool AppWindow::updateNetworkAccessTechnology()
    {
        if (statusBar == nullptr) {
            return false;
        }
        return statusBar->updateNetworkAccessTechnology();
    }

    void AppWindow::updatePhoneMode(sys::phone_modes::PhoneMode mode)
    {
        auto fn = [&](gui::status_bar::Configuration cfg) -> gui::status_bar::Configuration {
            gui::status_bar::Configuration ret(cfg);
            ret.setPhoneMode(mode);
            return ret;
        };

        applyToStatusBar(std::move(fn));
    }

    bool AppWindow::preventsAutoLocking() const noexcept
    {
        return preventsAutoLock;
    }

    bool AppWindow::updateTime()
    {
        applyToStatusBar([](status_bar::Configuration configuration) {
            using TimeMode = gui::status_bar::TimeConfiguration::TimeMode;
            auto modifier  = std::make_shared<gui::status_bar::TimeConfiguration>(
                utils::dateAndTimeSettings.isTimeFormat12() ? TimeMode::Time12h : TimeMode::Time24h);
            configuration.setIndicatorModifier(gui::status_bar::Indicator::Time, std::move(modifier));
            return configuration;
        });
        if (statusBar == nullptr) {
            return false;
        }
        return statusBar->updateTime();
    }

    void AppWindow::setTitle(const UTF8 &text)
    {
        if (title != nullptr) {
            title->setText(text);
            title->setVisible(text.length() != 0);
        }
        else {
            LOG_ERROR("cant set title - it doesn't exist!");
        }
    }

    bool AppWindow::onDatabaseMessage(sys::Message *msg)
    {
        return false;
    }

    bool AppWindow::onInput(const InputEvent &inputEvent)
    {
        // check if any of the lower inheritance onInput methods catch the event
        if (Window::onInput(inputEvent)) {
            return true;
        }

        if (inputEvent.isLongRelease(gui::KeyCode::KEY_RF)) {
            LOG_INFO("exit to main menu");
            app::manager::Controller::sendAction(application, app::manager::actions::Home);
        }

        if (inputEvent.isLongRelease(gui::KeyCode::KEY_PND)) {
            LOG_INFO("Locking phone");

            application->getPhoneLockSubject().lock();
        }

        if ((inputEvent.isShortRelease())) {
            switch (inputEvent.getKeyCode()) {
            case KeyCode::HEADSET_VOLUP:
                [[fallthrough]];
            case KeyCode::KEY_VOLUP: {
                return application->increaseCurrentVolume();
            }
            case KeyCode::HEADSET_VOLDN:
                [[fallthrough]];
            case KeyCode::KEY_VOLDN: {
                return application->decreaseCurrentVolume();
            }
            case KeyCode::KEY_RF: {
                application->returnToPreviousWindow();
                return true;
            }
            default:
                break;
            }
        }

        if (inputEvent.is(KeyCode::KEY_TORCH)) {
            if (inputEvent.isLongRelease()) {
                application->toggleTorchOnOff();
                return true;
            }
            else if (inputEvent.isShortRelease()) {
                application->toggleTorchColor();
                return true;
            }
        }

        return false;
    }

    void AppWindow::bottomBarTemporaryMode(const UTF8 &text, bool emptyOthers)
    {
        bottomBarTemporaryMode(text, BottomBar::Side::LEFT, emptyOthers);
    }

    void AppWindow::bottomBarTemporaryMode(const UTF8 &text, BottomBar::Side side, bool emptyOthers)
    {
        if (bottomBar == nullptr) {
            return;
        }

        bottomBar->store();

        if (emptyOthers) {
            bottomBar->setText(BottomBar::Side::LEFT, "");
            bottomBar->setText(BottomBar::Side::CENTER, "");
            bottomBar->setText(BottomBar::Side::RIGHT, "");
        }

        switch (side) {
        case BottomBar::Side::LEFT:
            bottomBar->setText(BottomBar::Side::LEFT, text);
            break;
        case BottomBar::Side::CENTER:
            bottomBar->setText(BottomBar::Side::CENTER, text);
            break;
        case BottomBar::Side::RIGHT:
            bottomBar->setText(BottomBar::Side::RIGHT, text);
            break;
        }
        application->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
    }

    void AppWindow::bottomBarRestoreFromTemporaryMode()
    {
        if (bottomBar == nullptr) {
            return;
        }

        bottomBar->restore();
        application->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
    }

    void AppWindow::setBottomBarText(const UTF8 &text, BottomBar::Side side)
    {
        bottomBar->setText(side, text);
    }

    void AppWindow::clearBottomBarText(BottomBar::Side side)
    {
        bottomBar->setText(side, "");
    }

    bool AppWindow::selectSpecialCharacter()
    {
        application->clearLongPressTimeout();
        return app::manager::Controller::sendAction(
            application,
            app::manager::actions::ShowSpecialInput,
            std::make_unique<gui::SwitchSpecialChar>(gui::SwitchSpecialChar::Type::Request, application->GetName()),
            app::manager::OnSwitchBehaviour::RunInBackground);
    }

    BoundingBox AppWindow::bodySize()
    {
        return {0,
                title->offset_h(),
                this->getWidth(),
                this->getHeight() - this->title->offset_h() - bottomBar->getHeight()};
    }

    void AppWindow::setBottomBarActive(BottomBar::Side side, bool value)
    {
        bottomBar->setActive(side, value);
    }

    void AppWindow::accept(GuiVisitor &visitor)
    {
        visitor.visit(*this);
    }

} /* namespace gui */
