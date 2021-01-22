// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "EnterNumberWindow.hpp"

#include "../ApplicationCall.hpp"
#include "../data/CallAppStyle.hpp"
#include "../data/CallSwitchData.hpp"

#include <ContactRecord.hpp>
#include <country.hpp>
#include <i18n/i18n.hpp>
#include <InputMode.hpp>
#include <service-appmgr/Controller.hpp>
#include <service-cellular/CellularServiceAPI.hpp>

#include <phonenumbers/phonenumberutil.h>
#include <phonenumbers/asyoutypeformatter.h>

#include <cassert>

using namespace utils;

namespace gui
{
    using namespace callAppStyle;
    using namespace callAppStyle::enterNumberWindow;

    EnterNumberWindow::EnterNumberWindow(app::Application *app,
                                         app::EnterNumberWindowInterface *interface,
                                         std::string windowName)
        : AppWindow(app, windowName), interface(interface), currentCountry(defaultCountry),
          numberUtil(*PhoneNumberUtil::GetInstance())
    {
        assert(interface != nullptr);
        assert(app != nullptr);
        switchFormatter(country::getAlpha2Code(currentCountry));
        buildInterface();
    }

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
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("common_add"));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("app_call_clear"));

        numberLabel = new gui::Label(this, numberLabel::x, numberLabel::y, numberLabel::w, numberLabel::h);
        numberLabel->setPenWidth(numberLabel::borderW);
        numberLabel->setFont(style::window::font::largelight);
        numberLabel->setEdges(RectangleEdge::Bottom);
        numberLabel->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Top));
        numberLabel->setEllipsis(Ellipsis::Left);

        newContactIcon                    = new gui::AddContactIcon(this, newContactIcon::x, newContactIcon::y);
        newContactIcon->activatedCallback = [=](gui::Item &item) { return addNewContact(); };
        setFocusItem(newContactIcon);
    }

    bool EnterNumberWindow::addNewContact()
    {
        interface->handleAddContactEvent(enteredNumber);
        return true;
    }

    void EnterNumberWindow::destroyInterface()
    {
        erase();
    }

    bool EnterNumberWindow::onInput(const InputEvent &inputEvent)
    {
        auto code = translator.handle(inputEvent.key, InputMode({InputMode::phone}).get());
        if (inputEvent.state == InputEvent::State::keyReleasedShort) {
            // Call function
            if (inputEvent.keyCode == KeyCode::KEY_LF) {
                interface->handleCallEvent(enteredNumber);
                return true;
            }
            // Clear/back function
            else if (inputEvent.keyCode == KeyCode::KEY_RF) {
                // if there isn't any char in phone number field return to previous application
                if (enteredNumber.empty()) {
                    formatter->Clear();
                    app::manager::Controller::switchBack(application);
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
                    app::manager::Controller::switchBack(application);
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
                interface->handleCallEvent(phoneNumber.getEntered());
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
