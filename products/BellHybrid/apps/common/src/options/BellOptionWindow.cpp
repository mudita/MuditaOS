// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "common/options/BellOptionWindow.hpp"
#include "common/options/OptionBellMenu.hpp"

#include <messages/OptionsWindow.hpp>
#include <TextFixedSize.hpp>

namespace gui
{
    BellOptionWindow::BellOptionWindow(app::ApplicationCommon *app, const std::string &name)
        : AppWindow(app, name), OptionsList(std::make_shared<OptionsModel>(
                                    app, style::bell_options::h + 2 * style::bell_options::option_margin))
    {
        buildInterface();
    }

    void BellOptionWindow::rebuild()
    {
        recreateOptions();
    }

    void BellOptionWindow::buildInterface()
    {
        AppWindow::buildInterface();

        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        navBar->setVisible(false);

        optionsList = new gui::ListViewWithArrows(this, 0, 0, style::window_width, style::window_height, optionsModel);
        optionsList->applySizeRestrictions(style::bell_options_list::w,
                                           style::bell_options_list::h,
                                           style::bell_options_list::outer_layouts_h,
                                           style::bell_options_list::outer_layouts_margin);

        optionsList->prepareRebuildCallback = [this]() { recreateOptions(); };
        optionsModel->createData(options);

        setFocusItem(optionsList);
    }

    void BellOptionWindow::setListTitle(const std::string &title)
    {
        optionsList->setListTitle(title);
    }

    void BellOptionWindow::onClose([[maybe_unused]] CloseReason reason)
    {
        optionsList->onClose();
    }

    void BellOptionWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (auto message = dynamic_cast<gui::OptionsWindowOptions *>(data)) {
            LOG_DEBUG("Options load!");
            options = message->takeOptions();
        }

        optionsList->rebuildList(listview::RebuildType::InPlace);
    }
} /* namespace gui */
