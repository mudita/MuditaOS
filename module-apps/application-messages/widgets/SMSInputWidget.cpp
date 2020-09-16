#include <module-apps/application-messages/ApplicationMessages.hpp>
#include "SMSInputWidget.hpp"
#include "application-messages/data/MessagesStyle.hpp"

#include <Style.hpp>
#include <i18/i18.hpp>
#include <Font.hpp>
#include <utility>

#include "TextParse.hpp"

namespace gui
{

    SMSInputWidget::SMSInputWidget(Item *parent, app::Application *application) : HBox(parent, 0, 0, 0, 0)
    {

        setMinimumSize(style::window::default_body_width, style::messages::smsInput::min_h);
        setMaximumHeight(style::messages::smsInput::max_input_h);
        setMargins(Margins(0, style::window::messages::new_sms_vertical_spacer, 0, 0));
        setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);

        inputText = new gui::Text(this, 0, 0, 0, 0, "", ExpandMode::EXPAND_UP);
        inputText->setMinimumSize(style::messages::smsInput::default_input_w,
                                  style::messages::smsInput::default_input_h);
        inputText->setMaximumHeight(style::messages::smsInput::max_input_h);
        inputText->setFont(style::window::font::medium);
        inputText->setPadding(Padding(0, 0, 0, style::messages::smsInput::bottom_padding));
        inputText->setPenFocusWidth(style::window::default_border_focus_w);
        inputText->setPenWidth(style::window::default_border_focus_w);
        inputText->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);

        replyImage = new Image(this, 0, 0, "messages_reply");
        replyImage->setAlignment(Alignment(gui::Alignment::Vertical::Bottom));
        replyImage->setMargins(Margins(0, 0, 0, style::messages::smsInput::reply_bottom_margin));
        replyImage->activeItem = false;

        inputText->inputCallback = [=](Item &, const InputEvent &event) {
            if (event.state == InputEvent::State::keyReleasedShort && event.keyCode == KeyCode::KEY_LF) {
                auto app = dynamic_cast<app::ApplicationMessages *>(application);
                assert(app != nullptr);
                return app->newMessageOptions(application->getCurrentWindow()->getName(), inputText);
            }
            return false;
        };

        inputText->focusChangedCallback = [=](Item &) -> bool {
            if (inputText->focus) {

                application->getCurrentWindow()->setBottomBarText(utils::localize.get("sms_reply"),
                                                                  BottomBar::Side::CENTER);

                inputText->setInputMode(new InputMode(
                    {InputMode::ABC, InputMode::abc, InputMode::digit},
                    [=](const UTF8 &Text) { application->getCurrentWindow()->bottomBarTemporaryMode(Text); },
                    [=]() { application->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); },
                    [=]() { application->getCurrentWindow()->selectSpecialCharacter(); }));

                if (inputText->getText().getLine() == utils::localize.get("sms_temp_reply")) {
                    inputText->clear();
                }
            }
            else {

                if (inputText->isEmpty()) {

                    // Temporary solution to be fixed when proper Text Color handling will be added.
                    auto format = TextFormat(Font(27).raw(), Color(7, 0));
                    for (auto &el : textToTextBlocks(utils::localize.get("sms_temp_reply"), format)) {
                        inputText->addText(el);
                    }
                }

                application->getCurrentWindow()->clearBottomBarText(BottomBar::Side::CENTER);
            }

            return true;
        };
    }

} /* namespace gui */
