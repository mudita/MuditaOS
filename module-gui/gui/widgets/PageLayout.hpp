#pragma once

#include <BoxLayout.hpp>

namespace gui
{

    /// Layout providing Pages for elements
    class PageLayout : public BoxLayout
    {
      public:
        PageLayout() = default;
        virtual ~PageLayout() = default;
        PageLayout(Item *parent, const uint32_t &x, const uint32_t &y, const uint32_t &w, const uint32_t &h);
        PageLayout(Item *parent, const BoundingBox &box);

        VBox *addPage();
        virtual void addWidget(Item *item) override;
        /// switch page to page ∈ [0 ... (num of pages -1) )
        /// set it's navigation and select first element as selected (in setVisible)
        bool switchPage(unsigned int n, bool previous = false);
    };

    ;
} // namespace gui
