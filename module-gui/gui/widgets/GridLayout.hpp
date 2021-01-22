// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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
        void resizeItems() override;
        void setNavigation() override;

        uint32_t rowSize = 0;
        uint32_t colSize = 0;

      private:
        void handleItemsOutOfGridLayoutArea(uint32_t maxItemsInArea);
        Item *getFirstActiveItem(uint32_t startposition, int step);
        inline uint32_t getLastColumnIndex(uint32_t col)
        {
            auto lastcolumnindex = col;
            while ((lastcolumnindex + colSize) < children.size())
                lastcolumnindex += colSize;
            return lastcolumnindex;
        }
        inline uint32_t getLastRowIndex(uint32_t row)
        {
            uint32_t lastrowindex = colSize * row + (colSize - 1);
            while (lastrowindex >= children.size())
                lastrowindex--;
            return lastrowindex;
        }
    };

}; // namespace gui
