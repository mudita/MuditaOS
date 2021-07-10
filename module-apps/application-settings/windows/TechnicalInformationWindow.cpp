// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TechnicalInformationWindow.hpp"
#include <application-settings/ApplicationSettings.hpp>
#include <widgets/Text.hpp>
#include <application-settings/widgets/SettingsStyle.hpp>
#include <source/version.hpp>

namespace gui
{

    TechnicalInformationWindow::TechnicalInformationWindow(
        app::Application *app, std::unique_ptr<TechnicalWindowContract::Presenter> technicalPresenter)
        : AppWindow(app, gui::window::name::certification), presenter(std::move(technicalPresenter))
    {
        presenter->attach(this);
        buildInterface();
    }

    void TechnicalInformationWindow::buildInterface()
    {
        AppWindow::buildInterface();

        setTitle(utils::translate("app_settings_technical_information"));

        auto vBox = new VBox(this,
                             style::window::default_left_margin,
                             style::window::default_vertical_pos + style::margins::very_big,
                             style::window::default_body_width,
                             style::window::default_body_height);
        vBox->setEdges(RectangleEdge::None);

        modelText = new gui::Text(vBox, 0, 0, style::techinfo::width, style::techinfo::height);
        modelText->setText(utils::translate("app_settings_tech_info_model"));
        modelText->setFont(style::window::font::smallbold);
        modelValue = new gui::Text(vBox, 0, 0, style::techinfo::width, style::techinfo::height);
        modelValue->setText(presenter->getModel());

        serialNumberText = new gui::Text(vBox, 0, 0, style::techinfo::width, style::techinfo::height);
        serialNumberText->setFont(style::window::font::smallbold);
        serialNumberText->setText(utils::translate("app_settings_tech_info_serial_number"));
        serialNumberValue = new gui::Text(vBox, 0, 0, style::techinfo::width, style::techinfo::height);
        serialNumberValue->setText(presenter->getSerial());

        osVersionText = new gui::Text(vBox, 0, 0, style::techinfo::width, style::techinfo::height);
        osVersionText->setFont(style::window::font::smallbold);
        osVersionText->setText(utils::translate("app_settings_tech_info_os_version"));
        osVersionValue = new gui::Text(vBox, 0, 0, style::techinfo::width, style::techinfo::height);
        osVersionValue->setText(std::string(VERSION));

        imeiText = new gui::Text(vBox, 0, 0, style::techinfo::width, style::techinfo::height);
        imeiText->setFont(style::window::font::smallbold);
        imeiText->setText(utils::translate("app_settings_tech_info_imei"));
        imeiValue = new gui::Text(vBox, 0, 0, style::techinfo::width, style::techinfo::height);
        imeiValue->setText(imei);

        batteryText = new gui::Text(vBox, 0, 0, style::techinfo::width, style::techinfo::height);
        batteryText->setFont(style::window::font::smallbold);
        batteryText->setText(utils::translate("app_settings_tech_info_battery"));
        batteryValue = new gui::Text(vBox, 0, 0, style::techinfo::width, style::techinfo::height);
        batteryValue->setText(presenter->getBatteryRev());

        pcbMbText = new gui::Text(vBox, 0, 0, style::techinfo::width, style::techinfo::height);
        pcbMbText->setText(utils::translate("app_settings_tech_info_pcb_mb"));
        pcbMbText->setFont(style::window::font::smallbold);
        pcbMbValue = new gui::Text(vBox, 0, 0, style::techinfo::width, style::techinfo::height);
        pcbMbValue->setText(presenter->getPcb(TechnicalWindowPresenter::PCB_MB));

        pcbLmText = new gui::Text(vBox, 0, 0, style::techinfo::width, style::techinfo::height);
        pcbLmText->setFont(style::window::font::smallbold);
        pcbLmText->setText(utils::translate("app_settings_tech_info_pcb_lm"));
        pcbLmValue = new gui::Text(vBox, 0, 0, style::techinfo::width, style::techinfo::height);
        pcbLmValue->setText(presenter->getPcb(TechnicalWindowPresenter::PCB_LM));

        pcbAmText = new gui::Text(vBox, 0, 0, style::techinfo::width, style::techinfo::height);
        pcbAmText->setFont(style::window::font::smallbold);
        pcbAmText->setText(utils::translate("app_settings_tech_info_pcb_am"));
        pcbAmValue = new gui::Text(vBox, 0, 0, style::techinfo::width, style::techinfo::height);
        pcbAmValue->setText(presenter->getPcb(TechnicalWindowPresenter::PCB_AM));

        pcbUmText = new gui::Text(vBox, 0, 0, style::techinfo::width, style::techinfo::height);
        pcbUmText->setFont(style::window::font::smallbold);
        pcbUmText->setText(utils::translate("app_settings_tech_info_pcb_um"));
        pcbUmValue = new gui::Text(vBox, 0, 0, style::techinfo::width, style::techinfo::height);
        pcbUmValue->setText(presenter->getPcb(TechnicalWindowPresenter::PCB_UM));

        bottomBar->setText(BottomBar::Side::RIGHT, utils::translate(style::strings::common::back));
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
