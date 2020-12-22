// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

// application manager
#include "InputEvent.hpp"
#include "service-appmgr/Controller.hpp"

// module-gui
#include "gui/widgets/BottomBar.hpp"
#include "PinLockWindow.hpp"

#include "application-desktop/ApplicationDesktop.hpp"
#include "application-desktop/data/LockPhoneData.hpp"
#include "application-desktop/widgets/ScreenLockBoxDesktop.hpp"
#include "SimLockBox.hpp"
#include "PukLockBox.hpp"
#include <application-phonebook/ApplicationPhonebook.hpp>
#include "application-desktop/data/AppDesktopStyle.hpp"

namespace lock_style = style::window::pin_lock;
namespace gui
{

    PinLockWindow::PinLockWindow(app::Application *app, const std::string &window_name)
        : PinLockBaseWindow(app, window_name), this_window_name(window_name)
    {
        buildInterface();
    }

    void PinLockWindow::rebuild()
    {
        // find which widget has focus
        destroyInterface();
        buildInterface();
        // set state
        focusItem = nullptr;
    }
    void PinLockWindow::buildInterface()
    {
        AppWindow::buildInterface();
        PinLockBaseWindow::build();
    }

    void PinLockWindow::destroyInterface()
    {
        erase();
        invalidate();
    }

    void PinLockWindow::invalidate() noexcept
    {
        PinLockBaseWindow::invalidate();
        LockBox      = nullptr;
        pinLabelsBox = nullptr;
    }

    void PinLockWindow::buildImages(const std::string &lockImg, const std::string &infoImg)
    {
        lockImage = new gui::Image(this, lock_style::image::x, lock_style::image::y, 0, 0, lockImg);
        infoImage = new gui::Image(this, lock_style::image::x, lock_style::image::y, 0, 0, infoImg);
    }

    void PinLockWindow::setImagesVisible(bool lockImg, bool infoImg)
    {
        lockImage->setVisible(lockImg);
        infoImage->setVisible(infoImg);
    }

    void PinLockWindow::setTitleBar(bool isVisible, bool isIceActive)
    {
        iceBox->setVisible(isIceActive);
        title->setVisible(isVisible);
        if (isVisible) {
            title->setEdges(RectangleEdge::Bottom);
        }
        else {
            title->clear();
            title->setEdges(RectangleEdge::None);
        }
    }

    auto PinLockWindow::getToken(Token token) const -> std::string
    {
        if (token == Token::Sim) {
            return "$SIM";
        }
        else if (token == Token::Attempts) {
            return "$ATTEMPTS";
        }
        else if (token == Token::Mins) {
            return "$MINUTES";
        }
        else if (token == Token::CmeCode) {
            return "$CMECODE";
        }
        return std::string{};
    }

    void PinLockWindow::restore() noexcept
    {
        PinLockBaseWindow::restore();
        lockImage->setVisible(false);
        infoImage->setVisible(false);
    }

