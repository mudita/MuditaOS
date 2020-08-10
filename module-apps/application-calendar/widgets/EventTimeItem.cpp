#include "EventTimeItem.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include <ListView.hpp>
#include <Style.hpp>
#include <Utils.hpp>

namespace gui
{

    EventTimeItem::EventTimeItem(const std::string &description,
                                 bool mode24H,
                                 std::function<void(const UTF8 &text)> bottomBarTemporaryMode,
                                 std::function<void()> bottomBarRestoreFromTemporaryMode)
        : mode24H{mode24H}, bottomBarTemporaryMode(std::move(bottomBarTemporaryMode)),
          bottomBarRestoreFromTemporaryMode(std::move(bottomBarRestoreFromTemporaryMode))
    {
        setMinimumSize(style::window::default_body_width, style::window::calendar::item::eventTime::height);

        setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        setMargins(gui::Margins(0, style::window::calendar::item::eventTime::margin, 0, 0));

        vBox = new gui::VBox(this, 0, 0, 0, 0);
        vBox->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);

        descriptionLabel = new gui::Label(vBox, 0, 0, 0, 0);
        descriptionLabel->setMinimumSize(style::window::default_body_width,
                                         style::window::calendar::item::eventTime::description_label_h);
        descriptionLabel->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        descriptionLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        descriptionLabel->setFont(style::window::font::small);
        descriptionLabel->activeItem = false;

        hBox = new gui::HBox(vBox, 0, 0, 0, 0);
        hBox->setMinimumSize(style::window::default_body_width,
                             style::window::calendar::item::eventTime::height -
                                 style::window::calendar::item::eventTime::description_label_h);
        hBox->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);

        hourInput = new gui::Text(hBox, 0, 0, 0, 0);
        hourInput->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
        hourInput->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        hourInput->setFont(style::window::font::largelight);
        hourInput->setInputMode(new InputMode({InputMode::digit}));
        hourInput->setPenFocusWidth(style::window::default_border_focus_w);
        hourInput->setPenWidth(style::window::default_border_rect_no_focus);
        hourInput->setEditMode(gui::EditMode::EDIT);

        colonLabel = new gui::Label(hBox, 0, 0, 0, 0);
        colonLabel->setMinimumSize(style::window::calendar::item::eventTime::colon_label_w,
                                   style::window::calendar::item::eventTime::height -
                                       style::window::calendar::item::eventTime::description_label_h);
        colonLabel->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        colonLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        colonLabel->setFont(style::window::font::medium);
        colonLabel->setText(":");
        colonLabel->activeItem = false;

        minuteInput = new gui::Text(hBox, 0, 0, 0, 0);
        minuteInput->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
        minuteInput->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        minuteInput->setFont(style::window::font::largelight);
        minuteInput->setInputMode(new InputMode({InputMode::digit}));
        minuteInput->setPenFocusWidth(style::window::default_border_focus_w);
        minuteInput->setPenWidth(style::window::default_border_rect_no_focus);
        minuteInput->setEditMode(gui::EditMode::EDIT);

        setNavigationItem(NavigationDirection::LEFT, hourInput);
        descriptionLabel->setText(description);

        focusChangedCallback = [&](Item &item) {
            LOG_DEBUG("Event time focus changed");
            setFocusItem(focus ? hourInput : nullptr);
            return true;
        };

        inputCallback = [&](Item &item, const InputEvent &event) {
            auto focusedItem = getFocusItem();
            if (event.state != gui::InputEvent::State::keyReleasedShort) {
                return false;
            }
            return focusedItem->onInput(event);
        };

        prepareForTimeMode();
        setNavigation();
    }

    void EventTimeItem::prepareForTimeMode()
    {
        if (!mode24H) {
            mode12hInput = new gui::Text(hBox, 0, 0, 0, 0);
            mode12hInput->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
            mode12hInput->setAlignment(
                gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
            mode12hInput->setFont(style::window::font::largelight);
            mode12hInput->setInputMode(new InputMode({InputMode::ABC}));
            mode12hInput->setPenFocusWidth(style::window::default_border_focus_w);
            mode12hInput->setPenWidth(style::window::default_border_rect_no_focus);
            mode12hInput->setEditMode(gui::EditMode::EDIT);
            mode12hInput->activatedCallback = [=](gui::Item &item) {
                LOG_INFO("12H mode clicked");
                return true;
            };
            mode12hInput->focusChangedCallback = [&](Item &item) {
                if (focus) {
                    LOG_DEBUG("Focus 12H");
                    bottomBarTemporaryMode("SWITCH");
                }
                else {
                    LOG_DEBUG("Lost focus 12H");
                    bottomBarRestoreFromTemporaryMode();
                }
                return true;
            };

            mode12hInput->setMinimumSize(style::window::calendar::item::eventTime::time_input_12h_w,
                                         style::window::calendar::item::eventTime::height -
                                             style::window::calendar::item::eventTime::description_label_h);
            mode12hInput->setMargins(
                gui::Margins(style::window::calendar::item::eventTime::mode12h_input_margin, 0, 0, 0));
            hourInput->setMinimumSize(style::window::calendar::item::eventTime::time_input_12h_w,
                                      style::window::calendar::item::eventTime::height -
                                          style::window::calendar::item::eventTime::description_label_h);
            minuteInput->setMinimumSize(style::window::calendar::item::eventTime::time_input_12h_w,
                                        style::window::calendar::item::eventTime::height -
                                            style::window::calendar::item::eventTime::description_label_h);
        }
        else {
            hourInput->setMinimumSize(style::window::calendar::item::eventTime::time_input_24h_w,
                                      style::window::calendar::item::eventTime::height -
                                          style::window::calendar::item::eventTime::description_label_h);
            minuteInput->setMinimumSize(style::window::calendar::item::eventTime::time_input_24h_w,
                                        style::window::calendar::item::eventTime::height -
                                            style::window::calendar::item::eventTime::description_label_h);
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
        vBox->setPosition(0, 0);
        vBox->setSize(newDim.w, newDim.h);
        return true;
    }

} /* namespace gui */
