// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "OptionWindow.hpp"
#include "Label.hpp"
#include <i18n/i18n.hpp>
#include "log/log.hpp"
#include <service-appmgr/model/ApplicationManager.hpp>
#include <Style.hpp>
#include <memory>
#include <utility>
#include <messages/OptionsWindow.hpp>

namespace gui
{
    OptionWindow::OptionWindow(app::Application *app, const std::string &name)
        : AppWindow(app, name), optionsModel{std::make_shared<OptionsModel>(app)}
    {
        buildInterface();
    }

    OptionWindow::OptionWindow(app::Application *app, const std::string &name, std::list<Option> options)
        : AppWindow(app, name), optionsModel{std::make_shared<OptionsModel>(app)}, options(std::move(options))
    {
        buildInterface();
    }

    void OptionWindow::rebuild()
    {
        clearOptions();
        addOptions(options);
    }

    void OptionWindow::refreshOptions()
    {
        optionsList->rebuildList(style::listview::RebuildType::InPlace);
    }

    void OptionWindow::addOptions(std::list<Option> &optionList)
    {
        optionsModel->createData(optionList);
        optionsList->rebuildList();
    }

    void OptionWindow::addOptions(std::list<Option> &&optionList)
    {
        options = std::move(optionList);
        addOptions(options);
    }

    void OptionWindow::resetOptions(std::list<Option> &&optionList)
    {
        clearOptions();
        addOptions(std::move(optionList));
    }

    void OptionWindow::clearOptions()
    {
        optionsList->clear();
        optionsModel->clearData();
    }

    void OptionWindow::buildInterface()
    {
        AppWindow::buildInterface();
        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::select));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));

        setTitle(name);

        optionsList = new gui::ListView(this,
                                        option::window::optionsListX,
                                        option::window::optionsListY,
                                        option::window::optionsListW,
                                        option::window::optionsListH,
                                        optionsModel,
                                        style::listview::ScrollBarType::None);

        rebuild();
        setFocusItem(optionsList);
    }

    void OptionWindow::destroyInterface()
    {
        erase();
        optionsList = nullptr;
    }

    OptionWindow::~OptionWindow()
    {
        destroyInterface();
    }

    void OptionWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (auto message = dynamic_cast<gui::OptionsWindowOptions *>(data)) {
            LOG_DEBUG("Options load!");
            resetOptions(message->takeOptions());
        }
    }

} /* namespace gui */
