#include "Options.hpp"
#include "Text.hpp"
#include "tools/Common.hpp"
#include <UiCommonActions.hpp>
#include <cassert>
#include <i18/i18.hpp>
#include <utility>

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
            rect->activatedCallback = [this](gui::Item &item) { return app::call(app, contact); };
            rect->addWidget(text);
            center(rect, text, Axis::Y);
            return rect;
        }
    };

}; // namespace gui::option

namespace gui::options
{
    using namespace app;

    Option call(Application *app, CallOperation callOperation, const ContactRecord &contact)
    {
        assert(app != nullptr);
        return Option{std::make_unique<gui::option::Call>(app, contact)};
    }

    Option contact(Application *app,
                   ContactOperation contactOperation,
                   const ContactRecord &contactRec,
                   gui::Arrow arrow)
    {
        assert(app != nullptr);

        std::string str;
        switch (contactOperation) {
        case ContactOperation::Details:
            str = utils::localize.get("app_options_contact_details");
            break;

        case ContactOperation::Add:
            str = utils::localize.get("app_options_contact_add");
            break;

        case ContactOperation::Edit:
            str = utils::localize.get("app_options_contact_edit");
            break;

        default:
            str = utils::localize.get("app_options_invalid_option");
            LOG_WARN("ContactOperation %d not supported", static_cast<int>(contactOperation));
            break;
        }

        return {str, [=](gui::Item &item) { return app::contact(app, contactOperation, contactRec); }, arrow};
    }
} // namespace gui::options
