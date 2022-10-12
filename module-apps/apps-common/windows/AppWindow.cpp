// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AppWindow.hpp"
#include "ApplicationCommon.hpp"
#include "InputEvent.hpp"
#include "StatusBar.hpp"
#include "status-bar/Time.hpp"
#include <Style.hpp>
#include <i18n/i18n.hpp>
#include <service-appmgr/Controller.hpp>
#include <service-audio/AudioServiceAPI.hpp>

using namespace style::header;

namespace gui
{

    AppWindow::AppWindow(app::ApplicationCommon *app, std::string name) : Window(name), application{app}
    {
        setSize(style::window_width, style::window_height);
    }

    void AppWindow::destroyInterface()
    {
        erase(statusBar);
        erase(header);
        erase(navBar);
        statusBar = nullptr;
        header    = nullptr;
        navBar    = nullptr;
    }

    void AppWindow::rebuild()
    {}

    void AppWindow::buildInterface()
    {
        auto config          = configureStatusBar(application->getStatusBarConfiguration());
        namespace status_bar = style::status_bar;
        statusBar            = new gui::status_bar::StatusBar(this,
                                                   status_bar::default_horizontal_pos,
                                                   status_bar::default_vertical_pos,
                                                   status_bar::width,
                                                   status_bar::height);
        statusBar->configure(std::move(config));

        header = new gui::header::Header(this,
                                         style::header::default_horizontal_pos,
                                         style::header::default_vertical_pos,
                                         style::header::width,
                                         style::header::height);

        navBar = new gui::nav_bar::NavBar(this,
                                          style::nav_bar::default_horizontal_pos,
                                          style::nav_bar::default_vertical_pos,
                                          style::nav_bar::width,
                                          style::nav_bar::height);
        navBar->setActive(nav_bar::Side::Left, false);
        navBar->setActive(nav_bar::Side::Center, false);
        navBar->setActive(nav_bar::Side::Right, false);
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

    bool AppWindow::updateBluetooth(sys::bluetooth::BluetoothMode mode)
    {
        if (statusBar == nullptr) {
            return false;
        }

        return statusBar->updateBluetooth(mode);
    }

    bool AppWindow::updateAlarmClock(bool status)
    {
        if (statusBar == nullptr) {
            return false;
        }
        return statusBar->updateAlarmClock(status);
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

    bool AppWindow::updateTethering(const sys::phone_modes::Tethering state)
    {
        if (statusBar == nullptr) {
            return false;
        }

        return statusBar->updateTetheringState(state);
    }

    bool AppWindow::preventsAutoLocking() const noexcept
    {
        return preventsAutoLock;
    }

    RefreshModes AppWindow::updateTime()
    {
        if (statusBar == nullptr) {
            return RefreshModes::GUI_REFRESH_NONE;
        }
        return statusBar->updateTime() ? RefreshModes::GUI_REFRESH_FAST : RefreshModes::GUI_REFRESH_NONE;
    }

    void AppWindow::setTitle(const UTF8 &text)
    {
        header->setTitle(text);
    }

    UTF8 AppWindow::getTitle()
    {
        return header->getTitle();
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

    void AppWindow::navBarTemporaryMode(const UTF8 &text, bool emptyOthers)
    {
        navBarTemporaryMode(text, nav_bar::Side::Left, emptyOthers);
    }

    void AppWindow::navBarTemporaryMode(const UTF8 &text, nav_bar::Side side, bool emptyOthers)
    {
        if (navBar == nullptr) {
            return;
        }

        navBar->store();

        if (emptyOthers) {
            navBar->setText(nav_bar::Side::Left, "");
            navBar->setText(nav_bar::Side::Center, "");
            navBar->setText(nav_bar::Side::Right, "");
        }

        switch (side) {
        case nav_bar::Side::Left:
            navBar->setText(nav_bar::Side::Left, text);
            break;
        case nav_bar::Side::Center:
            navBar->setText(nav_bar::Side::Center, text);
            break;
        case nav_bar::Side::Right:
            navBar->setText(nav_bar::Side::Right, text);
            break;
        }
        application->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
    }

    void AppWindow::navBarRestoreFromTemporaryMode()
    {
        if (navBar == nullptr) {
            return;
        }

        navBar->restore();
        application->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
    }

    void AppWindow::setNavBarText(const UTF8 &text, nav_bar::Side side)
    {
        navBar->setText(side, text);
    }

    void AppWindow::clearNavBarText(nav_bar::Side side)
    {
        navBar->setText(side, "");
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
                header->offset_h(),
                this->getWidth(),
                this->getHeight() - this->header->offset_h() - navBar->getHeight()};
    }

    void AppWindow::setNavBarActive(nav_bar::Side side, bool value)
    {
        navBar->setActive(side, value);
    }

    void AppWindow::accept(GuiVisitor &visitor)
    {
        visitor.visit(*this);
    }

    std::string AppWindow::getUniqueName()
    {
        constexpr auto separator = "/";
        return application->GetName() + separator + getName();
    }

} /* namespace gui */
