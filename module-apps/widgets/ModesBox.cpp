// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <i18n/i18n.hpp>
#include "ModesBox.hpp"

#include <gui/widgets/ImageBox.hpp>
#include <PhoneModes/Common.hpp>
#include <Utils.hpp>

namespace gui
{
    namespace img::dot
    {
        constexpr auto name = "dot_12px_hard_alpha_W_G";
    } // namespace img::dot

    ModesBox::ModesBox(Item *parent, uint32_t x, uint32_t y)
        : VBox(parent, x, y, style::window::modes::width, style::window::modes::height)
    {
        setEdges(RectangleEdge::None);
        addConnected();
        addNotDisturb();
        addOffline();
    }

    void ModesBox::update(const sys::phone_modes::PhoneMode &phoneMode, const bool flightMode)
    {
        using PhoneMode      = sys::phone_modes::PhoneMode;
        auto getUpdateValues = [&phoneMode](const PhoneMode &compare) -> std::pair<std::string, const bool> {
            return (phoneMode == compare) ? std::make_pair(style::window::font::largelight, true)
                                          : std::make_pair(style::window::font::medium, false);
        };
        connected->update(getUpdateValues(PhoneMode::Connected));
        notDisturb->update(getUpdateValues(PhoneMode::DoNotDisturb));
        offline->update(getUpdateValues(PhoneMode::Offline));
        if (phoneMode == PhoneMode::Offline && !flightMode) {
            messageOnly->setVisible(true);
        }
        else {
            messageOnly->setVisible(false);
        }
    }

    void ModesBox::addConnected()
    {
        connected = new ModeRow(this,
                                0,
                                0,
                                style::window::modes::width,
                                style::window::modes::height / style::window::modes::number_of_entries);
        connected->addText(utils::translate(style::window::modes::connected::title_key),
                           style::window::font::medium,
                           style::window::modes::text::width,
                           style::window::modes::height / style::window::modes::number_of_entries);

        connected->addImage(img::dot::name);
    }

    void ModesBox::addNotDisturb()
    {
        notDisturb = new ModeRow(this,
                                 0,
                                 0,
                                 style::window::modes::width,
                                 style::window::modes::height / style::window::modes::number_of_entries);

        notDisturb->addText(utils::translate(style::window::modes::notdisturb::title_key),
                            style::window::font::medium,
                            style::window::modes::text::width,
                            style::window::modes::height / style::window::modes::number_of_entries);
        notDisturb->addImage(img::dot::name);
    }

    void ModesBox::addOffline()
    {
        offline = new ModeRow(this,
                              0,
                              0,
                              style::window::modes::width,
                              style::window::modes::height / style::window::modes::number_of_entries);
        offline->addText(utils::translate(style::window::modes::offline::title_key),
                         style::window::font::medium,
                         style::window::modes::text::width,
                         style::window::modes::height / style::window::modes::number_of_entries);
        offline->addImage(img::dot::name);

        messageOnly = new ModeRow(this,
                                  0,
                                  0,
                                  style::window::modes::width,
                                  style::window::modes::height / style::window::modes::number_of_entries);

        messageOnly->addText(utils::translate(style::window::modes::offline::description_key),
                             style::window::font::verysmall,
                             style::window::modes::text::width,
                             style::window::modes::height / style::window::modes::number_of_entries);
    }

    ModeRow::ModeRow(Item *parent, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
        : HBox(parent, x, y, width, height)
    {
        this->setEdges(RectangleEdge::None);
    }

    void ModeRow::addText(const std::string &text, const std::string &fontSize, uint32_t width, uint32_t height)
    {
        label = new Label(this, 0, 0, width, height, text);
        label->setEdges(gui::RectangleEdge::None);
        label->setAlignment(Alignment(gui::Alignment::Horizontal::Right, gui::Alignment::Vertical::Center));
        label->setFont(fontSize);
    }

    void ModeRow::update(std::pair<std::string, const bool> &&params)
    {
        const auto [font, imgVisibility] = params;
        img->setVisible(imgVisibility);
        label->setFont(font);
    }

    void ModeRow::addImage(const std::string &imageName)
    {
        img = new ImageBox(this,
                           0,
                           0,
                           style::window::modes::image::width,
                           style::window::modes::height / style::window::modes::number_of_entries,
                           new Image(imageName));

        img->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
    }
} // namespace gui
