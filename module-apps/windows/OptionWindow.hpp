#pragma once

#include "../Application.hpp"
#include "AppWindow.hpp"
#include "PageLayout.hpp"
#include <functional>

#include <log/log.hpp>

namespace gui
{
    enum class Arrow : bool
    {
        Disabled,
        Enabled
    };

    namespace option
    {
        class Base
        {
          public:
            virtual ~Base()                                    = default;
            [[nodiscard]] virtual auto build() const -> Item * = 0;
            [[nodiscard]] virtual auto str() const -> std::string
            {
                return "";
            };
        };

        class Simple : public Base
        {
          private:
            const UTF8 text                               = "";
            std::function<bool(Item &)> activatedCallback = nullptr;
            Arrow arrow                                   = Arrow::Disabled;

          public:
            Simple(const UTF8 text, std::function<bool(Item &)> activatedCallback, Arrow arrow)
                : text(text), activatedCallback(activatedCallback), arrow(arrow)
            {}

            [[nodiscard]] auto build() const -> Item * override;
        };
    }; // namespace option

    struct Option
    {
      private:
        std::unique_ptr<option::Base> option;

      public:
        Option(std::unique_ptr<option::Base> option) : option(std::move(option))
        {}
        /// old one
        Option(const UTF8 text, std::function<bool(Item &)> cb, Arrow arrow = Arrow::Disabled)
            : Option(std::make_unique<option::Simple>(text, cb, arrow))
        {}

        Option(const Option &o) = delete;

        Option(Option &&o)
        {
            this->option = std::move(o.option);
        }

        [[nodiscard]] auto build() const -> Item *;
        //[[nodiscard]] auto str() const { return option->str(); }
    };

    /// creates new `option` label on heap with text description and on activated callback connected
    Item *newOptionLabel(const UTF8 &text,
                         std::function<bool(Item &)> activatedCallback,
                         Arrow arrow = Arrow::Disabled);

    class OptionWindow : public AppWindow
    {

      protected:
        PageLayout *body = nullptr;

      public:
        OptionWindow(app::Application *app, const std::string &name);
        virtual ~OptionWindow();

        void addOptionLabel(const UTF8 &text, std::function<bool(Item &)> activatedCallback, Arrow arrow);
        /// add options as {text, on click callback }
        void addOptions(std::list<Option> options);
        /// add options as list of items on heap
        /// [!] ownership on item will be moved to OptionWindow (so it will clean created kids on heap)
        void addOptions(std::list<Item *> items);
        void clearOptions();

        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
    };

    /// default name will set name to Options from i18
    OptionWindow *newOptionWindow(app::Application *app, std::string name = "", std::list<Option> options = {});

}; // namespace gui
