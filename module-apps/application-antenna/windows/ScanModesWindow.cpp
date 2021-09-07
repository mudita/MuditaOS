// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ScanModesWindow.hpp"
#include <application-antenna/AntennaAppStyle.hpp>
#include <application-antenna/ApplicationAntenna.hpp>
#include <i18n/i18n.hpp>
#include <module-gui/gui/widgets/BoxLayout.hpp>
#include <module-gui/gui/widgets/Item.hpp>
#include <module-gui/gui/widgets/Label.hpp>
#include <module-gui/gui/widgets/Style.hpp>
#include <module-gui/gui/widgets/Window.hpp>
#include <service-cellular/CellularServiceAPI.hpp>

namespace gui
{

    ScanModesWindow::ScanModesWindow(app::Application *app) : AppWindow(app, gui::name::window::scan_window)
    {
        buildInterface();
    }

    void ScanModesWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }
    void ScanModesWindow::buildInterface()
    {
        AppWindow::buildInterface();

        bottomBar->setActive(BottomBar::Side::LEFT, true);
        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::CENTER, utils::translate(style::strings::common::open));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::translate(style::strings::common::back));

        setTitle(utils::translate("app_desktop_tools_antenna"));

        modeButtonParams.insert(std::pair<uint32_t, std::string>(scanModes::Auto, "AUTO"));
        modeButtonParams.insert(std::pair<uint32_t, std::string>(scanModes::GSM_only, "GSM only"));
        modeButtonParams.insert(std::pair<uint32_t, std::string>(scanModes::WCDMA_only, "WCDMA only"));
        modeButtonParams.insert(std::pair<uint32_t, std::string>(scanModes::LTE_only, "LTE only"));
        modeButtonParams.insert(std::pair<uint32_t, std::string>(scanModes::TD_SCDMA_only, "TD-SCDMA only"));
        modeButtonParams.insert(std::pair<uint32_t, std::string>(scanModes::UTMS_only, "UTMS only"));
        modeButtonParams.insert(std::pair<uint32_t, std::string>(scanModes::CDMA_only, "CDMA only"));
        modeButtonParams.insert(std::pair<uint32_t, std::string>(scanModes::HDR_only, "HDR only"));
        modeButtonParams.insert(std::pair<uint32_t, std::string>(scanModes::CDMA_and_HDR_only, "CDMA and HDR only"));

        uint32_t w = this->getWidth() - style::window::default_left_margin * 2;
        uint32_t h = this->getHeight() - style::window::default_vertical_pos - bottomBar->getHeight();

        modesBox = new gui::VBox(this, style::window::default_left_margin, style::window::default_vertical_pos, w, h);
        modesBox->setPenWidth(0);
        modesBox->setPenFocusWidth(0);
        modesBox->setEdges(gui::RectangleEdge::None);

        for (auto el : modeButtonParams) {
            gui::Label *modeLabel = addMode(nullptr, el.second);

            modeLabel->activatedCallback = [=](gui::Item &) {
                std::string mode;
                try {
                    mode = std::to_string(el.first);
                }
                catch (const std::exception &e) {
                    LOG_ERROR("ServiceCellular::std::to_string exception %s", e.what());
                }
                auto result = CellularServiceAPI::SetScanMode(this->application, mode);
                if (result) {
                    commandResult->setText("Success!");
                }
                else {
                    commandResult->setText("Failure!");
                }
                application->render(RefreshModes::GUI_REFRESH_FAST);
                return true;
            };
            modesBox->addWidget(modeLabel);
            modesBox->addWidget(addSpacer());
        }

        commandResult = addMode(this, "");
        commandResult->setPosition(style::window::default_left_margin, antenna::scan_mode_window::reusultLabelPosY);
        setFocusItem(modesBox);

        CellularServiceAPI::GetScanMode(this->application);
    }

    void ScanModesWindow::destroyInterface()
    {
        erase();
    }

    void ScanModesWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {}

    bool ScanModesWindow::onInput(const InputEvent &inputEvent)
    {
        bool ret = AppWindow::onInput(inputEvent);
        if (ret) {
            application->render(RefreshModes::GUI_REFRESH_FAST);
            return true;
        }
        return false;
    }

    gui::Label *ScanModesWindow::addMode(gui::Item *parent, const UTF8 &text)
    {
        gui::Label *label = new gui::Label(parent,
                                           antenna::scan_mode_window::commonDefaultPos,
                                           antenna::scan_mode_window::commonDefaultPos,
                                           antenna::scan_mode_window::buttonW,
                                           antenna::scan_mode_window::buttonH,
                                           text);
        label->setFilled(false);
        label->setPenFocusWidth(antenna::scan_mode_window::commonFocusPen);
        label->setPenWidth(antenna::scan_mode_window::commonNoFocusPen);

        label->setFont(style::window::font::verysmall);
        label->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));

        return label;
    }

    gui::Label *ScanModesWindow::addSpacer(void)
    {
        gui::Label *label = new gui::Label(nullptr,
                                           antenna::scan_mode_window::commonDefaultPos,
                                           antenna::scan_mode_window::commonDefaultPos,
                                           antenna::scan_mode_window::buttonW,
                                           antenna::scan_mode_window::commonSpacerH);
        label->setEdges(gui::RectangleEdge::None);
        label->setFilled(false);
        label->activeItem = false;
        label->visible    = false;

        return label;
    }
    void ScanModesWindow::updateCurrentMode(std::string &data)
    {
        uint32_t mode = 0;
        try {
            mode = std::stoi(data);
        }
        catch (const std::exception &e) {
            LOG_ERROR("ScanModesWindow::updateCurrentMode exception %s", e.what());
            return;
        }
        if (mode < modeButtonParams.size()) {
            modesBox->setFocusOnElement(mode);
        }
    }
} // namespace gui
