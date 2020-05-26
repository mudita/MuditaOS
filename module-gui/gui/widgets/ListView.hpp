#pragma once

#include <list>
#include "Rect.hpp"
#include "BoxLayout.hpp"
#include "Span.hpp"
#include "ListItemProvider.hpp"
#include "Style.hpp"

namespace gui
{
    class ListItemProvider;

    class ListViewScroll : public Rect
    {
      public:
        ListViewScroll(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h);

        bool shouldShowScroll(int listPageSize, int elementsCount);
        void update(int startIndex, int listPageSize, int elementsCount);
    };

    class ListView : public Rect
    {
      protected:
        int startIndex             = 0;
        int elementsCount          = 1;
        ListItemProvider *provider = nullptr;
        VBox *body                 = nullptr;
        ListViewScroll *scroll     = nullptr;
        int itemSpanSize           = style::listview::item_span_small;
        Span *listSpanItem         = nullptr;

        int currentPageSize = 0;

        style::listview::Type listType       = style::listview::Type::TopDown;
        style::listview::Direction direction = style::listview::Direction::Bottom;

        void clearItems();
        virtual void addItemsOnPage();
        void setFocus();
        void refresh();
        void resizeWithScroll();
        Order getOrderFromDirection();
        bool listPageEndReached();

      public:
        ListView();
        ListView(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h);
        ~ListView();

        void setElementsCount(int count);
        void setProvider(ListItemProvider *provider);
        void setListViewType(style::listview::Type type);
        void setItemSpanSize(int size);
        void onProviderDataUpdate();
        void clear();

        // virtual methods from Item
        std::list<DrawCommand *> buildDrawList() override;
        bool onInput(const InputEvent &inputEvent) override;
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
    };

} /* namespace gui */
