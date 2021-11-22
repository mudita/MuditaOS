// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CallAppStyle.hpp"
#include "CallSwitchData.hpp"
#include "EnterNumberWindow.hpp"

#include <ContactRecord.hpp>
#include <country.hpp>
#include <i18n/i18n.hpp>
#include <text/modes/InputMode.hpp>
#include <phonenumbers/asyoutypeformatter.h>
#include <phonenumbers/phonenumberutil.h>
#include <service-appmgr/Controller.hpp>
#include <service-cellular/CellularServiceAPI.hpp>

#include <cassert>

namespace gui
{
    EnterNumberWindow::EnterNumberWindow(app::ApplicationCommon *app,
                                         app::EnterNumberWindowInterface *interface,
                                         std::string windowName)
        : NumberWindow(app, interface, std::move(windowName))
    {
        buildInterface();
    }

    void EnterNumberWindow::buildInterface()
    {
        using namespace callAppStyle;
        using namespace callAppStyle::enterNumberWindow;
        NumberWindow::buildInterface();

        navBar->setText(nav_bar::Side::Center, utils::translate("common_add"));
        navBar->setText(nav_bar::Side::Right, utils::translate("app_call_clear"));

        newContactIcon                    = new gui::AddContactIcon(this, newContactIcon::x, newContactIcon::y);
        newContactIcon->activatedCallback = [=](gui::Item &item) { return addNewContact(); };
        setFocusItem(newContactIcon);
    }

    status_bar::Configuration EnterNumberWindow::configureStatusBar(status_bar::Configuration appConfiguration)
    {
        appConfiguration.enable(status_bar::Indicator::PhoneMode);
        return appConfiguration;
    }

    bool EnterNumberWindow::addNewContact()
    {
        interface->handleAddContactEvent(enteredNumber);
        return true;
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
} /* namespace gui */
