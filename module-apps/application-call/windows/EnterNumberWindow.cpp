﻿// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CallAppStyle.hpp"
#include "CallSwitchData.hpp"
#include "EnterNumberWindow.hpp"

#include <country.hpp>
#include <i18n/i18n.hpp>
#include <service-appmgr/Controller.hpp>

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

        const auto iconsBox = new HBox(
            this, style::window::default_left_margin, iconsBox::y, style::window::default_body_width, iconsBox::h);
        iconsBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        iconsBox->setEdges(RectangleEdge::None);

        newContactIcon                    = new gui::AddContactIcon(iconsBox);
        newContactIcon->activatedCallback = [=]([[maybe_unused]] gui::Item &item) { return addNewContact(); };
        setFocusItem(newContactIcon);

        iconsBox->resizeItems();
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
            LOG_ERROR("No data received");
            return false;
        }

        if (data->getDescription() == app::EnterNumberData::descriptionStr) {
            const auto callData = dynamic_cast<app::EnterNumberData *>(data);
            assert(callData != nullptr);

            initFormatterInput(callData->getPhoneNumber());
            setNumberLabel(formattedNumber);
            application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
        }
        else if (data->getDescription() == app::CallSwitchData::descriptionStr) {
            const auto callData = dynamic_cast<app::CallSwitchData *>(data);
            assert(callData != nullptr);

            const auto &phoneNumber = callData->getPhoneNumber();

            initFormatterInput(phoneNumber.getEntered());
            setNumberLabel(phoneNumber.getFormatted());
            application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);

            if (callData->getType() == app::CallSwitchData::Type::EXECUTE_CALL) {
                interface->handleCallEvent(phoneNumber.getEntered());
            }
        }
        else {
            LOG_ERROR("Unhandled switch data: %s", data->getDescription().c_str());
        }

        return true;
    }
} /* namespace gui */
