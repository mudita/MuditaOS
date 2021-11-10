// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationMessages.hpp"
#include "MessagesStyle.hpp"
#include "OptionsMessages.hpp"
#include "SMSOutputWidget.hpp"

#include <OptionsWindow.hpp>
#include <OptionWindow.hpp>
#include <Style.hpp>
#include <time/time_conversion_factory.hpp>
#include <SMSdata.hpp>

namespace gui
{

    SMSOutputWidget::SMSOutputWidget(app::ApplicationCommon *application, const std::shared_ptr<SMSRecord> &record)
    {
        setMinimumSize(style::window::default_body_width, style::messages::smsOutput::default_h);
        setMargins(Margins(0, style::messages::smsOutput::sms_vertical_spacer, 0, 0));
        setEdges(gui::RectangleEdge::None);

        body = new HBox(this, 0, 0, 0, 0);
        body->setEdges(RectangleEdge::None);
        body->setMaximumSize(style::window::default_body_width, style::messages::smsOutput::sms_max_height);

        smsBubble = new TextBubble(nullptr, 0, 0, 0, 0);
        smsBubble->setMaximumSize(style::messages::smsOutput::sms_max_width,
                                  style::messages::smsOutput::sms_max_height);
        smsBubble->setAlignment(Alignment(Alignment::Vertical::Center));
        smsBubble->setTextType(TextType::MultiLine);
        smsBubble->setRadius(style::messages::smsOutput::sms_radius);
        smsBubble->setFont(style::window::font::medium);
        smsBubble->setPenFocusWidth(style::window::default_border_focus_w);
        smsBubble->setPenWidth(style::window::default_border_rect_no_focus);
        smsBubble->setPadding(style::messages::smsOutput::sms_right_bubble_padding);

        switch (record->type) {
        case SMSType::QUEUED:
            // Handle in the same way as case below. (pending sending display as already sent)
            [[fallthrough]];
        case SMSType::OUTBOX:
            smsBubble->setYaps(RectangleYap::TopRight);
            body->setReverseOrder(true);
            body->addWidget(smsBubble);
            timeLabelBuild(record->date);
            break;
        case SMSType::INBOX:
            smsBubble->setPadding(style::messages::smsOutput::sms_left_bubble_padding);
            smsBubble->setYaps(RectangleYap::TopLeft);
            body->setReverseOrder(false);
            body->addWidget(smsBubble);
            timeLabelBuild(record->date);
            break;
        case SMSType::FAILED:
            smsBubble->setYaps(RectangleYap::TopRight);
            body->setReverseOrder(true);
            errorIconBuild();
            body->addWidget(smsBubble);
            break;
        case SMSType::DRAFT:
            LOG_ERROR("Can't handle Draft type message in smsBubble");
            break;
        default:
            break;
        }

        smsBubble->setText(record->body);

        focusChangedCallback = [this]([[maybe_unused]] Item &item) {
            setFocusItem(focus ? body : nullptr);
            return false;
        };

        body->focusChangedCallback = [this]([[maybe_unused]] Item &item) {
            if (timeLabel != nullptr) {
                timeLabel->setVisible(focus);
                body->resizeItems();
            }
            return true;
        };

        inputCallback = [&]([[maybe_unused]] Item &item, const InputEvent &event) { return smsBubble->onInput(event); };

        smsBubble->inputCallback = [application, record](Item &, const InputEvent &event) {
            if (event.isShortRelease(KeyCode::KEY_LF)) {
                LOG_INFO("Message activated!");
                auto app = dynamic_cast<app::ApplicationMessages *>(application);
                assert(app != nullptr);
                app->switchWindow(gui::name::window::sms_options, std::make_unique<SMSSwitchData>(*record));
                return true;
            }
            return false;
        };

        dimensionChangedCallback = [&](gui::Item &, const BoundingBox &newDim) -> bool {
            body->setArea({0, 0, newDim.w, newDim.h});

            // We need to calculate margin between sms and timeLabel and we can do it only after sizes are set.
            positionTimeLabel();

            return true;
        };
    }

    void SMSOutputWidget::positionTimeLabel() const
    {
        if (timeLabel != nullptr) {
            timeLabel->setMinimumWidth(timeLabel->getTextNeedSpace());
            timeLabel->setMinimumHeight(style::messages::smsOutput::default_h);
            uint16_t timeLabelMargin = body->getWidth() - (smsBubble->getWidth() + timeLabel->getTextNeedSpace());

            if (body->getReverseOrder()) {
                timeLabel->setMargins(Margins(0, 0, timeLabelMargin, 0));
            }
            else {
                timeLabel->setMargins(Margins(timeLabelMargin, 0, 0, 0));
            }

            body->resizeItems();
        }
    }

    void SMSOutputWidget::timeLabelBuild(time_t timestamp)
    {
        timeLabel             = new gui::Label(body, 0, 0, 0, 0);
        timeLabel->activeItem = false;
        timeLabel->setFont(style::window::font::verysmall);
        using namespace utils::time;
        timeLabel->setText(*TimestampFactory().createTimestamp(TimestampType::Time, timestamp));
        timeLabel->setVisible(false);
        timeLabel->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        timeLabel->setEdges(RectangleEdge::None);
    }

    void SMSOutputWidget::errorIconBuild()
    {
        errorIcon = new gui::Image("messages_error_W_M");
        errorIcon->setAlignment(Alignment(Alignment::Vertical::Center));
        errorIcon->activeItem = false;
        errorIcon->setMargins(Margins(style::messages::smsOutput::sms_error_icon_left_margin,
                                      0,
                                      style::messages::smsOutput::sms_error_icon_right_margin,
                                      0));
        body->addWidget(errorIcon);
    }

    auto SMSOutputWidget::handleRequestResize([[maybe_unused]] const Item *child, Length request_w, Length request_h)
        -> Size
    {
        setMinimumHeight(request_h);
        return Size(request_w, request_h);
    }

} /* namespace gui */
