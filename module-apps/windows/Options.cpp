// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Options.hpp"
#include "Text.hpp"
#include "tools/Common.hpp"
#include <cassert>
#include <module-utils/i18n/i18n.hpp>
#include <utility>
#include <FontManager.hpp>

#include <service-appmgr/Controller.hpp>
#include <application-call/data/CallSwitchData.hpp>
#include <module-apps/application-phonebook/data/PhonebookItemData.hpp>

namespace style::option
{
    const inline gui::Length text_left_padding = 10;
}

namespace gui::option
{
    /// builder for call option
    /// creates text with caller in bold
    class Call : public Base
    {
      private:
        app::Application *app = nullptr;
        ContactRecord contact;

      public:
        Call(app::Application *app, ContactRecord contact) : app(app), contact(std::move(contact))
        {}

        [[nodiscard]] auto build() const -> Item * override
        {
            auto *rect     = new gui::Rect(nullptr,
                                       style::window::default_left_margin,
                                       0,
                                       style::window_width - 2 * style::window::default_right_margin,
                                       style::window::label::big_h);
            auto font      = FontManager::getInstance().getFont(style::window::font::medium);
            auto font_bold = FontManager::getInstance().getFont(style::window::font::mediumbold);
            auto text      = new Text(nullptr, style::option::text_left_padding, 0, 0, 0);
            text->setMaximumSize(rect->getWidth(), rect->getHeight());
            text->setEditMode(EditMode::BROWSE);
            text->setText(std::make_unique<TextDocument>(std::list<TextBlock>(
                {{utils::localize.get("sms_call_text"), font}, {contact.getFormattedName(), font_bold}})));
            style::window::decorate(rect);
            auto l_app              = app;
            auto l_contact          = contact;
            rect->activatedCallback = [l_app, l_contact](gui::Item &item) {
                if (!l_contact.numbers.empty()) {
                    const auto &phoneNumber = l_contact.numbers.front().number;
                    return app::manager::Controller::sendAction(
                        l_app, app::manager::actions::Dial, std::make_unique<app::ExecuteCallData>(phoneNumber));
                }
                return false;
            };
            rect->addWidget(text);
            center(rect, text, Axis::Y);
            return rect;
        }
    };
} // namespace gui::option

namespace gui::options
{
    namespace
    {
        bool onContactOptionClick(app::Application *app,
                                  ContactOperation contactOperation,
                                  const ContactRecord &contactRecord)
        {
            auto data = std::make_unique<PhonebookItemData>(std::make_shared<ContactRecord>(contactRecord));

            switch (contactOperation) {
            case ContactOperation::Add: {
                data->ignoreCurrentWindowOnStack = true;
                return app::manager::Controller::sendAction(app,
                                                            app::manager::actions::AddContact,
                                                            std::move(data),
                                                            app::manager::OnSwitchBehaviour::RunInBackground);
            }
            case ContactOperation::Details: {
                return app::manager::Controller::sendAction(app,
                                                            app::manager::actions::ShowContactDetails,
                                                            std::move(data),
                                                            app::manager::OnSwitchBehaviour::RunInBackground);
            }
            case ContactOperation::Edit: {
                return app::manager::Controller::sendAction(app,
                                                            app::manager::actions::EditContact,
                                                            std::move(data),
                                                            app::manager::OnSwitchBehaviour::RunInBackground);
            }
            }
            LOG_ERROR("ContactOperation not supported %" PRIu32, static_cast<uint32_t>(contactOperation));
            return false;
        }
    } // namespace

    Option call(app::Application *app, const ContactRecord &contact)
    {
        assert(app != nullptr);
        return Option{std::make_unique<gui::option::Call>(app, contact)};
    }

    Option contact(app::Application *app,
                   ContactOperation contactOperation,
                   const ContactRecord &contactRec,
                   gui::Arrow arrow)
    {
        assert(app != nullptr);

        std::string optionName;
        switch (contactOperation) {
        case ContactOperation::Details:
            optionName = utils::localize.get("app_options_contact_details");
            break;
        case ContactOperation::Add:
            optionName = utils::localize.get("app_options_contact_add");
            break;
        case ContactOperation::Edit:
            optionName = utils::localize.get("app_options_contact_edit");
            break;
        default:
            optionName = utils::localize.get("app_options_invalid_option");
            LOG_WARN("ContactOperation %d not supported", static_cast<int>(contactOperation));
            break;
        }

        return Option{
            optionName, [=](gui::Item &) { return onContactOptionClick(app, contactOperation, contactRec); }, arrow};
    }
} // namespace gui::options
