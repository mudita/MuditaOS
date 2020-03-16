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
#include "ListItemProvider.hpp"

namespace gui
{

    class ListItemProvider;

    /// fixed size and application Vertical ListView to be used with ListItemProvider and DatabaseModel only.
    /// For simple vertical list with no external model use VBox
    class ListView : public Rect
    {
      protected:
        // id of the first element visible on the list (location depends on orientation)
        int firstIndex;
        // index of last visible item
        int lastIndex;
        // defines total number of elements in the list
        int elementsCount;
        // defines whether separator should be placed between elements
        bool drawSeparators;
        // defines whether scroll bar should be drawn on the right side of list widget
        bool drawVerticalScroll;
        // defines whether first element is on bottom of widget ( BOTTOM_UP ) or on top of widget ( TOP_BOTTOM )
        int orientation;
        // maximum number of elements that can be displayed on the screen
        int maxElements;
        // index of the currently selected element
        int selectedIndex;
        // vector that holds currently visible list of items
        std::list<ListItem *> items;
        // pointer to the item provider object
        ListItemProvider *provider = nullptr;
        // rounded rectangle used to draw scroll bar
        Rect *scroll;
        // space available for new items - this will change after any new item retrieved from provider
        int listMode;
        int pageSize;

        virtual void updatePageItems();
        void updateContinousItems();
        void clearItems();
        void updateScrollDimenstions();
        void disableScroll();

      public:
        static const int ORIENTATION_TOP_DOWN  = 0x01;
        static const int ORIENTATION_BOTTOM_UP = 0x02;
        // list items has same height, incrementing beyond last or first item will cause all items to be changed
        static const int MODE_PAGE = 0x04;
        // following item will be displayed as long there is available space. Height of the items must be provided by
        // the items.
        static const int MODE_CONTINUOUS = 0x08;

        ListView();
        ListView(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h);
        virtual ~ListView();

        // virtual methods for list view
        virtual void setElementsCount(int count);
        virtual void setDrawSeparators(bool value);
        virtual void setOrientation(int orientation);
        virtual void drawScroll(bool value);
        virtual void setMaxElements(int value);
        virtual void setProvider(ListItemProvider *provider);
        virtual void setMode(int mode);
        virtual void setPageSize(int size);
        virtual ListItem *getSelectedItem();
        /*
         * Acquire and/or update items using item provider
         */
        virtual void updateItems();
        /**
         * removes all data stored in the list and resets size and position to 0
         */
        virtual void clear();

        // virtual methods from Item
        std::list<DrawCommand *> buildDrawList() override;
        bool onInput(const InputEvent &inputEvent) override;
        bool onActivated(void *data) override;
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
    };

} /* namespace gui */

#endif /* MIDDLEWARES_GUI_WIDGETS_LISTVIEW_HPP_ */
