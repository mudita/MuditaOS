#pragma once

#include <list>
#include "Rect.hpp"
#include "BoxLayout.hpp"
#include "Span.hpp"
#include "ListItemProvider.hpp"
#include "Style.hpp"

namespace gui
{

    enum class ListViewType
    {
        TopDown,
        Continuous
    };

    class ListItemProvider;

    class ListViewScroll : public Rect
    {

      public:
        ListViewScroll(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h);
        ~ListViewScroll(){};

        void update(int startIndex, int listPageSize, int elementsCount);
    };

    class ListView : public Rect
    {
      protected:
        int startIndex;
        int elementsCount;
        ListItemProvider *provider = nullptr;
        /// Vbox that holds currently visible list of items
        VBox *body;
        ListViewScroll *scroll = nullptr;
        /// list span item
        Span *listSpanItem = nullptr;

        int currentPageSize = 1;

        enum class Direction
        {
            Top,
            Bot
        };

        ListViewType listType = ListViewType::TopDown;
        Direction direction   = Direction::Bot;

        void refresh();
        void clearItems();
        void addItemsOnPage();
        void setFocus();
        bool listPageEndReached();

      public:
        ListView();
        ListView(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h);
        ~ListView();

        void setElementsCount(int count);
        void setProvider(ListItemProvider *provider);
        void setListViewType(ListViewType type);
        void clear();

        // virtual methods from Item
        std::list<DrawCommand *> buildDrawList() override;
        bool onInput(const InputEvent &inputEvent) override;
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
    };

} /* namespace gui */
