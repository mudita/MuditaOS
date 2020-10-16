#pragma once

#include <BoxLayout.hpp>
#include <i18/i18.hpp>
#include <utf8/UTF8.hpp>
#include <Label.hpp>

namespace style::toggle
{
    const gui::Position toggle_x = 408;
    const gui::Position toggle_y = 24;
} // namespace style::toggle

namespace gui
{
    class Toggle : public HBox
    {
      public:
        Toggle(Item *parent, int x, int y, int w, int h, UTF8 text, bool defaultState);
        ~Toggle() override = default;

        void doToggle();
        bool getState() const noexcept;

      private:
        void buildToggle();

        UTF8 text;

        Label *label     = nullptr;
        Label *indicator = nullptr;

        bool state;
        static constexpr uint toggle_width = 40;
    };
} // namespace gui
