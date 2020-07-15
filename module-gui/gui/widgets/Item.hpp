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
#include "Alignment.hpp"

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
        SPAN_ITEM,
        TEXT,
        IMAGE,
        LAYOUT,
        VBOX,
        HBOX
    };

    NavigationDirection inputToNavigation(const InputEvent &evt);

    using Padding = Margins;

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
            Min,
            Normal,
            Draw,
            Max,
        };
        /// actual bounding box of the item. This is in coordinates of the parent widget.
        BoundingBox widgetArea;
        /// bounding box of the item minimum size,
        BoundingBox widgetMinimumArea;
        /// bounding box of the item maximal size,
        BoundingBox widgetMaximumArea;
        // bounding box used for drawing. This is in coordinates of window
        BoundingBox drawArea; // drawableArea would be more accurate
                              // maximal bounding box size
        auto area(Area which = Area::Normal) -> BoundingBox &
        {
            switch (which) {
            case Area::Min:
                return widgetMinimumArea;
            case Area::Normal:
                return widgetArea;
            case Area::Draw:
                return drawArea;
            case Area::Max:
                return widgetMaximumArea;
            default:
                return widgetArea;
            }
        }

        Padding padding;
        Margins margins;

        Alignment alignment;

        /// radius of corner, default 0
        short radius = 0;
        /// flag that defines if item is active
        /// if false -> than it shouldn't be used with onInput, navigation etc.
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
        /// defined lambda function all Items have functions corresponding to callback
        /// 1. if you wish to create new item which does something new in action function - override it
        /// 2. if you wish to call function on item - set callback for it
        /// @attention all callbacks return true if handled, return true means end of event processing, i.e. if you
        /// handle enter in inputCallback, then activatedCallback shouldn't be called.
        /// @{

        /// called when item looses/gains focus
        std::function<bool(Item &)> focusChangedCallback;
        /// called when item has dimensions changed
        /// @note should be part of widgetArea
        std::function<bool(Item &, void *data)> dimensionChangedCallback;
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
        /// runs focusChangedCallback on item which changes
        /// @attention focusItem is just a pointer, might crash if item with focus was removed
        void setFocusItem(Item *item);
        /// gettter for focus item
        /// @attention focusItem is just a pointer, might crash if item with focus was removed
        Item *getFocusItem();
        /// @}

        /// @defgroup callbackCallers   functions which should call functors from callbacks group
        /// @{

        /// called from setFocus, does nothing (which means it doesn't call focusChangedCallback
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
        /// @attention It doesn't call `delete` please remove item after calling this function
        virtual bool removeWidget(Item *item);
        /// call removeWidget on item and delete on item
        virtual bool erase(Item *item);
        /// remove all children and destroy them
        virtual void erase();
        /// sets `visible` flag
        virtual void setVisible(bool value);
        virtual void setArea(BoundingBox area);
        void setAreaInAxis(Axis axis,
                           uint32_t posOnAxis,
                           uint32_t posOnOrthogonalAxis,
                           uint32_t sizeOnAxis,
                           uint32_t sizeOnOrthogonalAxis);
        /// sets position of element - this is sets area().x and area().y of item
        /// calls onDimensionChanged callback & updateDrawArea for item
        /// @attention should be bind to area
        virtual void setPosition(const short &x, const short &y);
        virtual void setPosition(const short &val, Axis axis);
        [[nodiscard]] uint16_t getSize(Axis axis) const;
        [[nodiscard]] uint16_t getPosition(Axis axis) const;

        void setMargins(const Margins &value);
        [[nodiscard]] Margins getMargins();

        virtual void setAlignment(const Alignment &value);
        [[nodiscard]] Alignment &getAlignment();
        [[nodiscard]] Alignment getAlignment(Axis axis);
        [[nodiscard]] virtual uint16_t getAxisAlignmentValue(Axis axis);

        /// @defgroup size_range_setters Named the same way that are in QT minimum/maximum sizes setters
        ///
        /// All setters:
        /// 1. only sets range in which normal area can be calculated in layouts
        /// 2. doesn't trigger any callbacks
        /// @note we can consider calling callback when setMinimum/Maximum exceeds normal size
        /// @{
        void setMaximumSize(uint32_t val, Axis axis);
        void setMaximumWidth(uint32_t w);
        void setMaximumHeight(uint32_t h);
        void setMaximumSize(uint32_t w, uint32_t h);

        void setMinimumSize(uint32_t val, Axis axis);
        void setMinimumSize(uint32_t w, uint32_t h);
        void setMinimumWidth(uint32_t w);
        void setMinimumHeight(uint32_t h);
        /// @}

        /// requests bigger size from parent if parent available
        /// if no parent available - sets size
        /// @return true if handled positively
        virtual auto requestSize(unsigned short request_w, unsigned short request_h) -> Size final;
        /// handle for layouts to implement to resize on demand ( i.e. when it needs to expand after addition/removal of
        /// chars )
        ///
        /// by default items do not resize of it's childrem so it's safe for them to pass handleRequestSize
        /// straight to setSize. Layout manages size of it's item in range { Area::Min <= size <= Area::Max } so for
        /// that case layout should to i.e. store request size and than handle resizes appropriately
        /// i.e. Text => text->requestSize => layout->storeRequest => layout use it in resizes
        /// with this both:
        /// 1. user setSize
        /// 2. resize requests from UI element
        /// should be handled without infinite loop on resize ( item->setSize -> notify Layout -> layout: item->setSize
        /// )
        ///
        /// @return bool requested size granted {w,h}
        virtual auto handleRequestResize(const Item *, unsigned short request_w, unsigned short request_h) -> Size;

        /// sets size of item
        virtual void setSize(const unsigned short w, const unsigned short h);
        void setSize(uint32_t val, Axis axis);
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

        void setX(const int32_t x);
        void setY(const int32_t y);
        [[nodiscard]] int32_t getX() const
        {
            return (widgetArea.x);
        }
        [[nodiscard]] int32_t getY() const
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
        /// helper function to show where widget ends in x axis
        [[nodiscard]] int32_t offset_w() const
        {
            return getWidth() + widgetArea.x;
        }
        /// helper function to show where widget ends in y axis
        [[nodiscard]] int32_t offset_h() const
        {
            return getHeight() + widgetArea.y;
        }
        [[nodiscard]] int32_t getOffset(Axis axis) const
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
