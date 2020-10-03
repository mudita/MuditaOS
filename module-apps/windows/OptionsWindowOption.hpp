#pragma once

#include <string>
#include <utf8/UTF8.hpp>
#include <functional>
#include <memory>

namespace gui
{
    class Item;

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
    };
} // namespace gui
