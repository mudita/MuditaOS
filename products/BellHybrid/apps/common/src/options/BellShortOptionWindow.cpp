// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "common/options/BellShortOptionWindow.hpp"
#include "common/options/OptionBellMenu.hpp"
#include "common/options/BellOptionsNavigation.hpp"

#include <messages/OptionsWindow.hpp>
#include <TextFixedSize.hpp>

namespace gui
{
    BellShortOptionWindow::BellShortOptionWindow(app::ApplicationCommon *app, const std::string &name)
        : AppWindow(app, name), OptionsList(std::make_shared<OptionsModel>(
                                    app, style::bell_options::h + 2 * style::bell_options::option_margin))
    {
        buildInterface();

        auto storedCallback        = optionsList->inputCallback;
        optionsList->inputCallback = [&, storedCallback](Item &item, const InputEvent &event) {
            return storedCallback(item, invertNavigationDirection(event));
        };
    }

    void BellShortOptionWindow::rebuild()
    {
        recreateOptions();
    }

    void BellShortOptionWindow::buildInterface()
    {
        AppWindow::buildInterface();

        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        navBar->setVisible(false);

        body = new BellBaseLayout(this, 0, 0, style::window_width, style::window_height, false);
        body->setEdges(RectangleEdge::None);

        optionsList = new gui::ListView(body->centerBox, 0, 0, 0, 0, optionsModel, listview::ScrollBarType::None);
        optionsList->setAlignment(Alignment(Alignment::Vertical::Bottom));
        optionsList->setMinimumSize(style::bell_options::default_text_width, style::bell_base_layout::center_layout_h);
        optionsList->setEdges(RectangleEdge::None);

        body->resize();

        optionsList->prepareRebuildCallback = [this]() { recreateOptions(); };
        optionsModel->createData(options);

        setFocusItem(optionsList);
    }

    void BellShortOptionWindow::setListTitle(const std::string &title, gui::Length height)
    {
        auto titleBody = new TextFixedSize(body->firstBox);
        titleBody->drawUnderline(false);
        titleBody->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        titleBody->setFont(style::window::font::largelight);
        titleBody->setMinimumSize(style::bell_base_layout::outer_layouts_w, height);
        titleBody->setEdges(RectangleEdge::None);
        titleBody->setEditMode(EditMode::Browse);
        titleBody->setRichText(title);

        body->firstBox->resizeItems();
    }

    void BellShortOptionWindow::onClose([[maybe_unused]] CloseReason reason)
    {
        if (reason != CloseReason::Popup) {
            optionsList->onClose();
        }
    }

    void BellShortOptionWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (auto message = dynamic_cast<gui::OptionsWindowOptions *>(data)) {
            LOG_DEBUG("Options load!");
            options = message->takeOptions();
        }

        optionsList->rebuildList(listview::RebuildType::InPlace);
    }

} /* namespace gui */
