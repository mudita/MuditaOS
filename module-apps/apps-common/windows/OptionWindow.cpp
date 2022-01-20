// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "OptionWindow.hpp"
#include <messages/OptionsWindow.hpp>

namespace gui
{
    OptionWindow::OptionWindow(app::ApplicationCommon *app, const std::string &name, const std::string &windowTitle)
        : AppWindow(app, name), OptionsList(std::make_shared<OptionsModel>(app)), windowTitle(windowTitle)
    {
        buildInterface();
    }

    OptionWindow::OptionWindow(app::ApplicationCommon *app,
                               const std::string &name,
                               std::list<Option> options,
                               const std::string &windowTitle)
        : AppWindow(app, name), OptionsList(std::make_shared<OptionsModel>(app), std::move(options)),
          windowTitle(windowTitle)
    {
        buildInterface();
    }

    void OptionWindow::rebuild()
    {
        recreateOptions();
    }

    void OptionWindow::buildInterface()
    {
        AppWindow::buildInterface();
        navBar->setActive(nav_bar::Side::Center, true);
        navBar->setActive(nav_bar::Side::Right, true);
        navBar->setText(nav_bar::Side::Center, utils::translate(style::strings::common::select));
        navBar->setText(nav_bar::Side::Right, utils::translate(style::strings::common::back));
        setTitle(windowTitle.empty() ? utils::translate("common_options_title") : windowTitle);

        optionsList = new gui::ListView(this,
                                        option::window::optionsListX,
                                        option::window::optionsListY,
                                        option::window::optionsListW,
                                        option::window::optionsListH,
                                        optionsModel,
                                        listview::ScrollBarType::None);
        optionsList->setBoundaries(Boundaries::Continuous);
        optionsList->prepareRebuildCallback = [this]() { recreateOptions(); };

        optionsModel->createData(options);

        setFocusItem(optionsList);
    }

    void OptionWindow::onClose([[maybe_unused]] CloseReason reason)
    {
        if (optionsList != nullptr) {
            optionsList->onClose();
        }
    }

    void OptionWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (auto message = dynamic_cast<gui::OptionsWindowOptions *>(data)) {
            LOG_DEBUG("Options load!");
            options = message->takeOptions();
        }

        optionsList->rebuildList(listview::RebuildType::InPlace);
    }

    void OptionWindow::destroyForTheFuture()
    {
        erase(optionsList);
        optionsList = nullptr;
    }

} /* namespace gui */
