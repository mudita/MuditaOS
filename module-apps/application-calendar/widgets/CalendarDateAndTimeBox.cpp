// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CalendarDateAndTimeBox.hpp"

namespace gui
{
    CalendarDateAndTimeBox::CalendarDateAndTimeBox(app::Application *application) : app(application)
    {
        assert(app != nullptr);

        setMinimumSize(style::window::default_body_width, 400);
        setEdges(RectangleEdge::None);

        vBox = new VBox(this, 0, 0, 0, 0);
        vBox->setEdges(RectangleEdge::None);

        dateAndTimeBox = new DateAndTimeBox(app, vBox);

        focusChangedCallback = [&](Item &item) {
            if (focus) {
                if (savedFocusItem == nullptr) {
                    setFocusItem(vBox);
                }
                else {
                    savedFocusItem->parent->focusChangedCallback(*savedFocusItem);
                }
            }
            else {
                savedFocusItem = getFocusItem();
            }
            return true;
        };

        inputCallback = [&](gui::Item &item, const gui::InputEvent &event) {
            if (event.state != gui::InputEvent::State::keyReleasedShort) {
                return false;
            }
            return vBox->onInput(event);
        };

        onLoadCallback = [&](std::shared_ptr<EventsRecord> event) { dateAndTimeBox->onLoadCallback(event); };

        onSaveCallback = [&](std::shared_ptr<EventsRecord> event) -> bool {
            dateAndTimeBox->onSaveCallback(event);
            return true;
        };
    }

    bool CalendarDateAndTimeBox::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        vBox->setPosition(0, 0);
        vBox->setSize(newDim.w, newDim.h);
        return true;
    }
} /* namespace gui */
