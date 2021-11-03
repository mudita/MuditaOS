// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationMessages.hpp"
#include "MessagesStyle.hpp"

#include <AppWindow.hpp>
#include <Font.hpp>
#include <i18n/i18n.hpp>
#include <service-cellular/service-cellular/MessageConstants.hpp>
#include <SMSInputWidget.hpp>
#include <Style.hpp>
#include <parsers/TextParse.hpp>

#include <algorithm>
#include <utility>

namespace gui
{

    SMSInputWidget::SMSInputWidget(app::ApplicationCommon *application) : application(application)
    {
        setMinimumSize(style::window::default_body_width, style::messages::smsInput::min_h);
        setMargins(Margins(
            style::messages::smsInput::new_sms_left_margin, style::messages::smsInput::new_sms_vertical_spacer, 0, 0));
        setEdges(gui::RectangleEdge::None);

        body = new HBox(this, 0, 0, 0, 0);
        body->setEdges(RectangleEdge::Bottom);
        body->setMaximumSize(style::window::default_body_width, style::messages::smsInput::max_input_h);

        deleteByList = false;
        inputText    = new gui::Text(body, 0, 0, 0, 0, ExpandMode::Up);
        inputText->setMaximumSize(style::messages::smsInput::default_input_w, style::messages::smsInput::max_input_h);
        inputText->setMinimumSize(style::messages::smsInput::default_input_w,
                                  style::messages::smsInput::default_input_h);
        inputText->setTextLimitType(gui::TextLimitType::MaxSignsCount, msgConstants::maxConcatenatedLen);
        inputText->setFont(style::window::font::medium);
        inputText->setPadding(Padding(0, 0, 0, style::messages::smsInput::bottom_padding));
        inputText->setPenFocusWidth(style::window::default_border_focus_w);
        inputText->setPenWidth(style::window::default_border_focus_w);
        inputText->setEdges(gui::RectangleEdge::None);

        replyImage = new Image(body, 0, 0, "messages_reply");
        replyImage->setAlignment(Alignment(gui::Alignment::Vertical::Bottom));
        replyImage->setMargins(Margins(0, 0, 0, style::messages::smsInput::reply_bottom_margin));
        replyImage->activeItem = false;

        inputCallback = [&]([[maybe_unused]] Item &item, const InputEvent &event) { return inputText->onInput(event); };

        focusChangedCallback = [this]([[maybe_unused]] Item &item) {
            setFocusItem(focus ? body : nullptr);
            return true;
        };

        inputText->setInputMode(new InputMode(
            {InputMode::ABC, InputMode::abc, InputMode::digit},
            [=](const UTF8 &Text) { application->getCurrentWindow()->navBarTemporaryMode(Text); },
            [=]() { application->getCurrentWindow()->navBarRestoreFromTemporaryMode(); },
            [=]() { application->getCurrentWindow()->selectSpecialCharacter(); }));

        inputText->inputCallback = [this, application]([[maybe_unused]] Item &, const InputEvent &event) {
            if (event.isShortRelease(KeyCode::KEY_LF)) {
                auto app = dynamic_cast<app::ApplicationMessages *>(application);
                assert(app != nullptr);
                return app->newMessageOptions(application->getCurrentWindow()->getName(), inputText);
            }
            return false;
        };

        inputText->focusChangedCallback = [this, application]([[maybe_unused]] Item &) -> bool {
            assert(body != nullptr);
            assert(application != nullptr);

            if (inputText->focus) {

                application->getWindow(gui::name::window::thread_view)
                    ->setNavBarText(utils::translate(utils::translate(style::strings::common::send)),
                                    nav_bar::Side::Center);

                if (inputText->getText() == utils::translate("sms_temp_reply")) {
                    inputText->clear();
                }
            }
            else {

                if (inputText->isEmpty()) {

                    // Temporary solution to be fixed when proper Text Color handling will be added.
                    auto format = TextFormat(Font(27).raw(), Color(7, 0));
                    for (auto &el : textToTextBlocks(utils::translate("sms_temp_reply"), format)) {
                        inputText->addText(el);
                    }
                }

                application->getWindow(gui::name::window::thread_view)
                    ->setNavBarText(utils::translate(style::strings::common::reply), nav_bar::Side::Center);
            }

            return true;
        };

        dimensionChangedCallback = [&](gui::Item &, const BoundingBox &newDim) -> bool {
            if (newDim.w == style::listview::item_width_with_scroll - style::messages::smsInput::new_sms_left_margin) {
                inputText->setMinimumWidth(style::messages::smsInput::default_input_w);
                inputText->setMaximumWidth(style::messages::smsInput::default_input_w);
            }
            else {
                inputText->setMinimumWidth(style::messages::smsInput::default_input_w +
                                           style::listview::scroll::item_margin);
                inputText->setMaximumWidth(style::messages::smsInput::default_input_w +
                                           style::listview::scroll::item_margin);
            }
            body->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };
    }

    void SMSInputWidget::handleDraftMessage()
    {
        if (const auto &text = inputText->getText(); text.empty() || (text == utils::translate("sms_temp_reply"))) {
            clearDraftMessage();
        }
        else {
            updateDraftMessage(text);
        }
    }

    void SMSInputWidget::clearDraftMessage()
    {
        if (!draft.has_value()) {
            displayDraftMessage();
            return;
        }

        auto app = dynamic_cast<app::ApplicationMessages *>(application);
        assert(app != nullptr);
        if (const auto removed = app->removeDraft(draft.value()); removed) {
            draft = std::nullopt;
            displayDraftMessage();
        }
    }

    void SMSInputWidget::displayDraftMessage() const
    {
        if (draft.has_value()) {
            inputText->setText(draft->body);
        }
        else {
            inputText->clear();
        }
    }

    void SMSInputWidget::updateDraftMessage(const UTF8 &inputText)
    {
        auto app = dynamic_cast<app::ApplicationMessages *>(application);
        assert(app != nullptr);
        assert(number != nullptr);

        if (draft.has_value()) {
            app->updateDraft(draft.value(), inputText);
        }
        else {
            app->createDraft(*number, inputText, [this](const SMSRecord &record) { draft = record; });
        }
    }

    auto SMSInputWidget::handleRequestResize([[maybe_unused]] const Item *child, Length request_w, Length request_h)
        -> Size
    {
        request_h =
            std::clamp((Length)request_h, style::messages::smsInput::min_h, style::messages::smsInput::max_input_h);

        setMinimumHeight(request_h);
        if (parent != nullptr) {
            requestSize(request_w, request_h);
        }

        return Size(request_w, request_h);
    }

} /* namespace gui */
