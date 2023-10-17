// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ColorTestListItem.hpp"
#include "ColorTestListStyle.hpp"
#include <Label.hpp>

namespace gui
{

    ColorTestListItem::ColorTestListItem(app::ApplicationCommon *app, const Color color) : app{app}
    {
        setMinimumSize(style::listview::item_width_with_scroll, style::colorTest::item::color::height);
        setMargins(Margins(style::margins::very_big, 0, 0, 0));

        vBox = new VBox(this, 0, 0, 0, 0);
        vBox->setEdges(RectangleEdge::None);
        vBox->setMinimumSize(style::colorTest::item::color::width, style::colorTest::listview::height);
        vBox->setFillColor(color);

        colorLabel = new Label(vBox, 0, 0, 0, 0);
        colorLabel->setEdges(gui::RectangleEdge::None);
        colorLabel->setMinimumSize(style::colorTest::item::color::width, style::colorTest::item::color::height);
        colorLabel->setAlignment(Alignment{gui::Alignment::Horizontal::Right, gui::Alignment::Vertical::Top});
        colorLabel->setText(std::to_string(color.intensity) + " ");

        if (color.intensity > ColorGrey.intensity) {
            colorLabel->setColor(ColorFullBlack);
        }
        else {
            colorLabel->setColor(ColorFullWhite);
        }

        dimensionChangedCallback = [&](gui::Item &, const BoundingBox &newDim) -> bool {
            vBox->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };
    }

    bool ColorTestListItem::onInput(const InputEvent &inputEvent)
    {
        bool handled = false;

        if (inputEvent.isShortRelease()) {
            if (inputEvent.is(KeyCode::KEY_LEFT)) {
                if (auto color = vBox->fillColor; color.intensity != ::gui::ColorFullBlack.intensity) {
                    color.intensity--;
                    vBox->setFillColor(color);
                    app->refreshWindow(RefreshModes::GUI_REFRESH_DEEP);
                }
                handled = true;
            }
            else if (inputEvent.is(KeyCode::KEY_RIGHT)) {
                if (auto color = vBox->fillColor; color.intensity != ::gui::ColorFullWhite.intensity) {
                    color.intensity++;
                    vBox->setFillColor(color);
                    app->refreshWindow(RefreshModes::GUI_REFRESH_DEEP);
                }
                handled = true;
            }
        }
        return handled;
    }

    uint8_t ColorTestListItem::getColorIntensity()
    {
        return vBox->fillColor.intensity;
    }
} // namespace gui
