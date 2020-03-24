#pragma once

#include <vector>
#include <list>
#include <functional>

#include "KeyEvent.hpp"
#include "Common.hpp"
#include "core/DrawCommand.hpp"
#include "core/BoundingBox.hpp"
#include "input/InputEvent.hpp"
#include "Layout.hpp"
#include "Navigation.hpp"
#include "Margins.hpp"

namespace gui
{

    class Navigation;

    enum class ItemType
    {
        ITEM = 0,
        RECT = 1,
        LABEL,
        LIST,
        LIST_ITEM,
        TEXT,
        IMAGE,
        LAYOUT,
        VBOX,
        HBOX
    };

    class Item
    {
      public:
        // flag that informs whether item has a focus
        bool focus;
        /// pointer to the child item that has focus
        Item *focusItem = nullptr;
        /// item type of the widget - could be used to check what type item we are working on
        /// right now unused except Text, where it calles callback for gui::BoxLayout which does nothing
        ItemType type = ItemType::ITEM;
        // pointer to the parent Item
        Item *parent = nullptr;
        // list of items that have the same parent.
        std::list<Item *> children;
        enum class Area
        {
            Normal,
            Draw,
            Actual, // actual area in use between: Normal <-> Max
            Max,
        };
        /// bounding box of the item. This is in coordinates of the parent widget.
        BoundingBox widgetArea;
        /// bounding box of the item maximal size, if not specified other - the same as widget Area
        BoundingBox widgetMaxArea;
        /// bounding box of the item maximal size, if not specified other - the same as widget Area
        BoundingBox widgetActualArea;
        // bounding box used for drawing. This is in coordinates of window
        BoundingBox drawArea; // drawableArea would be more accurate
                              // maximal bounding box size
        auto area(Area which = Area::Normal) -> BoundingBox &
        {
            switch (which) {
            case Area::Normal:
                return widgetArea;
            case Area::Draw:
                return drawArea;
            case Area::Max:
                return widgetMaxArea;
            case Area::Actual:
                return widgetActualArea;
            }
            return widgetArea;
        }
        Margins innerMargins;
        /// radius of corner, default 0
        short radius = 0;
        /// flag that defines if item is active
        /// if false -> than it shouldn't be used witn onInput, navitagion etc.
        bool activeItem = true;
        /// flag that defines whether widget is visible (this is - should be rendered)
        bool visible;
        // policy for changing vertical size if Item is placed inside layout
        LayoutVerticalPolicy verticalPolicy;
        // policy for changing horizontal size if Item is placed inside layout
        LayoutHorizontalPolicy horizontalPolicy;
        // Maximum height to which Layout base widget can scale current widget
        uint16_t maxHeight;
        // Maximum width to which Layout base widget can scale current widget
        uint16_t maxWidth;

        /// @defgroup callbacks     Item callback functions
        /// callback functors are meant to emulate signal <-> slot actions where you bind element instance to in code
        /// defined lambda function all Items have functions coresponding to callback
        /// 1. if you wish to create new item which does something new in action function - override it
        /// 2. if you wish to call function on item - set callback for it
        /// @attention all callbacks return true if handled, return true means end of event processing, i.e. if you
        /// handle enter in inputCallback, then activatedCallback shouldn't be called.
        /// @{

        /// called when item looses/gains focus
        std::function<bool(Item &)> focusChangedCallback;
        /// called when item has dimensions changed
        /// @note should be part of widgetArea
        std::function<void(Item &, void *data)> dimensionChangedCallback;
        /// called when item is activated, this is enter is pressed
        /// @param `this` item
        std::function<bool(Item &)> activatedCallback;
        /// callback when any key is pressed
        /// @param `this` item
        /// @param `InputEvent`
        std::function<bool(Item &, const InputEvent &inputEvent)> inputCallback;
        /// callback when element insides are changed
        /// @param `this` item
        std::function<bool(Item &)> contentCallback;
        /// callback on navigation, called when item passes navigation to handle by it's children
        /// @attention when child handles navigation it should return true, so that parent won't perform action for that
        std::function<bool(const InputEvent &)> itemNavigation = nullptr;

        /// @}

        ///  @defgroup focus    functions handling focus
        /// @{
        bool handleNavigation(const InputEvent inputEvent);
        /// sets/resets focus on `this` Item and runs focusChangedCallback for it
        bool setFocus(bool state);
        /// sets/resets child with focus in `this` Item
        /// runs focusChangledCallback on item which changes
        /// @attention focusItem is just a pointer, might crash if item with focus was removed
        void setFocusItem(Item *item);
        /// gettter for focus item
        /// @attention focusItem is just a pointer, might crash if item with focus was removed
        Item *getFocusItem();
        /// @}

