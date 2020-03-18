/*
 * @file EnterNumberWindow.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 1 lip 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "EnterNumberWindow.hpp"

#include "../ApplicationCall.hpp"
#include "../data/CallAppStyle.hpp"
#include "../data/CallSwitchData.hpp"
#include "InputMode.hpp"
#include "UiCommonActions.hpp"
#include "i18/i18.hpp"
#include "service-appmgr/ApplicationManager.hpp"
#include "service-cellular/api/CellularServiceAPI.hpp"
#include <ContactRecord.hpp>
#include <application-phonebook/windows/PhonebookContact.hpp>

namespace gui
{
    using namespace callAppStyle;
    using namespace callAppStyle::enterNumberWindow;

    EnterNumberWindow::EnterNumberWindow(app::Application *app, std::string windowName) : AppWindow(app, windowName)
    {
        buildInterface();
    }

    void EnterNumberWindow::rebuild()
    {}

    void EnterNumberWindow::setNumberLabel(const std::string num)
    {
        auto app = dynamic_cast<app::ApplicationCall *>(application);
        if (app == nullptr) {
            LOG_ERROR("app != ApplicationCall");
            return;
        }
        app->setDisplayedNumber(num);

        numberLabel->setText(num);

        if (numberLabel->getText().length() == 0) {
            bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("common_back"));
            return;
        }
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("app_call_clear"));
    }

    void EnterNumberWindow::buildInterface()
    {

        AppWindow::buildInterface();

        bottomBar->setText(BottomBar::Side::LEFT, utils::localize.get("app_call_call"));
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("app_call_add"));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("app_call_clear"));

        topBar->setActive(TopBar::Elements::SIGNAL, true);
        topBar->setActive(TopBar::Elements::BATTERY, true);
        topBar->setActive(TopBar::Elements::TIME, true);

        numberLabel = new gui::Label(this, numberLabel::x, numberLabel::y, numberLabel::w, numberLabel::h);
        numberLabel->setPenWidth(numberLabel::borderW);
        numberLabel->setFont(style::window::font::largelight);
        numberLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
        numberLabel->setAlignement(
            gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_TOP));
        numberLabel->setDotsMode(true, false);

        newContactIcon                    = new gui::AddContactIcon(this, newContactIcon::x, newContactIcon::y);
        newContactIcon->activatedCallback = [=](gui::Item &item) { return addNewContact(); };
        setFocusItem(newContactIcon);
    }

    bool EnterNumberWindow::addNewContact()
    {
        auto app = dynamic_cast<app::ApplicationCall *>(application);
        if (app != nullptr) {
            auto phoneNumber = app->getDisplayedNumber();
            return app::contact(getApplication(), app::ContactOperation::Add, phoneNumber);
        }
        return false;
    }

    void EnterNumberWindow::destroyInterface()
    {
        AppWindow::destroyInterface();
        if (numberLabel) {
            removeWidget(numberLabel);
            delete numberLabel;
            numberLabel = nullptr;
        }
        if (newContactIcon) {
            removeWidget(newContactIcon);
            delete newContactIcon;
            newContactIcon = nullptr;
        }
        children.clear();
    }

    EnterNumberWindow::~EnterNumberWindow()
    {
        destroyInterface();
    }

    bool EnterNumberWindow::onInput(const InputEvent &inputEvent)
    {
        auto app = dynamic_cast<app::ApplicationCall *>(application);
        if (app == nullptr) {
            LOG_ERROR("app != ApplicationCall");
            return AppWindow::onInput(inputEvent);
        }
        auto code = translator.handle(inputEvent.key, InputMode({InputMode::phone}).get());
        if (inputEvent.state == InputEvent::State::keyReleasedShort) {
            if (inputEvent.keyCode == KeyCode::KEY_LF) {
                std::string num = app->getDisplayedNumber();
                LOG_INFO("number: [%s]", num.c_str());
                auto ret = CellularServiceAPI::DialNumber(application, num.c_str());
                LOG_INFO("CALL RESULT: %s", (ret ? "OK" : "FAIL"));
                return true;
            }
            else if (inputEvent.keyCode == KeyCode::KEY_RF) {
                std::string num = app->getDisplayedNumber();
                // if there isn't any char in phone number field return to previous application
                if (num.empty()) {
                    sapm::ApplicationManager::messageSwitchPreviousApplication(application);
                    return true;
                }

                num = num.substr(0, num.size() - 1);
                setNumberLabel(num);

                application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);

                return true;
            }
            else if (code != 0) {
                std::string num = app->getDisplayedNumber();
                num += code;
                setNumberLabel(num);

                app->refreshWindow(RefreshModes::GUI_REFRESH_FAST);

                return true;
            }
        }
        else if (inputEvent.state == InputEvent::State::keyReleasedLong) {
            // erase all characters from phone number
            if (inputEvent.keyCode == KeyCode::KEY_RF) {
                std::string num = app->getDisplayedNumber();
                // if there isn't any char in phone number field return to previous application
                if (num.empty()) {
                    sapm::ApplicationManager::messageSwitchPreviousApplication(application);
                    return true;
                }

                setNumberLabel("");

                application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);

                return true;
            }
            // long press of '0' key is translated to '+'
            else if (inputEvent.keyCode == KeyCode::KEY_0) {
                std::string num = app->getDisplayedNumber();
                num += '+';
                setNumberLabel(num);

                application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);

                return true;
            }
        }

        // check if any of the lower inheritance onInput methods catch the event
        return AppWindow::onInput(inputEvent);
    }

    bool EnterNumberWindow::handleSwitchData(SwitchData *data)
    {

        if (data == nullptr) {
            LOG_ERROR("Received null pointer");
            return false;
        }

        app::CallSwitchData *callData = dynamic_cast<app::CallSwitchData *>(data);
        if (callData != nullptr) {
            std::string num = callData->getPhoneNumber();
            setNumberLabel(num);
            application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
            switch (callData->getType()) {
            case app::CallSwitchData::Type::EXECUTE_CALL: {
                LOG_INFO("number: [%s]", num.c_str());
                auto ret = CellularServiceAPI::DialNumber(application, num.c_str());
                LOG_INFO("CALL RESULT: %s", (ret ? "OK" : "FAIL"));
                return true;
            }

            case app::CallSwitchData::Type::ENTER_NUMBER: {
                return true;
            } break;

            default: {
                LOG_ERROR("Unhandled callData type");
                return false;
            }
            }
        }
        else {
            LOG_ERROR("Unrecognized SwitchData");
            return false;
        }

        return false;
    }

} /* namespace gui */
