#pragma once

#include "BoxLayout.hpp"

namespace gui
{

    class GridLayout : public BoxLayout
    {
      protected:
        /// size of singular grid element
        struct GridSize
        {
            uint32_t x = 0;
            uint32_t y = 0;
        } grid;

      public:
        GridLayout(
            Item *parent, const uint32_t &x, const uint32_t &y, const uint32_t &w, const uint32_t &h, GridSize grid);
        GridLayout(const uint32_t &x, const uint32_t &y, const uint32_t &w, const uint32_t &h, GridSize grid)
            : GridLayout(nullptr, x, y, w, h, grid)
        {}
        GridLayout() : GridLayout(0, 0, 0, 0, {0, 0})
        {}
        ~GridLayout() override = default;
        /// when reached top -> start from bottom. When reached left, start from right.
        bool navigationRotate = true;
        void resizeItems() override;
        void setNavigation() override;
    };

}; // namespace gui
