#pragma once

#include "OptionWindow.hpp"
#include "Application.hpp"

namespace gui
{
    enum class RightIcon : uint8_t
    {
        Disabled,
        Enabled,
        Border,
        On,
        Off
    };

    class OptionSettings : public option::Base
    {
      private:
        UTF8 text;
        std::function<bool(Item &)> activatedCallback = nullptr;
        std::function<bool(Item &)> focusCb           = nullptr;
        AppWindow *app                                = nullptr;
        RightIcon rIcon                               = RightIcon::Disabled;

      public:
        OptionSettings(const UTF8 text,
                       std::function<bool(Item &)> cb,
                       std::function<bool(Item &)> cbFocus,
                       AppWindow *app,
                       RightIcon rIcon = RightIcon::Disabled)
            : text(text), activatedCallback(std::move(cb)), focusCb(std::move(cbFocus)), app(app), rIcon(rIcon)
        {}
        [[nodiscard]] auto build() const -> Item * override;
    };
} // namespace gui