        /// @defgroup callbackCallers   functions which should call functors from callbacks group
        /// @{

        /// called from setFocus, does nothing (which means it doesn't call focusChagedCallback
        virtual bool onFocus(bool state);
        /// called when `this` Item was pressed with enter (middle key on phone action keys), used for callback input
        /// handling calls activatedCallback
        /// @param[in]: data unused
        virtual bool onActivated(void *data);
        /// called when any key is pressed, before onActivated , after focus
        /// calls: inputCallback
        virtual bool onInput(const InputEvent &inputEvent);
        /// (should be) called each time when dimension of element was changed, added and used only with ListView
        /// calls: none, inconsistent api
        /// @note TODO should be fixed so that api would be consistent
        virtual bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim);
        /// (should be) called each time content in item was changed, added for gui::Text widget
        /// calls: none, inconsistent behaviour
        /// @note TODO should be fixed so that api would be consistent
        virtual bool onContent();

        /// @}

        /// function called to add child to item
        /// similar approach to QT, adding widget will always succeed.
        virtual void addWidget(Item *item);
        /// function to remove child item from element
        /// it's recursive for all elements underneath
        /// @attention It doe not call `delete` please remove item after calling this function
        virtual bool removeWidget(Item *item);
        /// sets `visible` flag
        virtual void setVisible(bool value);
        /// sets position of element - this is sets area().x and area().y of item
        /// calls onDimensionChanged callback & updateDrawArea for item
        /// @attention should be bind to area
        virtual void setPosition(const short &x, const short &y);
        /// sets size of element - this is sets area().w and area().h of item
        /// calls onDimensionChanged & updateDrawArea for item
        /// @attention should be bind to area
        virtual void setSize(const unsigned short w, const unsigned short h);
        /// used in ListView to position element sets area() = WidgetArea(params)
        /// calls onDimensionChanged & updateDrwArea
        /// @attention should be bind to area
        virtual void setBoundingBox(const BoundingBox &new_box);
        /// list of commands for renderer to draw elements on screen
        /// all elements consist od lines, arcs, rectangles, text and images
        /// for full list of elements please see renderer code
        /// @note we should consider lazy evaluation prior to drawing on screen, rather than on each resize of elements
        virtual std::list<DrawCommand *> buildDrawList();
        /// sets radius of Item box
        /// @note this should be moved to Rect
        virtual void setRadius(int value);

        /// get next navigation item in NavigationDirection
        virtual Item *getNavigationItem(NavigationDirection direction);
        /// set next navigation item in NavigationDirection
        virtual void setNavigationItem(NavigationDirection direction, Item *item);

        Item();
        virtual ~Item();

        /// @defgroup inconsistent  inconsistent size/offset accessors and setters
        /// all this elements should be checked for naming/use consistency
        /// possibly all of that should be handled via area() (and area should have callback pinned from Item on resize
        /// @{

        void setX(const uint32_t x);
        void setY(const uint32_t y);
        [[nodiscard]] uint32_t getX() const
        {
            return (widgetArea.x);
        }
        [[nodiscard]] uint32_t getY() const
        {
            return (widgetArea.y);
        }
        [[nodiscard]] uint32_t getWidth() const
        {
            return (widgetArea.w);
        }
        [[nodiscard]] uint32_t getHeight() const
        {
            return (widgetArea.h);
        }
        int16_t w()
        {
            return widgetArea.w;
        }
        int16_t h()
        {
            return widgetArea.h;
        }
        /// helper function to show where widget ends in x axis
        int32_t offset_w()
        {
            return w() + widgetArea.x;
        }
        /// helper function to show where widget ends in y axis
        int32_t offset_h()
        {
            return h() + widgetArea.y;
        }
        int32_t offset(Axis axis)
        {
            return this->widgetArea.size(axis) + this->widgetArea.pos(axis);
        };
        /// @}
      protected:
        /// On change of position or size this method will recalculate visible part of the widget
        /// considering widgets hierarchy and calculate absolute position of drawing primitives.
        virtual void updateDrawArea();
        /// Pointer to navigation object. It is added when object is set for one of the directions
        gui::Navigation *navigationDirections = nullptr;
    };

} /* namespace gui */