    void PinLockWindow::buildBottomBar()
    {
        setBottomBarWidgetsActive(false, false, false);
        bottomBar->setText(BottomBar::Side::LEFT, utils::localize.get("app_desktop_emergency"));
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::confirm));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));
    }

    void PinLockWindow::buildTopBar()
    {
        topBar->setActive(TopBar::Elements::SIGNAL, true);
        topBar->setActive(TopBar::Elements::BATTERY, true);
        topBar->setActive(TopBar::Elements::LOCK, true);
    }

    void PinLockWindow::buildTitleBar()
    {
        using namespace style::window::pin_lock;

        iceBox = new gui::HBox(this, ice::x, ice::y, ice::w, ice::h);
        iceBox->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
        iceBox->setEdges(RectangleEdge::None);
        iceBox->setVisible(false);

        auto arrow        = new gui::Image("left_label_arrow");
        arrow->activeItem = false;
        arrow->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
        arrow->setMargins(Margins(0, 0, ice::margin, 0));
        iceBox->addWidget(arrow);

        auto iceText        = new gui::Text(nullptr, 0, 0, ice::text::w, ice::h);
        iceText->activeItem = false;
        iceText->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
        iceText->setFont(style::window::font::verysmall);
        iceText->setText(utils::localize.get("app_desktop_emergency"));
        iceBox->addWidget(iceText);

        title = new gui::Text(this, title::x, title::y, title::w, title::h);
        title->setFilled(false);
        title->setBorderColor(gui::ColorFullBlack);
        title->setFont(style::header::font::title);
        title->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        title->setVisible(false);
        title->setPenWidth(2);
    }

    void PinLockWindow::setVisibleState()
    {
        restore();
        if (lock->isState(PinLock::LockState::PasscodeRequired)) {
            LockBox->setVisibleStateEnterPin(PinLockBox::EnterPasscodeType::ProvidePasscode);
        }
        else if (lock->isState(PinLock::LockState::PasscodeInvalidRetryRequired)) {
            LockBox->setVisibleStateInvalidPin(PinLockBox::PasscodeErrorType::InvalidPasscode, lock->getValue());
        }
        else if (lock->isState(PinLock::LockState::Blocked)) {
            LockBox->setVisibleStateBlocked();
        }
        else if (lock->isState(PinLock::LockState::NewPasscodeRequired)) {
            LockBox->setVisibleStateEnterPin(PinLockBox::EnterPasscodeType::ProvideNewPasscode);
        }
        else if (lock->isState(PinLock::LockState::NewPasscodeConfirmRequired)) {
            LockBox->setVisibleStateEnterPin(PinLockBox::EnterPasscodeType::ConfirmNewPasscode);
        }
        else if (lock->isState(PinLock::LockState::NewPasscodeInvalid)) {
            LockBox->setVisibleStateInvalidPin(PinLockBox::PasscodeErrorType::NewPasscodeConfirmFailed,
                                               lock->getValue());
        }
        else if (lock->isState(PinLock::LockState::ErrorOccurred)) {
            LockBox->setVisibleStateInvalidPin(PinLockBox::PasscodeErrorType::UnhandledError, lock->getValue());
        }
        application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
    }

    void PinLockWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (auto lockData = dynamic_cast<LockPhoneData *>(data)) {
            rebuild();
            lockTimeoutApplication = lockData->getPreviousApplication();
            lock                   = lockData->getLock();
            assert(lock);

            buildPinLockBox();
            LockBox->buildLockBox(lock->getMaxPinSize());

            if (lock->isState(PinLock::LockState::PasscodeRequired)) {
                currentPasscodeType = PinLockBox::EnterPasscodeType::ProvidePasscode;
            }
            else if (lock->isState(PinLock::LockState::NewPasscodeRequired)) {
                currentPasscodeType = PinLockBox::EnterPasscodeType::ProvideNewPasscode;
            }
            setVisibleState();
        }
    }

    bool PinLockWindow::onInput(const InputEvent &inputEvent)
    {
        if (!inputEvent.isShortPress()) {
            return AppWindow::onInput(inputEvent);
        }
        // accept only LF, enter, RF, #, and numeric values;
        if (inputEvent.is(KeyCode::KEY_LEFT) && iceBox->visible) {
            app::manager::Controller::sendAction(application, app::manager::actions::ShowEmergencyContacts);
            return true;
        }
        else if (inputEvent.is(KeyCode::KEY_LF) && bottomBar->isActive(BottomBar::Side::LEFT)) {
            app::manager::Controller::sendAction(application, app::manager::actions::ShowEmergencyContacts);
            return true;
        }
        else if (inputEvent.is(KeyCode::KEY_RF) && bottomBar->isActive(BottomBar::Side::RIGHT)) {
            if (usesNumericKeys()) {
                lock->clearAttempt();
            }
            else if (lock->isState(PinLock::LockState::PasscodeInvalidRetryRequired)) {
                lock->consumeState();
            }
            application->switchWindow(gui::name::window::main_window);
            return true;
        }
        else if (inputEvent.is(KeyCode::KEY_PND)) {
            if (usesNumericKeys()) {
                lock->popChar();
                LockBox->popChar(lock->getCharCount());
                bottomBar->setActive(BottomBar::Side::CENTER, lock->canVerify());
                return true;
            }
        }
        else if (0 <= gui::toNumeric(inputEvent.keyCode) && gui::toNumeric(inputEvent.keyCode) <= 9) {
            if (usesNumericKeys() && lock->canPut()) {
                LockBox->putChar(lock->getCharCount());
                lock->putNextChar(gui::toNumeric(inputEvent.keyCode));
                bottomBar->setActive(BottomBar::Side::CENTER, lock->canVerify());
                return true;
            }
        }
        else if (inputEvent.is(KeyCode::KEY_ENTER) && bottomBar->isActive(BottomBar::Side::CENTER)) {
            lock->activate();
            bottomBar->setActive(BottomBar::Side::CENTER, false);
            return true;
        }
        // check if any of the lower inheritance onInput methods catch the event
        return AppWindow::onInput(inputEvent);
    }

    void PinLockWindow::buildPinLockBox()
    {
        auto lockType = lock->getLockType();
        if (lockType == PinLock::LockType::Screen) {
            LockBox = std::make_unique<ScreenLockBoxDesktop>(this);
        }
        else if (lockType == PinLock::LockType::SimPuk) {
            LockBox = std::make_unique<PukLockBox>(this);
            setTitleBar(true, true);
            setText("app_desktop_header_sim_setup", TextType::Title, true, {{getToken(Token::Sim), "SIM1"}});
        }
        else if (lockType == PinLock::LockType::SimPin) {
            LockBox = std::make_unique<SimLockBox>(this);
        }
        assert(LockBox != nullptr);
    }

    auto PinLockWindow::usesNumericKeys() const noexcept -> bool
    {
        return lock->isState(PinLock::LockState::PasscodeRequired) ||
               lock->isState(PinLock::LockState::NewPasscodeRequired) ||
               lock->isState(PinLock::LockState::NewPasscodeConfirmRequired);
    }
} /* namespace gui */
