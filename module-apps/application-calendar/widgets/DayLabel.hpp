#pragma once
#include "Application.hpp"
#include <BoxLayout.hpp>

namespace gui
{

    class DayLabel : public Rect
    {
        gui::VBox *vBox       = nullptr;
        gui::Label *dayNumber = nullptr;
        gui::Image *dotImage  = nullptr;

      public:
        DayLabel(app::Application *app,
                 gui::Item *parent,
                 const uint32_t &cellIndex,
                 const uint32_t &firstWeekOffset,
                 const uint32_t &width,
                 const uint32_t &height,
                 bool isDayEmpty);
        ~DayLabel() override = default;

        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
        uint32_t getDayNumber();
    };

} /* namespace gui */
