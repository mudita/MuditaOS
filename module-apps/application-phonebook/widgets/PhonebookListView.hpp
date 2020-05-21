#pragma once

#include "Application.hpp"
#include "ListView.hpp"
#include "../widgets/PhonebookItem.hpp"
#include <functional>

namespace gui
{
    class ListItemProvider;

    class PhonebookListView : public ListView
    {

        UTF8 labelMark = "";

        void addItemsOnPage() override;
        void addLabelMarker(PhonebookItem *item);

      public:
        PhonebookListView(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h);
    };

} /* namespace gui */
