#include "NumberWithIconsWidget.hpp"

#include "application-phonebook/data/PhonebookStyle.hpp"
#include "UiCommonActions.hpp"

#include <BottomBar.hpp>

namespace gui
{
    NumberWithIconsWidget::NumberWithIconsWidget(app::Application *app,
                                                 const utils::PhoneNumber::View &number,
                                                 const std::string &font,
                                                 Item *parent)
        : HBox(parent, 0, 0, 0, 0)
    {
        setReverseOrder(true);
        setMinimumSize(phonebookStyle::informationWidget::w, phonebookStyle::numbersWithIconsWidget::h);
        setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        setAlignment(Alignment(gui::Alignment::Horizontal::Right, gui::Alignment::Vertical::Center));

        smsImage                = new ImageBox(this,
                                0,
                                0,
                                phonebookStyle::numbersWithIconsWidget::sms_image_w,
                                phonebookStyle::numbersWithIconsWidget::sms_image_h,
                                new Image("mail"));
        smsImage->inputCallback = [=](Item &item, const InputEvent &input) {
            if (input.keyCode == KeyCode::KEY_ENTER && input.state == InputEvent::State::keyReleasedShort) {
                return app::sms(app, app::SmsOperation::New, number);
                LOG_INFO("SMS operation started");
            }
            return false;
        };

        phoneImage = new ImageBox(this,
                                  0,
                                  0,
                                  phonebookStyle::numbersWithIconsWidget::phone_image_w,
                                  phonebookStyle::numbersWithIconsWidget::phone_image_h,
                                  new Image("phonebook_phone_ringing"));
        phoneImage->setMargins(Margins(phonebookStyle::numbersWithIconsWidget::phone_image_margin_left,
                                       0,
                                       phonebookStyle::numbersWithIconsWidget::phone_image_margin_right,
                                       0));
        phoneImage->inputCallback = [=](Item &item, const InputEvent &input) {
            if (input.keyCode == KeyCode::KEY_ENTER && input.state == InputEvent::State::keyReleasedShort) {
                return app::call(app, number);
                LOG_INFO("Call operation started");
            }
            return false;
        };

        numberText = new TextFixedSize(this, 0, 0, 0, 0);
        numberText->setUnderline(false);
        numberText->setMaximumSize(phonebookStyle::informationWidget::w,
                                   phonebookStyle::numbersWithIconsWidget::number_text_h);
        numberText->setFont(font);
        numberText->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        numberText->setEditMode(EditMode::BROWSE);
        numberText->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        numberText->setText(number.getFormatted());
        numberText->activeItem = false;

        focusChangedCallback = [&](Item &item) {
            setFocusItem(focus ? phoneImage : nullptr);
            return true;
        };

        phoneImage->focusChangedCallback = [&, app](Item &item) {
            if (phoneImage->focus) {
                phoneImage->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM | RectangleEdgeFlags::GUI_RECT_EDGE_TOP);
                app->getCurrentWindow()->bottomBarTemporaryMode(
                    utils::localize.get(style::strings::common::call), BottomBar::Side::CENTER, false);
            }
            else {
                phoneImage->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
            }
            return true;
        };

        smsImage->focusChangedCallback = [&, app](Item &item) {
            if (smsImage->focus) {
                smsImage->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM | RectangleEdgeFlags::GUI_RECT_EDGE_TOP);
                app->getCurrentWindow()->bottomBarTemporaryMode(
                    utils::localize.get(style::strings::common::send), BottomBar::Side::CENTER, false);
            }
            else {
                smsImage->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
            }
            return true;
        };
    }
} /* namespace gui */
