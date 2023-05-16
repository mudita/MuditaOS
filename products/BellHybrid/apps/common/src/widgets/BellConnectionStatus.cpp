// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <EventStore.hpp>
#include <common/widgets/BellConnectionStatus.hpp>
#include <gui/widgets/text/TextFixedSize.hpp>

namespace
{
    constexpr auto connected_message      = "bell_connected-message";
} // namespace

namespace gui
{
    BellConnectionStatus::BellConnectionStatus(Item *parent, LayoutMode widthMode) : HBox(parent), widthMode(widthMode)
    {

        statusText = new Text(parent);
        statusText->setMaximumSize(350U, connected::status_h);
        statusText->setFont(style::window::font::veryverybiglight);
        statusText->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        statusText->setEdges(RectangleEdge::All);
        statusText->setEditMode(EditMode::Browse);
        statusText->activeItem = false;
        statusText->drawUnderline(false);
        statusText->setText("Connected");
        statusText->setVisible(false);

    }

    void BellConnectionStatus::setFont(const UTF8 &fontName)
    {
        statusText->setFont(fontName);
    }

    void BellConnectionStatus::update(const Store::Battery::State& state)
    {
        if (state != Store::Battery::State::Discharging) { // to be changed
            // Without this the text is not set properly if percentText was hidden
            statusText->setVisible(true);
        }
        else{
            statusText->setVisible(false);
        }
    }

//    void BellConnectionStatus::setBatteryPercentMode(BatteryPercentMode mode)
//    {
//        batteryPercentMode = mode;
//        if (batteryPercentMode == BatteryPercentMode::Show) {
//            connectedText->setVisible(true);
//        }
//        else {
//            connectedText->setVisible(false);
//        }
//
////        if (widthMode == BatteryWidthMode::FitToContent) {
////            setWidthsToFitContent();
////        }
//    }

    void BellConnectionStatus::setPositionToFitContent()
    {
//        if (connectedText->visible) {
//            connectedText->setMinimumWidthToFitText();
//            width += connectedText->widgetMinimumArea.w;
//        }
//        setMinimumWidth(width);
//        setMaximumWidth(width);
    }
} // namespace gui
