#pragma once

#include <Alignment.hpp>
#include <gui/core/Color.hpp>
#include <inttypes.h>
#include <string>

namespace gui
{
    class Label;
};

namespace style {
const inline uint32_t window_height = 600;
const inline uint32_t window_width  = 480;
namespace header {
namespace font {
const inline std::string time  = "gt_pressura_regular_24";
const inline std::string title = "gt_pressura_bold_32";
};  // namespace font
};  // namespace header
namespace window {
const inline uint32_t default_left_margin = 30;
const inline uint32_t default_right_margin = 30;
const inline uint32_t default_border_focucs_w   = 2;
const inline uint32_t default_border_no_focus_w = 0;
namespace font {
const inline std::string verybig = "gt_pressura_light_46";
const inline std::string bigbold = "gt_pressura_bold_30";
const inline std::string big     = "gt_pressura_regular_30";
const inline std::string mediumbold = "gt_pressura_bold_27";
const inline std::string medium  = "gt_pressura_regular_27";
const inline std::string small   = "gt_pressura_regular_24";
const inline std::string verysmall   = "gt_pressura_regular_20";
};  // namespace font

namespace label {
const inline uint32_t small_h           = 33;
const inline uint32_t default_h         = 50;
const inline uint32_t big_h             = 55;
};  // namespace label

/// minimal label decoration - edges, focus & alignment
void decorate(gui::Label *el);
/// minimal label decoration for Option
void decorateOption(gui::Label *el);

};  // namespace window
namespace footer {
namespace font {
const inline std::string bold   = "gt_pressura_bold_24";
const inline std::string medium = "gt_pressura_regular_24";
};  // namespace font
};  // namespace footer
namespace phonebook
{
namespace font
{
const inline std::string tiny = "gt_pressura_regular_16";
const inline std::string tinybold = "gt_pressura_bold_16";
const inline std::string tinylight = "gt_pressura_light_16";
}; // namespace font
}; // namespace phonebook
namespace color
{
const inline gui::Color lightgrey = gui::Color(3, 0);
}; //  namespace color
namespace text
{
const inline gui::Alignment defaultTextAlignment = gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_LEFT, gui::Alignment::ALIGN_VERTICAL_BOTTOM);
};
};  // namespace style
