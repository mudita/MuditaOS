// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "common/options/BellOptionWindow.hpp"

#include <messages/OptionsWindow.hpp>
#include <TextFixedSize.hpp>

namespace gui
{
    BellOptionWindow::BellOptionWindow(app::ApplicationCommon *app, const std::string &name)
        : AppWindow(app, name), OptionsList(std::make_shared<OptionsModel>(app))
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
        bottomBar->setVisible(false);

        createLayout();

        optionsList = new gui::ListView(body->centerBox, 0, 0, 0, 0, optionsModel, listview::ScrollBarType::None);
        optionsList->setAlignment(Alignment(Alignment::Vertical::Center));
        optionsList->setMaximumSize(style::bell_options_list::w, style::bell_options_list::center_layout_h);

        body->centerBox->resizeItems();

        optionsList->prepareRebuildCallback = [this]() { recreateOptions(); };

        optionsModel->createData(options);

        setFocusItem(optionsList);
    }

    void BellOptionWindow::createLayout()
    {
        auto windowBody = new VBox(this, 0, 0, style::window_width, style::window_height);
        windowBody->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));

        body = new VThreeBox<VBox, VBox, VBox>(
            windowBody, 0, 0, style::bell_options_list::w, style::bell_options_list::center_layout_h);
        body->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        body->setEdges(RectangleEdge::None);

        body->firstBox = new VBox(body);
        body->firstBox->setMinimumSize(style::bell_options_list::w, style::bell_options_list::outer_layouts_h);
        body->firstBox->setMargins(Margins(0, 0, 0, style::bell_options_list::outer_layouts_margin));
        body->firstBox->setAlignment(Alignment(Alignment::Vertical::Top));
        body->firstBox->setEdges(RectangleEdge::None);
        body->firstBox->setVisible(false);

        body->centerBox = new VBox(body);
        body->centerBox->setAlignment(Alignment(gui::Alignment::Horizontal::Center));
        body->centerBox->setMaximumSize(style::bell_options_list::w, style::bell_options_list::center_layout_h);
        body->centerBox->setEdges(RectangleEdge::None);

        body->lastBox = new VBox(body);
        body->lastBox->setMinimumSize(style::bell_options_list::w, style::bell_options_list::outer_layouts_h);
        body->lastBox->setMargins(Margins(0, style::bell_options_list::outer_layouts_margin, 0, 0));
        body->lastBox->setAlignment(Alignment(Alignment::Vertical::Top));
        body->lastBox->setEdges(RectangleEdge::None);
        body->lastBox->setVisible(false);
    }

    void BellOptionWindow::setListTitle(const std::string &title)
    {
        auto titleBody = new TextFixedSize(body->firstBox);
        titleBody->drawUnderline(false);
        titleBody->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        titleBody->setFont(style::window::font::large);
        titleBody->setMinimumSize(style::bell_options_list::w, style::bell_options_list::outer_layouts_h);
        titleBody->setEdges(RectangleEdge::None);
        titleBody->setEditMode(EditMode::Browse);
        titleBody->setText(title);

        optionsList->setAlignment(Alignment(Alignment::Vertical::Top));
        body->firstBox->setVisible(true);

        body->resizeItems();
        // Second resize is needed as we need to first apply max size for center box and next extra margins.
        body->resizeItems();
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
