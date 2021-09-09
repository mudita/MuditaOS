// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "OptionWindow.hpp"
#include "Label.hpp"
#include <i18n/i18n.hpp>
#include <log.hpp>
#include <Style.hpp>
#include <memory>
#include <utility>
#include <messages/OptionsWindow.hpp>

namespace gui
{
    OptionWindow::OptionWindow(app::ApplicationCommon *app, const std::string &name)
        : AppWindow(app, name), optionsModel{std::make_shared<OptionsModel>(app)}
    {
        buildInterface();
    }

    OptionWindow::OptionWindow(app::ApplicationCommon *app, const std::string &name, std::list<Option> options)
        : AppWindow(app, name), optionsModel{std::make_shared<OptionsModel>(app)}, options(std::move(options))
    {
        buildInterface();
    }

    void OptionWindow::rebuild()
    {
        recreateOptions();
    }

    void OptionWindow::createOptions()
    {
        optionsModel->createData(options);
    }

    void OptionWindow::refreshOptions(std::list<Option> &&optionList)
    {
        options = std::move(optionList);
        optionsList->rebuildList(listview::RebuildType::InPlace);
    }

    void OptionWindow::refreshOptions(std::list<Option> &&optionList, unsigned int pageIndex)
    {
        options = std::move(optionList);
        optionsList->rebuildList(listview::RebuildType::OnPageElement, pageIndex);
    }

    void OptionWindow::addOptions(std::list<Option> &&optionList)
    {
        options = std::move(optionList);
        createOptions();

        optionsList->rebuildList();
    }

    void OptionWindow::changeOptions(std::list<Option> &&optionList)
    {
        clearOptions();
        addOptions(std::move(optionList));
    }

    void OptionWindow::recreateOptions()
    {
        clearOptions();
        createOptions();
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
        bottomBar->setText(BottomBar::Side::CENTER, utils::translate(style::strings::common::select));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::translate(style::strings::common::back));

        setTitle(name);

        optionsList = new gui::ListView(this,
                                        option::window::optionsListX,
                                        option::window::optionsListY,
                                        option::window::optionsListW,
                                        option::window::optionsListH,
                                        optionsModel,
                                        listview::ScrollBarType::None);

        optionsList->prepareRebuildCallback = [this]() { recreateOptions(); };

        optionsModel->createData(options);

        setFocusItem(optionsList);
    }

    void OptionWindow::onClose([[maybe_unused]] CloseReason reason)
    {
        optionsList->onClose();
    }

    void OptionWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (auto message = dynamic_cast<gui::OptionsWindowOptions *>(data)) {
            LOG_DEBUG("Options load!");
            options = message->takeOptions();
        }

        optionsList->rebuildList(listview::RebuildType::InPlace);
    }

} /* namespace gui */
