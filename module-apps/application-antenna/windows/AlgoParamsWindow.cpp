// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlgoParamsWindow.hpp"
#include <AntennaAppStyle.hpp>
#include <application-antenna/ApplicationAntenna.hpp>
#include <module-gui/gui/widgets/BoxLayout.hpp>
#include <module-gui/gui/widgets/Item.hpp>
#include <module-gui/gui/widgets/text/Label.hpp>
#include <module-gui/gui/widgets/Style.hpp>
#include <module-gui/gui/widgets/Window.hpp>
#include <i18n/i18n.hpp>
#include <service-cellular/CellularServiceAPI.hpp>

using StringList = std::list<std::string>;

namespace gui
{

    AlgoParamsWindow::AlgoParamsWindow(app::ApplicationCommon *app) : AppWindow(app, gui::name::window::algo_window)
    {
        buildInterface();
    }

    void AlgoParamsWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }
    void AlgoParamsWindow::buildInterface()
    {
        AppWindow::buildInterface();

        navBar->setActive(nav_bar::Side::Left, true);
        navBar->setActive(nav_bar::Side::Center, true);
        navBar->setActive(nav_bar::Side::Right, true);
        navBar->setText(nav_bar::Side::Center, utils::translate(style::strings::common::open));
        navBar->setText(nav_bar::Side::Right, utils::translate(style::strings::common::back));

        setTitle(utils::translate("app_desktop_tools_antenna"));

        lowBandBox  = new gui::VBox(this,
                                   antenna::algo_window::leftColumnXPos,
                                   antenna::algo_window::boxYPos,
                                   antenna::algo_window::boxW,
                                   antenna::algo_window::boxH);
        highBandBox = new gui::VBox(this,
                                    antenna::algo_window::rightColumnXPos,
                                    antenna::algo_window::boxYPos,
                                    antenna::algo_window::boxW,
                                    antenna::algo_window::boxH);

        selectedAntenna = new gui::Label(this,
                                         style::window::default_left_margin,
                                         antenna::algo_window::selectedAntennaYPos,
                                         antenna::algo_window::commonW,
                                         style::window::label::default_h);
        selectedAntenna->setFont(style::window::font::medium);
        selectedAntenna->setPenWidth(antenna::algo_window::commonNoFocusPen);

        info = new gui::Label(this,
                              style::window::default_left_margin,
                              antenna::algo_window::infoYPos,
                              antenna::algo_window::commonW,
                              style::window::label::small_h);
        info->setText("Params format: CSQ  |  CREG |  Band frequency");
        info->setFont(style::window::font::verysmall);
        info->setPenWidth(antenna::algo_window::commonNoFocusPen);

        lowBandLabel = new gui::Label(this,
                                      antenna::algo_window::leftColumnXPos,
                                      antenna::algo_window::culumnLabelYPos,
                                      antenna::algo_window::boxW,
                                      style::window::label::small_h);
        lowBandLabel->setText(lowBandString);
        lowBandLabel->setFont(style::window::font::verysmall);
        lowBandLabel->setPenWidth(antenna::algo_window::commonNoFocusPen);

        highBandLabel = new gui::Label(this,
                                       antenna::algo_window::rightColumnXPos,
                                       antenna::algo_window::culumnLabelYPos,
                                       antenna::algo_window::boxW,
                                       style::window::label::small_h);
        highBandLabel->setText(highBandString);
        highBandLabel->setFont(style::window::font::verysmall);
        highBandLabel->setPenWidth(antenna::algo_window::commonNoFocusPen);

        app = dynamic_cast<app::ApplicationAntenna *>(this->application);
        if (app != nullptr) {
            auto currentAntenna = app->getAntenna();
            updateAntennaLabel(currentAntenna);
        }
    }

    void AlgoParamsWindow::destroyInterface()
    {
        erase();
    }

    void AlgoParamsWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {}

    bool AlgoParamsWindow::onInput(const InputEvent &inputEvent)
    {
        bool ret = AppWindow::onInput(inputEvent);
        if (ret) {
            application->render(RefreshModes::GUI_REFRESH_FAST);
            return true;
        }
        return false;
    }

    void AlgoParamsWindow::handleNewParams(std::string data, bool refresh)
    {

        app = dynamic_cast<app::ApplicationAntenna *>(this->application);
        if (app == nullptr) {
            return;
        }

        StringList *list = nullptr;
        gui::VBox *box   = nullptr;
        if (app->getAntenna() == bsp::cellular::antenna::lowBand) {
            list = &lowBandParams;
            box  = lowBandBox;
        }
        else {
            list = &highBandParams;
            box  = highBandBox;
        }
        list->push_front(data);
        if (list->size() > app::antenna::paramsMaxSize) {
            list->pop_back();
        }
        box->erase();
        for (auto item : *list) {
            box->addWidget(addLabel(box, item));
        }

        if (refresh) {
            application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
        }
    }

    void AlgoParamsWindow::handleAntennaChanged(bsp::cellular::antenna antenna, bool refresh)
    {
        StringList *list    = nullptr;
        gui::VBox *box      = nullptr;
        auto currentAntenna = app->getAntenna();
        if (currentAntenna == bsp::cellular::antenna::lowBand) {
            list = &lowBandParams;
            box  = lowBandBox;
        }
        else {
            list = &highBandParams;
            box  = highBandBox;
        }
        updateAntennaLabel(currentAntenna);
        list->clear();
        box->erase();

        if (refresh) {
            application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
        }
    }
    gui::Label *AlgoParamsWindow::addLabel(gui::Item *parent, const UTF8 &text)
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
    void AlgoParamsWindow::updateAntennaLabel(bsp::cellular::antenna antenna)
    {
        if (antenna == bsp::cellular::antenna::lowBand) {
            selectedAntenna->setText(selectedAntennaPrefix + lowBandString);
        }
        else {

            selectedAntenna->setText(selectedAntennaPrefix + highBandString);
        }
    }
} // namespace gui
