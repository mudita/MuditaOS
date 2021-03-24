// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TechnicalInformationWindow.hpp"
#include <application-settings-new/ApplicationSettings.hpp>
#include <widgets/Text.hpp>
#include <application-settings-new/widgets/SettingsStyle.hpp>
#include <source/version.hpp>

static constexpr auto model        = "1.0";
static constexpr auto serialNumber = "XXXXXXXXXXXXXXX";
static constexpr auto imei         = "AA-BBBBBB-CCCCCC-D";

namespace gui
{
    TechnicalInformationWindow::TechnicalInformationWindow(app::Application *app)
        : AppWindow(app, gui::window::name::certification)
    {
        buildInterface();
    }

    void TechnicalInformationWindow::buildInterface()
    {
        AppWindow::buildInterface();

        setTitle(utils::localize.get("app_settings_technical_information"));

        modelText  = new gui::Text(this,
                                  style::techinfo::textmodel::x,
                                  style::techinfo::textmodel::y,
                                  style::techinfo::textmodel::width,
                                  style::techinfo::textmodel::height);
        modelValue = new gui::Text(this,
                                   style::techinfo::valuemodel::x,
                                   style::techinfo::valuemodel::y,
                                   style::techinfo::valuemodel::width,
                                   style::techinfo::valuemodel::height);

        serialNumberText  = new gui::Text(this,
                                         style::techinfo::textserialnumber::x,
                                         style::techinfo::textserialnumber::y,
                                         style::techinfo::textserialnumber::width,
                                         style::techinfo::textserialnumber::height);
        serialNumberValue = new gui::Text(this,
                                          style::techinfo::valueserialnumber::x,
                                          style::techinfo::valueserialnumber::y,
                                          style::techinfo::valueserialnumber::width,
                                          style::techinfo::valueserialnumber::height);

        osVersionText  = new gui::Text(this,
                                      style::techinfo::textosversion::x,
                                      style::techinfo::textosversion::y,
                                      style::techinfo::textosversion::width,
                                      style::techinfo::textosversion::height);
        osVersionValue = new gui::Text(this,
                                       style::techinfo::valueosversion::x,
                                       style::techinfo::valueosversion::y,
                                       style::techinfo::valueosversion::width,
                                       style::techinfo::valueosversion::height);

        imeiText  = new gui::Text(this,
                                 style::techinfo::textimea::x,
                                 style::techinfo::textimea::y + 10,
                                 style::techinfo::textimea::width,
                                 style::techinfo::textimea::height);
        imeiValue = new gui::Text(this,
                                  style::techinfo::valueimea::x,
                                  style::techinfo::valueimea::y + 20,
                                  style::techinfo::valueimea::width,
                                  style::techinfo::valueimea::height);

        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));
    }

    void TechnicalInformationWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        // dummy data for now
        modelText->setText(utils::localize.get("app_settings_tech_info_model"));
        modelValue->setText(model);
        serialNumberText->setText(utils::localize.get("app_settings_tech_info_serial_number"));
        serialNumberValue->setText(serialNumber);
        osVersionText->setText(utils::localize.get("app_settings_tech_info_os_version"));
        osVersionValue->setText(std::string(VERSION));
        imeiText->setText(utils::localize.get("app_settings_tech_info_imei"));
        imeiValue->setText(imei);
    }

    void TechnicalInformationWindow::destroyInterface()
    {
        erase();
        modelText         = nullptr;
        modelValue        = nullptr;
        serialNumberText  = nullptr;
        serialNumberValue = nullptr;
        osVersionText     = nullptr;
        osVersionValue    = nullptr;
        imeiText          = nullptr;
        imeiValue         = nullptr;
    };

    TechnicalInformationWindow::~TechnicalInformationWindow()
    {
        destroyInterface();
    }
} // namespace gui
