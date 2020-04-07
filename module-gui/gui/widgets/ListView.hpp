/*
 * ListView.h
 *
 *  Created on: 11 mar 2019
 *      Author: robert
 */

#ifndef MIDDLEWARES_GUI_WIDGETS_LISTVIEW_HPP_
#define MIDDLEWARES_GUI_WIDGETS_LISTVIEW_HPP_

#include <list>
#include "Rect.hpp"
#include "BoxLayout.hpp"
#include "Span.hpp"
#include "ListItemProvider.hpp"
#include "Style.hpp"

namespace gui
{

    class ListItemProvider;

    /// fixed size and application Vertical ListView to be used with ListItemProvider and DatabaseModel only.
    /// For simple vertical list with no external model use VBox
    class ListView : public Rect
    {
      protected:
        // list first item index
        int startIndex;
        // list current scroll direction
        int direction;
        // defines total number of elements in the list
        int elementsCount;
        // defines whether scroll bar should be drawn on the right side of list widget
        bool drawVerticalScroll;
        // maximum number of elements that can be displayed on the screen
        int maxElements;
        // vector that holds currently visible list of items
        std::list<ListItem *> items;
        // pointer to the item provider object
        ListItemProvider *provider = nullptr;
        // Vbox that holds currently visible list of items
        VBox *listItems;
        // rounded rectangle used to draw scroll bar
        Rect *scroll = nullptr;
        // list span item
        Span *listSpanItem = nullptr;

        int listPageSize;
        bool listEndPageReached;
        bool listContinuous;

        virtual void refreshPage();
        void clearItems();
        void updateScrollDimenstions();
        void disableScroll();
        bool listPageEndReached();

      public:
        static const int DIRECTION_BOTTOM = 0x01;
        static const int DIRECTION_TOP    = 0x02;

        ListView();
        ListView(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h);
        virtual ~ListView();

        // virtual methods for list view
        virtual void setElementsCount(int count);
        virtual void drawScroll(bool value);
        virtual void setMaxElements(int value);
        virtual void setProvider(ListItemProvider *provider);
        virtual void setPageSize(int size);
        virtual ListItem *getSelectedItem();
        /*
         * Acquire and/or update items using item provider
         */
        virtual void refreshList();
        /**
         * removes all data stored in the list and resets size and position to 0
         */
        virtual void clear();

        // virtual methods from Item
        std::list<DrawCommand *> buildDrawList() override;
        bool onInput(const InputEvent &inputEvent) override;
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
    };

} /* namespace gui */

#endif /* MIDDLEWARES_GUI_WIDGETS_LISTVIEW_HPP_ */
