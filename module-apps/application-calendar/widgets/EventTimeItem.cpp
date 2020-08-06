#include "EventTimeItem.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include <ListView.hpp>
#include <Style.hpp>
#include <Utils.hpp>

namespace gui
{

    EventTimeItem::EventTimeItem(const std::string &description, bool mode24H) : mode24H{mode24H}
    {
        setMinimumSize(style::window::default_body_width, style::window::calendar::item::eventTime::height);
        setMaximumSize(style::window::default_body_width, style::window::calendar::item::eventTime::height);

        setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        setMargins(gui::Margins(0, style::window::calendar::item::eventTime::margin, 0, 0));

        hBox = new gui::HBox(this, 0, 0, style::window::default_body_width, 0);
        hBox->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        hBox->setPenFocusWidth(style::window::default_border_focus_w);
        hBox->setPenWidth(style::window::default_border_rect_no_focus);

        colonLabel = new gui::Label(hBox, 0, 0, 0, 0);
        colonLabel->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        colonLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Bottom));
        colonLabel->setFont(style::window::font::medium);
        colonLabel->setText(":");
        colonLabel->activeItem = false;

        descriptionLabel = new gui::Label(hBox, 0, 0, 0, 0);
        descriptionLabel->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        descriptionLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Top));
        descriptionLabel->setFont(style::window::font::small);
        descriptionLabel->activeItem = false;

        hourInput = new gui::Text(hBox, 0, 0, 0, 0);
        hourInput->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
        hourInput->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Bottom));
        hourInput->setFont(style::window::font::largelight);
        hourInput->setInputMode(new InputMode({InputMode::digit}));
        hourInput->setPenFocusWidth(style::window::default_border_focus_w);
        hourInput->setPenWidth(style::window::default_border_rect_no_focus);
        hourInput->setEditMode(gui::EditMode::EDIT);

        minuteInput = new gui::Text(hBox, 0, 0, 0, 0);
        minuteInput->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
        minuteInput->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Bottom));
        minuteInput->setFont(style::window::font::largelight);
        minuteInput->setInputMode(new InputMode({InputMode::digit}));
        minuteInput->setPenFocusWidth(style::window::default_border_focus_w);
        minuteInput->setPenWidth(style::window::default_border_rect_no_focus);
        minuteInput->setEditMode(gui::EditMode::EDIT);

        setNavigationItem(NavigationDirection::LEFT, hourInput);
        descriptionLabel->setText(description);

        focusChangedCallback = [&](Item &item) {
            setFocusItem(focus ? hBox : nullptr);
            return true;
        };

        inputCallback = [&](Item &item, const InputEvent &event) { return hourInput->onInput(event); };

        prepareForTimeMode();
        setNavigation();
    }

    void EventTimeItem::prepareForTimeMode()
    {
        if (!mode24H) {
            mode12hInput = new gui::Text(hBox, 0, 0, 0, 0);
            mode12hInput->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
            mode12hInput->setAlignment(
                gui::Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Bottom));
            mode12hInput->setFont(style::window::font::largelight);
            mode12hInput->setInputMode(new InputMode({InputMode::ABC}));
            mode12hInput->setPenFocusWidth(style::window::default_border_focus_w);
            mode12hInput->setPenWidth(style::window::default_border_rect_no_focus);
            mode12hInput->setEditMode(gui::EditMode::EDIT);
            mode12hInput->activatedCallback = [=](gui::Item &item) {
                LOG_INFO("12H mode clicked");
                return true;
            };
        }
    }

    void EventTimeItem::setNavigation()
    {
        if (mode24H) {
            hourInput->setNavigationItem(NavigationDirection::LEFT, minuteInput);
            hourInput->setNavigationItem(NavigationDirection::RIGHT, minuteInput);
            minuteInput->setNavigationItem(NavigationDirection::LEFT, hourInput);
            minuteInput->setNavigationItem(NavigationDirection::RIGHT, hourInput);
        }
        else {
            hourInput->setNavigationItem(NavigationDirection::LEFT, mode12hInput);
            hourInput->setNavigationItem(NavigationDirection::RIGHT, minuteInput);
            minuteInput->setNavigationItem(NavigationDirection::LEFT, hourInput);
            minuteInput->setNavigationItem(NavigationDirection::RIGHT, mode12hInput);
            mode12hInput->setNavigationItem(NavigationDirection::LEFT, minuteInput);
            mode12hInput->setNavigationItem(NavigationDirection::RIGHT, hourInput);
        }
    }

    bool EventTimeItem::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        hBox->setPosition(0, 0);
        hBox->setSize(newDim.w, newDim.h);
        LOG_DEBUG("SIZE TIME ITEM: %i,  %i", newDim.w, newDim.h);
        // sth works wrong when text update..
        descriptionLabel->setArea(BoundingBox(0,
                                              0,
                                              style::window::calendar::item::eventTime::description_label_w,
                                              style::window::calendar::item::eventTime::description_label_h));

        if (!mode24H) {
            colonLabel->setArea(BoundingBox(style::window::calendar::item::eventTime::colon_label_12h_x,
                                            style::window::calendar::item::eventTime::colon_label_y,
                                            style::window::calendar::item::eventTime::colon_label_w,
                                            style::window::calendar::item::eventTime::colon_label_h));
            hourInput->setArea(BoundingBox(style::window::calendar::item::eventTime::hour_input_x,
                                           style::window::calendar::item::eventTime::time_input_y,
                                           style::window::calendar::item::eventTime::time_input_12h_w,
                                           newDim.h - style::window::calendar::item::eventTime::time_input_y));
            minuteInput->setArea(BoundingBox(style::window::calendar::item::eventTime::minute_input_12h_x,
                                             style::window::calendar::item::eventTime::time_input_y,
                                             style::window::calendar::item::eventTime::time_input_12h_w,
                                             newDim.h - style::window::calendar::item::eventTime::time_input_y));
            mode12hInput->setArea(BoundingBox(style::window::calendar::item::eventTime::mode12h_input_x,
                                              style::window::calendar::item::eventTime::time_input_y,
                                              style::window::calendar::item::eventTime::time_input_12h_w,
                                              newDim.h - style::window::calendar::item::eventTime::time_input_y));
        }
        else {
            colonLabel->setArea(BoundingBox(style::window::calendar::item::eventTime::colon_label_24h_x,
                                            style::window::calendar::item::eventTime::colon_label_y,
                                            style::window::calendar::item::eventTime::colon_label_w,
                                            style::window::calendar::item::eventTime::colon_label_h));
            hourInput->setArea(BoundingBox(style::window::calendar::item::eventTime::hour_input_x,
                                           style::window::calendar::item::eventTime::time_input_y,
                                           style::window::calendar::item::eventTime::time_input_24h_w,
                                           newDim.h - style::window::calendar::item::eventTime::time_input_y));
            minuteInput->setArea(BoundingBox(style::window::calendar::item::eventTime::minute_input_24h_x,
                                             style::window::calendar::item::eventTime::time_input_y,
                                             style::window::calendar::item::eventTime::time_input_24h_w,
                                             newDim.h - style::window::calendar::item::eventTime::time_input_y));
        }

        return true;
    }

} /* namespace gui */
