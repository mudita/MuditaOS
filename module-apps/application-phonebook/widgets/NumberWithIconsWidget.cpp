#include "NumberWithIconsWidget.hpp"

#include "application-phonebook/data/PhonebookStyle.hpp"
#include "UiCommonActions.hpp"

#include <BottomBar.hpp>

namespace gui
{
    NumberWithIconsWidget::NumberWithIconsWidget(app::Application *app,
                                                 const utils::PhoneNumber::View &number,
                                                 Item *parent,
                                                 const uint32_t &x,
                                                 const uint32_t &y,
                                                 const uint32_t &w,
                                                 const uint32_t &h)
        : HBox(parent, x, y, w, h)
    {
        setReverseOrder(true);
        setEdges(gui::RectangleEdgeFlags::GUI_RECT_ALL_EDGES);
        setAlignment(Alignment(gui::Alignment::Horizontal::Right, gui::Alignment::Vertical::Center));
        setPenFocusWidth(style::window::default_border_focus_w);
        setPenWidth(style::window::default_border_no_focus_w);

        smsImage                = new ImageBox(this,
                                0,
                                0,
                                phonebookStyle::numbersWithIconsWidget::sms_image_w,
                                phonebookStyle::numbersWithIconsWidget::sms_image_h,
                                new Image("mail"));
        smsImage->inputCallback = [&](Item &item, const InputEvent &input) {
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
        phoneImage->setMargins(Margins(phonebookStyle::numbersWithIconsWidget::phone_image_margin_left, 0, 0, 0));
        phoneImage->inputCallback = [&](Item &item, const InputEvent &input) {
            if (input.keyCode == KeyCode::KEY_ENTER && input.state == InputEvent::State::keyReleasedShort) {
                return app::call(app, number);
                LOG_INFO("Call operation started");
            }
            return false;
        };

        numberText = new Text(this, 0, 0, 0, 0);
        numberText->setMaximumSize(w, phonebookStyle::numbersWithIconsWidget::number_text_h);
        numberText->setFont(style::window::font::medium);
        numberText->setPenFocusWidth(style::window::default_border_focus_w);
        numberText->setPenWidth(style::window::default_border_no_focus_w);
        numberText->setEditMode(EditMode::BROWSE);
        numberText->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        numberText->setText(number.getFormatted());

        inputCallback = [&](gui::Item &item, const gui::InputEvent &event) {
            if (event.state != gui::InputEvent::State::keyReleasedShort) {
                return false;
            }

            LOG_INFO("jestem w dziecku?");

            return false;
        };
    }

    auto NumberWithIconsWidget::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) -> bool
    {
        resizeItems();
        return true;
    }

} /* namespace gui */
