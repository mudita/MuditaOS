#include "OptionSetting.hpp"
#include "OptionsStyle.hpp"

namespace gui
{
    auto OptionSettings::build() const -> Item *
    {
        auto *label = new gui::Label(nullptr,
                                     style::window::default_left_margin,
                                     0,
                                     style::window_width - 2 * style::window::default_left_margin,
                                     style::window::label::big_h,
                                     text);
        style::window::decorateOption(label);
        label->activatedCallback = activatedCallback;
        std::string img;
        switch (rIcon) {
        case RightIcon::Enabled:
            img = "right_label_arrow";
            break;
        case RightIcon::Border:
            img = "right_label_arrow_border";
            break;
        case RightIcon::On:
            img = "btn_off";
            break;
        case RightIcon::Off:
            img = "btn_off";
            break;
        case RightIcon::Bt:
            img = "bt";
            break;
        default:
            break;
        }

        if (!img.empty()) {
            new gui::Image(label, style::option::arrow_position_x, style::option::arrow_positon_y, 0, 0, img);
        }
        label->focusChangedCallback = focusCb;
        return label;
    }
} // namespace gui
