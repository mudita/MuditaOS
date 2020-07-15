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

#include <application-phonebook/windows/PhonebookContact.hpp>
#include <ContactRecord.hpp>
#include <country.hpp>
#include <i18/i18.hpp>
#include <InputMode.hpp>
#include <service-appmgr/ApplicationManager.hpp>
#include <service-cellular/api/CellularServiceAPI.hpp>
#include <UiCommonActions.hpp>

#include <phonenumbers/phonenumberutil.h>
#include <phonenumbers/asyoutypeformatter.h>

using namespace utils;

namespace gui
{
    using namespace callAppStyle;
    using namespace callAppStyle::enterNumberWindow;

    EnterNumberWindow::EnterNumberWindow(app::Application *app, std::string windowName)
        : AppWindow(app, windowName), currentCountry(defaultCountry), numberUtil(*PhoneNumberUtil::GetInstance())
    {
        switchFormatter(country::getAlpha2Code(currentCountry));
        buildInterface();
    }

    void EnterNumberWindow::rebuild()
    {}

    void EnterNumberWindow::setNumberLabel(const std::string num)
    {
        numberLabel->setText(num);

        if (numberLabel->getText().length() == 0) {
            bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));
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
        numberLabel->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Top));
        numberLabel->setEllipsis(Ellipsis::Left);

        newContactIcon                    = new gui::AddContactIcon(this, newContactIcon::x, newContactIcon::y);
        newContactIcon->activatedCallback = [=](gui::Item &item) { return addNewContact(); };
        setFocusItem(newContactIcon);
    }

    bool EnterNumberWindow::addNewContact()
    {
        auto app = dynamic_cast<app::ApplicationCall *>(application);
        if (app != nullptr) {
            app->handleAddContactEvent(enteredNumber);
        }
        return false;
    }

    void EnterNumberWindow::destroyInterface()
    {
        erase();
    }

    EnterNumberWindow::~EnterNumberWindow()
    {
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
            // Call function
            if (inputEvent.keyCode == KeyCode::KEY_LF) {
                app->handleCallEvent(enteredNumber);
                return true;
            }
            // Clear/back function
            else if (inputEvent.keyCode == KeyCode::KEY_RF) {
                // if there isn't any char in phone number field return to previous application
                if (enteredNumber.empty()) {
                    formatter->Clear();
                    sapm::ApplicationManager::messageSwitchPreviousApplication(application);
                }
                // if there is the last char just clear input
                else if (enteredNumber.size() == 1) {
                    clearInput();
                }
                else {
                    // remove last digit and reformat entered number otherwise
                    enteredNumber = enteredNumber.substr(0, enteredNumber.size() - 1);
                    initFormatterInput(enteredNumber);
                    setNumberLabel(formattedNumber);

                    application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
                }
                return true;
            }
            else if (code != 0) {
                addDigit(code);
                return true;
            }
        }
        else if (inputEvent.state == InputEvent::State::keyReleasedLong) {
            // erase all characters from phone number
            if (inputEvent.keyCode == KeyCode::KEY_RF) {
                // if there isn't any char in phone number field return to previous application
                if (enteredNumber.empty()) {
                    sapm::ApplicationManager::messageSwitchPreviousApplication(application);
                    return true;
                }

                clearInput();

                return true;
            }
            // long press of '0' key is translated to '+'
            else if (inputEvent.keyCode == KeyCode::KEY_0) {
                addDigit('+');
                return true;
            }
        }

        // check if any of the lower inheritance onInput methods catch the event
        return AppWindow::onInput(inputEvent);
    }

    bool EnterNumberWindow::handleSwitchData(SwitchData *data)
    {
        auto app = dynamic_cast<app::ApplicationCall *>(application);

        if (data == nullptr) {
            LOG_ERROR("Received null pointer");
            return false;
        }

        if (data->getDescription() == app::EnterNumberData::descriptionStr) {
            auto *callData = dynamic_cast<app::EnterNumberData *>(data);
            assert(callData != nullptr);

            initFormatterInput(callData->getPhoneNumber());
            setNumberLabel(formattedNumber);
            application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
        }
        else if (data->getDescription() == app::CallSwitchData::descriptionStr) {
            auto *callData = dynamic_cast<app::CallSwitchData *>(data);
            assert(callData != nullptr);

            auto &phoneNumber = callData->getPhoneNumber();

            initFormatterInput(phoneNumber.getEntered());
            setNumberLabel(phoneNumber.getFormatted());
            application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);

            if (callData->getType() == app::CallSwitchData::Type::EXECUTE_CALL) {
                app->handleCallEvent(phoneNumber.getEntered());
            }
        }
        else {
            LOG_ERROR("Unhandled switch data");
            abort();
        }

        return true;
    }

    void EnterNumberWindow::switchFormatter(const std::string &regionCode)
    {
        auto newFormatter = std::unique_ptr<Formatter>(numberUtil.GetAsYouTypeFormatter(regionCode));
        formatter.swap(newFormatter);
        LOG_INFO("Switched formatter to region: %s", regionCode.c_str());
    }

    void EnterNumberWindow::initFormatterInput(const std::string &number)
    {
        enteredNumber = number;
        formatter->Clear();
        for (auto c : number) {
            formatter->InputDigit(c, &formattedNumber);
        }
    }

    void EnterNumberWindow::addDigit(const std::string::value_type &digit)
    {
        enteredNumber += digit;
        formatter->InputDigit(digit, &formattedNumber);
        setNumberLabel(formattedNumber);
        application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
    }

    const std::string &EnterNumberWindow::getEnteredNumber() const noexcept
    {
        return enteredNumber;
    }

    void EnterNumberWindow::clearInput()
    {
        enteredNumber.clear();
        formattedNumber.clear();
        formatter->Clear();
        setNumberLabel("");
        application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
    }
} /* namespace gui */
