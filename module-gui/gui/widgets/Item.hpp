// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Alignment.hpp"        // for Alignment
#include "Axes.hpp"             // for Axis
#include <module-gui/gui/Common.hpp>
#include "Layout.hpp"           // for LayoutHorizontalPolicy, LayoutVertic...
#include "Margins.hpp"          // for Padding, Margins
#include "core/BoundingBox.hpp" // for BoundingBox, BoundingBox::(anonymous)
#include <cstdint>              // for uint32_t, int32_t, uint16_t
#include <functional>           // for function
#include <list>                 // for list
#include <memory>               // for unique_ptr
#include <utility>              // for move
#include <core/DrawCommandForward.hpp>
#include <module-gui/gui/widgets/visitor/GuiVisitor.hpp>

namespace gui
{
    class InputEvent;
}
namespace gui
{
    class Navigation;
} // namespace gui
namespace gui
{
    class Timer;
}

namespace gui
{
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

    class Item
    {
      private:
        /// list of timers so that item could have it's timers in itself
        std::list<std::unique_ptr<Timer>> timers;

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
        std::function<bool(Item &, BoundingBox data)> dimensionChangedCallback;
        /// called when item is activated, this is enter is pressed
        /// @param `this` item
        std::function<bool(Item &)> activatedCallback;
        /// callback when any key is pressed
        /// @param `this` item
        /// @param `InputEvent`
        std::function<bool(Item &, const InputEvent &inputEvent)> inputCallback;
        /// callback when timer is called on Item and onTimer is executed
        /// @param `this` item
        /// @param `timer` which triggered this callback
        std::function<bool(Item &, Timer &)> timerCallback = nullptr;

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
        Item *getFocusItem() const;
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
        /// called on Timer event in application, triggeres timerCallback
        /// @param timer timer element which triggered this action
        virtual bool onTimer(Timer &timer);

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

        virtual void setMargins(const Margins &value);
        [[nodiscard]] Margins getMargins();

        virtual void setPadding(const Padding &value);
        [[nodiscard]] Padding getPadding() const;

        virtual void setAlignment(const Alignment &value);
        [[nodiscard]] Alignment &getAlignment();
        [[nodiscard]] Alignment getAlignment(Axis axis);
        [[nodiscard]] virtual uint16_t getAxisAlignmentValue(Axis axis, uint16_t itemSize);

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
        /// entry function to create commands to execute in renderer to draw on screen
        /// @note we should consider lazy evaluation prior to drawing on screen, rather than on each resize of elements
        /// @return list of commands for renderer to draw elements on screen
        virtual std::list<Command> buildDrawList() final;
        /// Implementation of DrawList per Item to be drawn on screen
        /// This is called from buildDrawList before children elements are added
        /// should be = 0;
        virtual void buildDrawListImplementation(std::list<Command> &commands)
        {}
        std::function<void(std::list<Command> &)> preBuildDrawListHook  = nullptr;
        std::function<void(std::list<Command> &)> postBuildDrawListHook = nullptr;
        /// sets radius of Item box
        /// @note this should be moved to Rect
        virtual void setRadius(int value);

        /// get next navigation item in NavigationDirection
        virtual Item *getNavigationItem(NavigationDirection direction);
        /// set next navigation item in NavigationDirection
        virtual void setNavigationItem(NavigationDirection direction, Item *item);
        /// clear next navigation item in NavigationDirection
        virtual void clearNavigationItem(gui::NavigationDirection direction);

        Item();
        Item(Item &) = delete;
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

        /// adds timer to gui item
        /// this is needed so that timer for element would live as long as element lives
        void attachTimer(std::unique_ptr<Timer> &&timer)
        {
            timers.emplace_back(std::move(timer));
        }

        /// remove timer from item and as a result - destory it
        void detachTimer(Timer &timer);

        virtual void accept(GuiVisitor &visitor);

      protected:
        /// On change of position or size this method will recalculate visible part of the widget
        /// considering widgets hierarchy and calculate absolute position of drawing primitives.
        virtual void updateDrawArea();
        virtual void buildChildrenDrawList(std::list<Command> &commands) final;
        /// Pointer to navigation object. It is added when object is set for one of the directions
        gui::Navigation *navigationDirections = nullptr;
    };

    NavigationDirection inputToNavigation(const InputEvent &evt);
    bool isInputNavigation(const InputEvent &evt);
} /* namespace gui */
