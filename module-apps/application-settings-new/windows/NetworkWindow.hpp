#pragma once

#include "OptionWindow.hpp"
#include "Application.hpp"

namespace gui
{

    enum class NetworkArrow : uint8_t
    {
        Disabled,
        Enabled,
        Border,
        On,
        Off
    };

    namespace window
    {
        inline const std::string network_window = "Network";
    };

    class NetworkWindow : public OptionWindow
    {
      private:
        bool operatorsOn;
        auto netOptList() -> std::list<gui::Option>;
        void rebuildOptList();

      public:
        NetworkWindow(app::Application *app);
        void onBeforeShow(ShowMode m, SwitchData *d) override;
    };

    class NetworkOption : public option::Base
    {
      private:
        UTF8 text;
        std::function<bool(Item &)> activatedCallback = nullptr;
        std::function<bool(Item &)> focusCb           = nullptr;
        AppWindow *app                                = nullptr;
        NetworkArrow arrow                            = NetworkArrow::Disabled;

      public:
        NetworkOption(const UTF8 text,
                      std::function<bool(Item &)> cb,
                      std::function<bool(Item &)> cbFocus,
                      AppWindow *app,
                      NetworkArrow arrow = NetworkArrow::Disabled)
            : text(text), activatedCallback(std::move(cb)), focusCb(std::move(cbFocus)), app(app), arrow(arrow)
        {}
        [[nodiscard]] auto build() const -> Item * override;
    };
} // namespace gui
