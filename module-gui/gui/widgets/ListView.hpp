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

    class ListView : public Rect
    {
      protected:
        /// list first item index
        int startIndex;
        /// defines total number of elements in the list
        int elementsCount;
        /// defines whether scroll bar should be drawn on the right side of list widget
        bool drawVerticalScroll;
        /// maximum number of elements that can be displayed on the screen
        //        TODO: Remove
        int maxElements;
        /// pointer to the item provider object
        ListItemProvider *provider = nullptr;
        /// Vbox that holds currently visible list of items
        VBox *body;
        /// rounded rectangle used to draw scroll bar
        Rect *scroll = nullptr;
        /// list span item
        Span *listSpanItem = nullptr;

        int listPageSize;

        enum class Type
        {
            TopDown,
            Continuous
        };

        enum class Direction
        {
            Top,
            Bot
        };

        Type listType       = Type::Continuous;
        Direction direction = Direction::Bot;

        void refresh();
        void clearItems();
        void updateScrollDimenstions();
        void disableScroll();

        //        TODO: add direction as parameter
        bool listPageEndReached();

      public:

        ListView();
        ListView(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h);
        ~ListView();

        void setElementsCount(int count);
        void drawScroll(bool value);
        void setMaxElements(int value);
        void setProvider(ListItemProvider *provider);
        void setPageSize(int size);
        virtual ListItem *getSelectedItem();

        void clear();

        // virtual methods from Item
        std::list<DrawCommand *> buildDrawList() override;
        bool onInput(const InputEvent &inputEvent) override;
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
    };

} /* namespace gui */

