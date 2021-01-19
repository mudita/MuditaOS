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
        /// when reached top -> start from bottom. When reached left, start from right.
        bool navigationRotate = true;
        void resizeItems() override;
        void setNavigation() override;
        Item *nextNavigationItem(std::list<Item *>::iterator it);

        uint32_t rowSize = 0;
        uint32_t colSize = 0;
        ///> elementsInIncompletedLastRow describes how many items has been put to last row,
        /// in case when items for last row is not equal to colSize
        uint32_t elementsInIncompletedLastRow = 0;

      private:
        uint32_t calculateColumnSizeForBorderTransition(const uint32_t currentPosition);
        uint32_t calculateRowSizeForBorderTransition(const uint32_t currentPosition);

        void handleItemsOutOfGridLayoutArea(uint32_t maxItemsInArea);
    };

}; // namespace gui
