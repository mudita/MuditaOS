/*
 * ScanModeWindow.cpp
 *
 *  Created on: 31 mar 2020
 *      Author: kuba
 */

#include "ScanModesWindow.hpp"
#include "gui/widgets/BoxLayout.hpp"
#include "gui/widgets/Item.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Window.hpp"

#include "i18/i18.hpp"
#include <Style.hpp>

#include "../AntennaAppStyle.hpp"
#include "../ApplicationAntenna.hpp"
#include "service-cellular/api/CellularServiceAPI.hpp"

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
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::open));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));

        topBar->setActive(TopBar::Elements::SIGNAL, true);
        topBar->setActive(TopBar::Elements::TIME, true);
        topBar->setActive(TopBar::Elements::BATTERY, true);

        setTitle(utils::localize.get("app_desktop_tools_antenna"));

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
        uint32_t h = this->getHeight() - title->offset_h() - bottomBar->getHeight();

        modesBox = new gui::VBox(this, style::window::default_left_margin, (uint32_t)title->offset_h(), w, h);
        modesBox->setPenWidth(0);
        modesBox->setPenFocusWidth(0);
        modesBox->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);

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
                    commandResult->setText("Succes!");
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
        label->setAlignment(
            gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));

        return label;
    }

    gui::Label *ScanModesWindow::addSpacer(void)
    {
        gui::Label *label = new gui::Label(nullptr,
                                           antenna::scan_mode_window::commonDefaultPos,
                                           antenna::scan_mode_window::commonDefaultPos,
                                           antenna::scan_mode_window::buttonW,
                                           antenna::scan_mode_window::commonSpacerH);
        label->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
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
