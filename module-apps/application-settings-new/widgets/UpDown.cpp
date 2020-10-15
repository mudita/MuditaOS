#include "UpDown.hpp"

#include <Label.hpp>
#include <Image.hpp>

namespace gui
{
    UpDown::UpDown(Item *parent, int x, int y, int w, int h, UTF8 name, uint32_t defaultValue, uint32_t max)
        : HBox(parent, x, y, w, h), value(defaultValue), maxValue(max)
    {
        setEdges(gui::RectangleEdge::None);
        build(name);
    }

    void UpDown::build(UTF8 text)
    {
        removeWidget(label);

        label = new gui::Label(nullptr,
                               0,
                               0,
                               style::window_width - style::window::default_right_margin,
                               style::window::label::big_h,
                               text);
        label->setEdges(gui::RectangleEdge::None);
        style::window::decorateOption(label);

        progress = new ProgressBar(label,
                                   label->getWidth() - progress_width - style::window::default_right_margin,
                                   (label->getHeight() / 2) - (style::window::label::small_h / 2),
                                   progress_width,
                                   style::window::label::small_h);
        progress->setMaximum(maxValue);
        progress->setPercentageValue(value);

        auto leftArrow  = new Image(label,
                                   label->getWidth() / 2,
                                   (label->getHeight() / 2),
                                   style::window::label::small_h,
                                   style::window::label::small_h,
                                   "left_label_arrow");
        auto rightArrow = new Image(label,
                                    label->getWidth() / 2 + leftArrow->getWidth() + style::margins::small,
                                    (label->getHeight() / 2),
                                    style::window::label::small_h,
                                    style::window::label::small_h,
                                    "right_label_arrow");

        style::window::decorate(leftArrow);
        style::window::decorate(rightArrow);

        inputCallback = [this](Item &item, const InputEvent &event) { return setValueOnInput(event); };

        addWidget(label);
        setVisible(true);
    }

    auto UpDown::setValueOnInput(const InputEvent &event) -> bool
    {
        if (event.keyCode == KeyCode::KEY_RIGHT) {
            value += (value < maxValue) ? 1 : 0;
            progress->setPercentageValue(value);
            return true;
        }
        if (event.keyCode == KeyCode::KEY_LEFT) {
            value -= (value > 0) ? 1 : 0;
            progress->setPercentageValue(value);
            return true;
        }
        return false;
    }
} // namespace gui
