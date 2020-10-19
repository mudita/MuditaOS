#pragma once
#include <CheckBox.hpp>
#include <Label.hpp>

namespace style::checkbox
{
    const inline int check_x           = 380;
    const inline int check_y           = 12;
    const inline int description_width = 340;
    const inline int check_width       = 60;
} // namespace style::checkbox

namespace gui
{
    class CheckBoxWithLabel : public gui::Item
    {
      public:
        CheckBoxWithLabel(Item *parent,
                          int x,
                          int y,
                          UTF8 description,
                          std::function<void(CheckBoxWithLabel &)> checkCallback = nullptr);

        ~CheckBoxWithLabel() override = default;

        void setChecked(bool state);
        auto isChecked() const -> bool;

      private:
        gui::CheckBox *check = nullptr;
        gui::Label *label    = nullptr;
    };
} // namespace gui
