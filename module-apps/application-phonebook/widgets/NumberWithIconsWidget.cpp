#include "NumberWithIconsWidget.hpp"

#include "application-phonebook/data/PhonebookStyle.hpp"
#include "UiCommonActions.hpp"

#include <BottomBar.hpp>

namespace gui
{
    NumberWithIconsWidget::NumberWithIconsWidget(app::Application *app,
                                                 gui::Item *parent,
                                                 const utils::PhoneNumber::View &number)
    {
        this->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        this->setAlignment(Alignment(gui::Alignment::Horizontal::Right, gui::Alignment::Vertical::Center));
        this->setPenFocusWidth(style::window::default_border_focus_w);
        this->setPenWidth(style::window::messages::sms_border_no_focus);

        numberText = new Text(this, 0, 0, 0, phonebookStyle::multiLineTextWithLabelItem::input_text_h);
        numberText->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        numberText->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Bottom));
        numberText->setFont(style::window::font::medium);
        numberText->setPenFocusWidth(style::window::default_border_focus_w);
        numberText->setPenWidth(style::window::default_border_no_focus_w);

        phoneImage = new Image(this, 0, 0, 32, 32, "phonebook_phone_ringing");
        phoneImage->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM | RectangleEdgeFlags::GUI_RECT_EDGE_TOP);
        phoneImage->inputCallback = [&](Item &item, const InputEvent &input) {
            if (input.keyCode == KeyCode::KEY_ENTER && input.state == InputEvent::State::keyReleasedShort) {
                return app::call(app, number);
            }
            LOG_ERROR("Call operation failed");
            return false;
        };

        smsImage = new Image(this, 0, 0, 32, 32, "mail");
        smsImage->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM | RectangleEdgeFlags::GUI_RECT_EDGE_TOP);
        smsImage->inputCallback = [&](Item &item, const InputEvent &input) {
            if (input.keyCode == KeyCode::KEY_ENTER && input.state == InputEvent::State::keyReleasedShort) {
                return app::sms(app, app::SmsOperation::New, number);
            }
            LOG_ERROR("SMS operation failed");
            return false;
        };
    }

} /* namespace gui */
