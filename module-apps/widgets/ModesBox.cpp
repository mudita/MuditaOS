// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <module-utils/i18n/i18n.hpp>
#include "ModesBox.hpp"

namespace gui
{
    ModesBox::ModesBox(Item *parent, uint32_t x, uint32_t y)
        : VBox(parent, x, y, style::window::modes::width, style::window::modes::height)
    {
        setEdges(RectangleEdge::None);
        addConnected();
        addNotDisturb();
        addOffline();
    }

    void ModesBox::addConnected()
    {
        connected = new ModeRow(this, 0, 0, style::window::modes::width, style::window::modes::connected::height);
        connected->addText(utils::localize.get(style::window::modes::connected::title_key),
                           style::window::font::medium,
                           Margins(style::window::modes::connected::margin::left, 0, 0, 0),
                           style::window::modes::connected::width,
                           style::window::modes::connected::height);
        connected->resizeItems();
    }

    void ModesBox::addNotDisturb()
    {
        notDisturb = new ModeRow(this, 0, 0, style::window::modes::width, style::window::modes::notdisturb::height);
        notDisturb->addText(
            utils::localize.get(style::window::modes::notdisturb::title_key),
            style::window::font::largelight,
            Margins(
                style::window::modes::notdisturb::margin::left, 0, style::window::modes::notdisturb::margin::right, 0),
            style::window::modes::notdisturb::width,
            style::window::modes::notdisturb::height);
        notDisturb->addImage("dot_12px_hard_alpha_W_G");
        notDisturb->setMargins(Margins(
            0, style::window::modes::notdisturb::margin::top, 0, style::window::modes::notdisturb::margin::bottom));
        notDisturb->resizeItems();
    }

    void ModesBox::addOffline()
    {
        offline = new ModeRow(this, 0, 0, style::window::modes::width, style::window::modes::offline::height);

        offline->addText(utils::localize.get(style::window::modes::offline::title_key),
                         style::window::font::medium,
                         Margins(style::window::modes::offline::margin::left, 0, 0, 0),
                         style::window::modes::offline::width,
                         style::window::modes::offline::height);
        offline->resizeItems();
    }

    ModeRow::ModeRow(Item *parent, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
        : HBox(parent, x, y, width, height)
    {
        this->setEdges(RectangleEdge::None);
    }

    void ModeRow::addText(
        const std::string &text, const std::string &fontSize, const Margins &margin, uint32_t width, uint32_t height)
    {
        label = new Label(this, 0, 0, width, height);
        label->setMinimumSize(width, height);
        label->setEdges(gui::RectangleEdge::None);
        label->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        label->activeItem = false;
        label->setText(text);
        label->setFont(fontSize);
        label->setMargins(margin);
    }

    void ModeRow::addImage(const std::string &imageName)
    {
        img = new Image(this, 0, 0, 0, 0);
        img->setMinimumSize(style::window::modes::image::width, style::window::modes::image::height);
        img->setAlignment(Alignment(gui::Alignment::Horizontal::Right, gui::Alignment::Vertical::Center));
        img->set(imageName);
        img->setMargins(Margins(style::window::modes::image::margin::left, 0, 0, 0));
    }
} // namespace gui
