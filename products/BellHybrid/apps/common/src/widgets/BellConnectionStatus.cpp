// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include <EventStore.hpp>
#include <common/widgets/BellConnectionStatus.hpp>
#include <gui/widgets/text/TextFixedSize.hpp>
#include "i18n/i18n.hpp"

namespace
{
    constexpr auto usb_connected_status     = "app_bellmain_usb_status_connected";
    constexpr auto status_text_max_w        = 350U;
    constexpr auto status_text_max_h        = 102U;
} // namespace

namespace gui
{
    BellConnectionStatus::BellConnectionStatus(Item *parent) : HBox(parent)
    {
        statusText = new Text(parent);
        statusText->setMaximumSize(status_text_max_w, status_text_max_h);
        statusText->setFont(style::window::font::verybiglight);
        statusText->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        statusText->setEdges(RectangleEdge::None);
        statusText->setEditMode(EditMode::Browse);
        statusText->activeItem = false;
        statusText->drawUnderline(false);
        statusText->setText(utils::translate(usb_connected_status));
        statusText->setVisible(false);
    }

    void BellConnectionStatus::setFont(const UTF8 &fontName)
    {
        statusText->setFont(fontName);
    }

    void BellConnectionStatus::show(bool visibility)
    {
        statusText->setVisible(visibility);
    }

    bool BellConnectionStatus::isVisible() const
    {
        return statusText->visible;
    }
} // namespace gui
