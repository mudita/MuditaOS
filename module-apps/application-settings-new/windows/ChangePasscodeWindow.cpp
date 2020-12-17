// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "application-desktop/windows/ScreenLockBox.hpp"
#include "application-settings-new/ApplicationSettings.hpp"
#include "ChangePasscodeWindow.hpp"

namespace gui
{
    ChangePasscodeWindow::ChangePasscodeWindow(app::Application *app)
        : PinLockBaseWindow(app, gui::window::name::change_passcode)
    {
        lock = std::make_unique<gui::PinLock>(
            Store::GSM::SIM::NONE, PinLock::LockState::PasscodeRequired, PinLock::LockType::Screen, 4);
        buildInterface();
    }

    void ChangePasscodeWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    void ChangePasscodeWindow::buildInterface()
    {
        AppWindow::buildInterface();
        PinLockBaseWindow::build();
        setTitle(utils::localize.get("app_settings_security_change_passcode"));

        topBar->setActive(TopBar::Elements::SIM, false);
        topBar->setActive(TopBar::Elements::LOCK, false);
        topBar->setActive(TopBar::Elements::BATTERY, false);
        topBar->setActive(TopBar::Elements::TIME, true);

        lockBox = std::make_unique<ScreenLockBox>(this);
        lockBox->buildLockBox(4U);
        lockImage->setVisible(false);
        infoImage->setVisible(false);
        pinLabelsBox->setPosition(248U, gui::Axis::Y);

        bottomBar->setActive(BottomBar::Side::LEFT, false);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));

        if (lock->isState(PinLock::LockState::PasscodeRequired)) {
            primaryText->setText("Type current passcode");
            primaryText->setPosition(160U, gui::Axis::Y);
        }
        else if (lock->isState(PinLock::LockState::NewPasscodeRequired)) {
            primaryText->setText("Enter new passcode");
            primaryText->setPosition(160U, gui::Axis::Y);
        }
    }

    auto ChangePasscodeWindow::onInput(const InputEvent &inputEvent) -> bool
    {
        if (AppWindow::onInput(inputEvent)) {
            return true;
        }

        if (!inputEvent.isShortPress()) {
            return false;
        }

        if (inputEvent.is(KeyCode::KEY_ENTER)) {
            primaryText->setText("Enter new passcode");
            primaryText->setPosition(160U, gui::Axis::Y);
            return true;
        }
        return false;
    }
} // namespace gui
