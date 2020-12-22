// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "application-settings-new/widgets/ScreenLockBoxSettings.hpp"
#include "application-settings-new/ApplicationSettings.hpp"
#include "ChangePasscodeWindow.hpp"
#include "application-desktop/widgets/PinHash.hpp"
#include "application-desktop/data/Style.hpp"
#include <module-services/service-db/agents/settings/SystemSettings.hpp>

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

        lockBox = std::make_unique<ScreenLockBoxSettings>(this);
        lockBox->buildLockBox(4U);

        lockImage->setVisible(false);
        infoImage->setVisible(false);
        pinLabelsBox->setPosition(248U, gui::Axis::Y);

        bottomBar->setActive(BottomBar::Side::LEFT, false);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));

        setVisibleState();
    }

    void ChangePasscodeWindow::destroyInterface()
    {
        erase();
        invalidate();
    }

    void ChangePasscodeWindow::invalidate() noexcept
    {
        lockBox       = nullptr;
        title         = nullptr;
        lockImage     = nullptr;
        infoImage     = nullptr;
        primaryText   = nullptr;
        secondaryText = nullptr;
    }

    void ChangePasscodeWindow::setVisibleState()
    {
        if (lock->isState(PinLock::LockState::PasscodeRequired) ||
            lock->isState(PinLock::LockState::PasscodeInvalidRetryRequired)) {
            primaryText->setText("Type current passcode");
            primaryText->setPosition(160U, gui::Axis::Y);
        }
        else if (lock->isState(PinLock::LockState::NewPasscodeRequired)) {
            primaryText->setText("Enter new passcode");
            primaryText->setPosition(160U, gui::Axis::Y);
        }
        else if (lock->isState(PinLock::LockState::NewPasscodeConfirmRequired)) {
            primaryText->setText("Confirm new passcode");
            primaryText->setPosition(160U, gui::Axis::Y);
        }
        else if (lock->isState(PinLock::LockState::Unlocked)) {
            infoImage->setVisible(true);
            secondaryText->setText("Passcode changed successfully");
            bottomBar->setActive(BottomBar::Side::CENTER, false);
            pinLabelsBox->setVisible(false);
        }

        if (lock->isState(PinLock::LockState::PasscodeInvalidRetryRequired) ||
            lock->isState(PinLock::LockState::NewPasscodeInvalid)) {
            secondaryText->setText("Wrong passcode!");
        }
    }

    auto ChangePasscodeWindow::onInput(const InputEvent &inputEvent) -> bool
    {
        if (lock->isState(PinLock::LockState::Unlocked) && inputEvent.isShortPress()) {
            application->returnToPreviousWindow(1);
        }
        if (!inputEvent.isShortPress()) {
            return AppWindow::onInput(inputEvent);
        }

        auto keyCodeNum = gui::toNumeric(inputEvent.keyCode);
        if (0 <= keyCodeNum && keyCodeNum <= 9 && lock->canPut()) {
            lockBox->putChar(lock->getCharCount());
            lock->putNextChar(keyCodeNum);
            return true;
        }
        else if (inputEvent.is(KeyCode::KEY_PND)) {
            lock->popChar();
            lockBox->popChar(lock->getCharCount());
            return true;
        }
        else if (inputEvent.is(KeyCode::KEY_ENTER)) {
            if (lock->isState(PinLock::LockState::PasscodeRequired)) {
                const auto callback = [this](PinLock::LockType, const std::vector<unsigned int> &pin) {
                    std::hash<std::vector<unsigned int>> hashEngine;
                    uint32_t hash = hashEngine(pin);
                    auto app      = static_cast<app::ApplicationSettingsNew *>(application);
                    if (hash == app->lockPassHash) {
                        lock->lockState = PinLock::LockState::NewPasscodeRequired;
                    }
                    else {
                        lock->lockState = PinLock::LockState::PasscodeInvalidRetryRequired;
                    }
                    rebuild();
                };
                lock->onActivatedCallback = callback;
            }
            else if (lock->isState(PinLock::LockState::NewPasscodeRequired)) {
                const auto callback = [this](PinLock::LockType, const std::vector<unsigned int> &pin) {
                    if (pin.size() < lock->getMaxPinSize()) {
                        lock->lockState = PinLock::LockState::PasscodeInvalidRetryRequired;
                        rebuild();
                        return;
                    }
                    std::hash<std::vector<unsigned int>> hashEngine;
                    newPasscodeHash = hashEngine(pin);
                    lock->lockState = PinLock::LockState::NewPasscodeConfirmRequired;
                    rebuild();
                };
                lock->onActivatedCallback = callback;
            }
            else if (lock->isState(PinLock::LockState::NewPasscodeConfirmRequired)) {
                const auto callback = [this](PinLock::LockType, const std::vector<unsigned int> &pin) {
                    std::hash<std::vector<unsigned int>> hashEngine;
                    const auto newPasscodeConfirmedHash = hashEngine(pin);
                    if (newPasscodeHash == newPasscodeConfirmedHash) {
                        lock->lockState = PinLock::LockState::Unlocked;
                        auto app        = static_cast<app::ApplicationSettingsNew *>(application);
                        app->updateLockPassHash(newPasscodeConfirmedHash);
                    }
                    else {
                        lock->lockState = PinLock::LockState::NewPasscodeInvalid;
                    }
                    rebuild();
                };
                lock->onActivatedCallback = callback;
            }
            lock->activate();
            return true;
        }
        return AppWindow::onInput(inputEvent);
    }
} // namespace gui
