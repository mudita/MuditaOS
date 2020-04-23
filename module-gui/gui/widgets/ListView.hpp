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
        /// list first item index
        int startIndex;
        /// defines total number of elements in the list
        int elementsCount;
        /// pointer to the item provider object
        ListItemProvider *provider = nullptr;
        /// Vbox that holds currently visible list of items
        VBox *body;
        /// rounded rectangle used to draw scroll bar
        ListViewScroll *scroll = nullptr;
        /// list span item
        Span *listSpanItem = nullptr;

        int listPageSize;

        enum class Direction
        {
            Top,
            Bot
        };

        ListViewType listType = ListViewType::TopDown;
        Direction direction   = Direction::Bot;

        void refresh();
        void clearItems();

        //        TODO: add direction as parameter
        bool listPageEndReached();

      public:
        ListView();
        ListView(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h);
        ~ListView();

        void setElementsCount(int count);
        void setProvider(ListItemProvider *provider);
        void setListViewType(ListViewType type);
        virtual ListItem *getSelectedItem();

        void clear();

        // virtual methods from Item
        std::list<DrawCommand *> buildDrawList() override;
        bool onInput(const InputEvent &inputEvent) override;
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
    };

} /* namespace gui */
