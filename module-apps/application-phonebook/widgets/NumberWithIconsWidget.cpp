#include "NumberWithIconsWidget.hpp"

#include "application-phonebook/data/PhonebookStyle.hpp"

#include <BottomBar.hpp>

namespace gui
{
    NumberWithIconsWidget::NumberWithIconsWidget(gui::Item *parent, const std::string &number)
    {
        hBox = new HBox(parent, 0, 0, style::window::default_body_width, 0);
        hBox->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        hBox->setAlignment(Alignment(gui::Alignment::Horizontal::Right, gui::Alignment::Vertical::Center));
        hBox->setPenFocusWidth(style::window::default_border_focus_w);
        hBox->setPenWidth(style::window::messages::sms_border_no_focus);

        numberText = new Text(hBox, 0, 0, 0, phonebookStyle::multiLineTextWithLabelItem::input_text_h);
        numberText->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        numberText->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Bottom));
        numberText->setFont(style::window::font::medium);
        numberText->setPenFocusWidth(style::window::default_border_focus_w);
        numberText->setPenWidth(style::window::default_border_no_focus_w);

        phoneImage = new Image(hBox, 0, 0, 32, 32, "phonebook_phone_ringing");

        smsImage = new Image(hBox, 0, 0, 32, 32, "mail");
    }

} /* namespace gui */
