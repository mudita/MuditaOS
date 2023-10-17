// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BoxLayout.hpp"

namespace gui
{
    /// GridLayout lays out items (widgets) in a grid.
    class GridLayout : public BoxLayout
    {
      protected:
        /// Data container for size of singular grid element
        struct GridSize
        {
            uint32_t x = 0;
            uint32_t y = 0;
        } grid;

      public:
        GridLayout(Item *parent,
                   const uint32_t &x,
                   const uint32_t &y,
                   const uint32_t &width,
                   const uint32_t &height,
                   GridSize cellSize);
        GridLayout(
            const uint32_t &x, const uint32_t &y, const uint32_t &width, const uint32_t &height, GridSize cellSize)
            : GridLayout(nullptr, x, y, width, height, cellSize)
        {}
        GridLayout() : GridLayout(0, 0, 0, 0, {0, 0})
        {}
        /// Calculates Items position.
        ///@note It is called when new widgets are added to layout or on resize requests.
        ///@attention Function does not yet resize items -> only items position are calculated"
        void resizeItems() override;
        /// Sets navigation for layout children (for widgets/items inside layout).
        ///@note It is called when new widgets are added to layout or on resize requests.
        void setNavigation() override;
        /// calculated rows count in grid layout.
        uint32_t rowSize = 0;
        /// calculated columns count in grid layout.
        uint32_t colSize = 0;

      private:
        /// function to handle items that do not fit into grid layout area
        /// @param maxItemsInArea : maximum amount of items that fits into grid size
        void handleItemsOutOfGridLayoutArea(uint32_t maxItemsInArea);
        /// retrieves active item from children items with defined step to next item
        /// in case when item in startPosition is not active
        /// @param startPosition : starting point for calculation
        /// @param step : single step used to search for next item
        /// @return pointer to active Item or nullptr in case when active Item has not been found
        Item *getFirstActiveItem(uint32_t startPosition, int step);
        /// retrieves index of last item in the selected column.
        ///@param col : column index for which calculation shall be made
        ///@return calculated last item index for selected column index
        inline uint32_t getLastColumnIndex(uint32_t col)
        {
            auto lastColumnIndex = col;
            while ((lastColumnIndex + colSize) < children.size())
                lastColumnIndex += colSize;
            return lastColumnIndex;
        }
        /// retrieves index of last item in the selected row.
        ///@param row : row index for which calculation shall be made
        ///@return calculated last item index for selected row index
        inline uint32_t getLastRowIndex(uint32_t row)
        {
            uint32_t lastRowIndex = colSize * row + (colSize - 1);
            while (lastRowIndex >= children.size())
                lastRowIndex--;
            return lastRowIndex;
        }
    };

}; // namespace gui
