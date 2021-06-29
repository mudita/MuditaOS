// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationSettings.hpp"
#include "CertificationWindow.hpp"
#include "SettingsStyle.hpp"

#include <widgets/Image.hpp>
#include <widgets/Text.hpp>

inline constexpr auto certno = "XXXXXXXXXXXXXXXXXXX";

namespace gui
{
    CertificationWindow::CertificationWindow(app::Application *app) : AppWindow(app, gui::window::name::certification)
    {
        buildInterface();
    }

    void CertificationWindow::buildInterface()
    {
        AppWindow::buildInterface();

        setTitle(utils::translate("app_settings_certification"));

        usFccIdText = new gui::Text(this,
                                    style::certification::textfc::x,
                                    style::certification::textfc::y,
                                    style::certification::textfc::width,
                                    style::certification::textfc::height);
        usFccIdText->setFont(::style::window::font::bigbold);
        usFccIdValue = new gui::Text(this,
                                     style::certification::valuefc::x,
                                     style::certification::valuefc::y,
                                     style::certification::valuefc::width,
                                     style::certification::valuefc::height);
        usFccIdImage = new gui::Image(this,
                                      style::certification::imagefc::x,
                                      style::certification::imagefc::y,
                                      style::certification::imagefc::width,
                                      style::certification::imagefc::height,
                                      "settings_certification_fc");

        canadaIcText = new gui::Text(this,
                                     style::certification::textic::x,
                                     style::certification::textic::y,
                                     style::certification::textic::width,
                                     style::certification::textic::height);
        canadaIcText->setFont(::style::window::font::bigbold);
        canadaIcValue = new gui::Text(this,
                                      style::certification::valueic::x,
                                      style::certification::valueic::y,
                                      style::certification::valueic::width,
                                      style::certification::valueic::height);
        canadaIcImage = new gui::Image(this,
                                       style::certification::imageic::x,
                                       style::certification::imageic::y,
                                       style::certification::imageic::width,
                                       style::certification::imageic::height,
                                       "settings_certification_ic");

        europeCeText = new gui::Text(this,
                                     style::certification::textce::x,
                                     style::certification::textce::y,
                                     style::certification::textce::width,
                                     style::certification::textce::height);
        europeCeText->setFont(::style::window::font::bigbold);
        europeCeValue = new gui::Text(this,
                                      style::certification::valuece::x,
                                      style::certification::valuece::y,
                                      style::certification::valuece::width,
                                      style::certification::valuece::height);
        europeCeImage = new gui::Image(this,
                                       style::certification::imagece::x,
                                       style::certification::imagece::y,
                                       style::certification::imagece::width,
                                       style::certification::imagece::height,
                                       "settings_certification_ce");

        bottomBar->setText(BottomBar::Side::RIGHT, utils::translate(style::strings::common::back));
    }

    void CertificationWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        // dummy data for now (to be changed later)
        usFccIdText->setText(utils::translate("app_settings_us_fcc_id"));
        usFccIdValue->setText(certno);
        canadaIcText->setText(utils::translate("app_settings_canada_ic"));
        canadaIcValue->setText(certno);
        europeCeText->setText(utils::translate("app_settings_europe"));
        europeCeValue->setText(certno);
    }

    void CertificationWindow::destroyInterface()
    {
        erase();
        usFccIdText   = nullptr;
        usFccIdValue  = nullptr;
        usFccIdImage  = nullptr;
        canadaIcText  = nullptr;
        canadaIcValue = nullptr;
        canadaIcImage = nullptr;
        europeCeText  = nullptr;
        europeCeValue = nullptr;
        europeCeImage = nullptr;
    };

    CertificationWindow::~CertificationWindow()
    {
        destroyInterface();
    }
} // namespace gui
