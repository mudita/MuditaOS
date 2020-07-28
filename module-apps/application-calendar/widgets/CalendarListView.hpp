#pragma once

#include "AllEventsItem.hpp"
#include "Application.hpp"
#include <ListView.hpp>
#include <functional>

namespace gui
{
    class ListItemProvider;

    class CalendarListView : public ListView
    {

        UTF8 labelMark           = "";
        UTF8 previousLabelMark   = "";
        bool previousItemIsLabel = false;

        void addItemsOnPage() override;
        void addLabelMarker(AllEventsItem *item);

      public:
        CalendarListView(
            Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h, std::shared_ptr<ListItemProvider> prov);
    };

} /* namespace gui */
