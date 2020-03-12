#pragma once

#include "BoxLayout.hpp"

namespace gui
{

    class GridLayout : public BoxLayout
    {
      protected:
        struct GridSize
        {
            uint32_t x = 0;
            uint32_t y = 0;
        } grid;

      public:
        // one defined
        GridLayout(
            Item *parent, const uint32_t &x, const uint32_t &y, const uint32_t &w, const uint32_t &h, GridSize grid);
        // derived
        GridLayout(const uint32_t &x, const uint32_t &y, const uint32_t &w, const uint32_t &h, GridSize grid)
            : GridLayout(nullptr, x, y, w, h, grid)
        {}
        GridLayout() : GridLayout(0, 0, 0, 0, {0, 0})
        {}
        ~GridLayout() override = default;
        /// when reached top -> start from bottom. When reached left, start from right.
        bool navigationRotate = true;
        void resizeItems() override;

        // virtual methods from Item - use from parent
        //    void setPosition(const short &x, const short &y) override;
        //    void setSize(const short &w, const short &h) override;
        //    bool addWidget(gui::Item *item) override;
        //    bool removeWidget(Item *item) override;
        //    std::list<DrawCommand *> buildDrawList() override;
        void setNavigation() override;
    };

}; // namespace gui
