// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Alignment.hpp" // for Alignment
#include "Axes.hpp"      // for Axis
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
#include <Timers/Timer.hpp>

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
    /// enumaration that contains gui item types
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
    /// Base class for all GUI items.
    /// @details It is a basic element for creating widgets and applications
    /// that make up the GUI (Graphic User Interface) in MuditaOS
    class Item
    {
      public:
        /// flag that informs whether item has a focus
        bool focus;
        /// pointer to the child item that has focus
        Item *focusItem = nullptr;
        /// item type of the widget - could be used to check what type item we are working on
        /// right now unused except Text, where it calls callback for gui::BoxLayout which does nothing
        ItemType type = ItemType::ITEM;
        /// pointer to the parent Item
        Item *parent = nullptr;
        /// list of item's children. @note Items can have only one parent.
        std::list<Item *> children;
        /// enumaration with options for item bounding box (size & position) area
        enum class Area
        {
            Min,
            Normal,
            Draw,
            Max,
        };
        /// actual bounding box (size & position) of the item. This is in coordinates of the parent widget.
        BoundingBox widgetArea;
        /// bounding box (size & position) of the item minimum size,
        BoundingBox widgetMinimumArea;
        /// bounding box (size & position) of the item maximal size,
        BoundingBox widgetMaximumArea;
        /// bounding box (size & position) used for drawing. This is in coordinates of window
        BoundingBox drawArea; // drawableArea would be more accurate
                              // maximal bounding box size
        /// gets bounding box for selected area
        BoundingBox &area(Area which = Area::Normal)
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

        /// flag that defines if item is active.
        /// @details When flag is set to true item is focusable (focus on item can be set)
        /// and clickable (item action can be activated). Otherwise item is non focusable and non clickable.
        /// @note if false -> than it shouldn't be used with onInput, navigation etc.
        bool activeItem = true;
        /// flag that defines whether widget is visible (this is - should be rendered)
        bool visible;
        /// policy for changing vertical size if Item is placed inside layout.
        LayoutVerticalPolicy verticalPolicy;
        /// policy for changing horizontal size if Item is placed inside layout.
        LayoutHorizontalPolicy horizontalPolicy;
        /// Maximum height to which Layout base widget can scale current widget.
        Length maxHeight;
        /// Maximum width to which Layout base widget can scale current widget.
        Length maxWidth;

        /// @defgroup callbacks     Item callback functions
        /// callback functors are meant to emulate signal <-> slot actions where you bind element instance to in code
        /// defined lambda function all Items have functions corresponding to callback
        /// 1. if you wish to create new item which does something new in action function - override it
        /// 2. if you wish to call function on item - set callback for it
        /// @attention all callbacks return true if handled, return true means end of event processing, i.e. if you
        /// handle enter in inputCallback, then activatedCallback shouldn't be called.
        /// @{

        /// called when item looses/gains focus
        /// @param `this` : item
        std::function<bool(Item &)> focusChangedCallback;
        /// called when item has dimensions changed
        /// @param `this` : item
        /// @param data : new bounding box area
        /// @note should be part of widgetArea
        std::function<bool(Item &, BoundingBox data)> dimensionChangedCallback;
        /// called when item is activated, this is enter is pressed
        /// @param `this` : item
        std::function<bool(Item &)> activatedCallback;
        /// callback when any key is pressed
        /// @param `this` : item
        /// @param inputEvent : input event e.g. key pressed
        std::function<bool(Item &, const InputEvent &inputEvent)> inputCallback;
        /// callback when timer is called on Item and onTimer is executed
        /// @param `this` : item
        /// @param `timer` : which triggered this callback
        std::function<bool(Item &, sys::Timer &)> timerCallback = nullptr;

        /// callback on navigation, called when item passes navigation to handle by its children
        /// @param `InputEvent` : input event e.g. key pressed
        /// @attention when child handles navigation it should return true, so that parent won't perform action for that
        std::function<bool(const InputEvent &)> itemNavigation = nullptr;

        /// @}

        ///  @defgroup focus    functions handling focus
        /// @{

        /// navigation handler for input events e.g. keyboard key pressed
        /// @param inputEvent : input event e.g. key pressed
        bool handleNavigation(const InputEvent inputEvent);
        /// sets/resets focus on this Item and runs focusChangedCallback for it
        /// @param state : true to set focus on item, false to clear focus from item
        bool setFocus(bool state);
        /// sets/resets focus on provided item child if one exists.
        /// @param item : this
        /// @note runs focusChangedCallback on item which changes
        /// @attention focusItem is just a pointer, might crash if item with focus was removed
        void setFocusItem(Item *item);
        /// getter for focus item
        /// @attention focusItem is just a pointer, might crash if item with focus was removed
        Item *getFocusItem() const;

        /// @}

        /// @defgroup callbackCallers   functions which should call functors from callbacks group
        /// @{

        /// called from setFocus, does nothing (which means it doesn't call focusChangedCallback
        /// @param state : new focus state
        virtual bool onFocus(bool state);
        /// called when this Item was pressed with enter (middle key on phone action keys),
        /// used for callback input handling calls activatedCallback
        /// @param[in] data : unused
        virtual bool onActivated(void *data);
        /// called when any key is pressed, before onActivated , after focus
        /// calls: inputCallback
        /// @param InputEvent : input event e.g. key pressed
        virtual bool onInput(const InputEvent &inputEvent);
        /// (should be) called each time when dimension of element was changed
        /// @param oldDim : old bounding box dimensions (item size & position)
        /// @param newDim : new bounding box dimensions (item size & position)
        /// @note TODO should be fixed so that api would be consistent
        virtual bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim);
        /// called on Timer event in application, triggeres timerCallback
        /// @param : timer timer element which triggered this action
        virtual bool onTimer(sys::Timer &timer);

        /// @}

        /// function called to add child to item
        /// @param item : pointer to item (widget) that will be added to this item children's
        /// similar approach to QT, adding widget will always succeed.
        virtual void addWidget(Item *item);
        ///  function to remove child item from element
        /// @param item : pointer to item (widget) that will be removed from this item children's
        /// it's recursive for all elements underneath
        /// @attention It doesn't call delete please remove item after calling this function
        virtual bool removeWidget(Item *item);
        /// call removeWidget on item and delete on item
        /// @param item : pointer to item (widget) that will be deleted and removed from this item children's
        virtual bool erase(Item *item);
        /// remove all children and destroy them
        virtual void erase();
        /// sets `visible` flag
        virtual void setVisible(bool value);
        /// sets bounding box area (normal) for item
        /// @param area : new bounding box dimensions (area) for item (item size & position)
        virtual void setArea(BoundingBox area);
        /// function for setting bounding box area (normal) on selected axis
        /// @param axis : indicates axis X or Y for operation
        /// @param posOnAxis : new position value for selected axis
        /// @param posOnOrthogonalAxis : new position for axis orthogonal to selected
        /// @param sizeOnAxis : new size for selected axis
        /// @param sizeOnOrthogonalAxis : new size for orthogonal axis to selected
        void setAreaInAxis(Axis axis,
                           Position posOnAxis,
                           Position posOnOrthogonalAxis,
                           Length sizeOnAxis,
                           Length sizeOnOrthogonalAxis);
        /// sets position of element - this is sets area().x and area().y of item
        /// @note calls onDimensionChanged callback & updateDrawArea for item
        /// @attention should be bind to area
        virtual void setPosition(const Position &x, const Position &y);
        virtual void setPosition(const Position &val, Axis axis);
        [[nodiscard]] Length getSize(Axis axis) const;
        [[nodiscard]] Position getPosition(Axis axis) const;
        virtual void setMargins(const Margins &value);
        [[nodiscard]] Margins getMargins();
        virtual void setPadding(const Padding &value);
        [[nodiscard]] Padding getPadding() const;
        virtual void setAlignment(const Alignment &value);
        [[nodiscard]] Alignment &getAlignment();
        [[nodiscard]] Alignment getAlignment(Axis axis);
        /// function to calculate absolute position in selected axis in case of alignment
        /// @param axis : selected axis (X or Y)
        /// @param itemSize : size of item on selected axis for this calculation
        /// @return alignment for selected axis.
        [[nodiscard]] virtual Length getAxisAlignmentValue(Axis axis, Length itemSize);

        /// @defgroup size_range_setters Named the same way that are in QT minimum/maximum sizes setters
        ///
        /// All setters:
        /// 1. only sets range in which normal area can be calculated in layouts
        /// 2. doesn't trigger any callbacks
        /// @note we can consider calling callback when setMinimum/Maximum exceeds normal size
        /// @{
        void setMaximumSize(Length val, Axis axis);
        void setMaximumWidth(Length w);
        void setMaximumHeight(Length h);
        void setMaximumSize(Length w, Length h);
        void setMinimumSize(Length val, Axis axis);
        void setMinimumSize(Length w, Length h);
        void setMinimumWidth(Length w);
        void setMinimumHeight(Length h);
        /// @}

        /// requests bigger size from parent if parent available
        /// if no parent available - sets size
        /// @return true if handled positively
        virtual Size requestSize(Length request_w, Length request_h) final;
        /// handle for layouts to implement to resize on demand ( i.e. when it needs to expand after
        /// addition/removal of chars )
        ///
        /// @details by default items do not resize of their children so it's safe for them to pass handleRequestSize
        /// straight to setSize. Layout manages size of its item in range { Area::Min <= size <= Area::Max } so in
        /// that case layout should to i.e. store request size and than handle resizes appropriately
        /// i.e. Text => text->requestSize => layout->storeRequest => layout use it in resizes
        /// with this both:
        /// 1. user setSize
        /// 2. resize requests from UI element
        /// should be handled without infinite loop on resize ( item->setSize -> notify Layout -> layout: item->setSize
        /// )
        /// @return bool requested size granted {w,h}
        virtual Size handleRequestResize(const Item *, Length request_w, Length request_h);

        /// flag informing that content has changed
        bool contentChanged = false;
        /// inform parent that child content has changed.
        void informContentChanged();
        /// handle child content change request.
        virtual void handleContentChanged();

        virtual void setSize(Length w, Length h);
        void setSize(Length val, Axis axis);
        virtual void setBoundingBox(const BoundingBox &new_box);
        /// entry function to create commands to execute in renderer to draw on screen
        /// @note we should consider lazy evaluation prior to drawing on screen, rather than on each resize of elements
        /// @return list of commands for renderer to draw elements on screen
        virtual std::list<Command> buildDrawList() final;
        /// Implementation of DrawList per Item to be drawn on screen
        /// This is called from buildDrawList before children elements are added
        /// should be = 0;
        /// @param : commands list of commands for renderer to draw elements on screen
        virtual void buildDrawListImplementation(std::list<Command> &commands)
        {}

        /// pre hook function, if set it is executed before building draw command
        /// at Item::buildDrawListImplementation()
        /// @param `commandlist` : commands list of commands for renderer to draw elements on screen
        std::function<void(std::list<Command> &)> preBuildDrawListHook = nullptr;
        /// post hook function, if set it is executed after building draw command
        /// at Item::buildDrawListImplementation()
        /// @param `commandlist` : commands list of commands for renderer to draw elements on screen
        std::function<void(std::list<Command> &)> postBuildDrawListHook = nullptr;
        /// sets radius for item edges
        /// @note this should be moved to Rect
        virtual void setRadius(int value);

        /// gets next navigation item in NavigationDirection
        /// @param direction : navigation direction (LEFT,RIGHT,UP,DOWN)
        /// @return pointer to next navigation item in selected direction
        virtual Item *getNavigationItem(NavigationDirection direction);
        /// sets navigation item in NavigationDirection
        /// @param direction : navigation direction (LEFT,RIGHT,UP,DOWN)
        /// @param item : pointer to next item for selected direction
        virtual void setNavigationItem(NavigationDirection direction, Item *item);
        /// clears next navigation item in NavigationDirection
        /// @param direction : navigation direction (LEFT,RIGHT,UP,DOWN)
        virtual void clearNavigationItem(gui::NavigationDirection direction);

        /// item constructor.
        Item();

        Item(Item &) = delete;
        /// item virtual destructor.
        virtual ~Item();

        /// @defgroup inconsistent  inconsistent size/offset accessors and setters
        /// all these elements should be checked for naming/use consistency
        /// possibly all of that should be handled via area() (and area should have callback pinned from Item on resize
        /// @{
        void setX(const Position x);
        void setY(const Position y);
        [[nodiscard]] Position getX() const
        {
            return (widgetArea.x);
        }
        [[nodiscard]] Position getY() const
        {
            return (widgetArea.y);
        }
        [[nodiscard]] gui::Length getWidth() const
        {
            return (widgetArea.w);
        }
        [[nodiscard]] gui::Length getHeight() const
        {
            return (widgetArea.h);
        }
        /// helper function to show where widget ends in x axis
        /// @return item ends position in X axis
        [[nodiscard]] Position offset_w() const
        {
            return getWidth() + widgetArea.x;
        }
        /// helper function to show where widget ends in y axis
        /// @return item ends position in Y axis
        [[nodiscard]] Position offset_h() const
        {
            return getHeight() + widgetArea.y;
        }
        /// helper function to show where widget ends in selected axis
        /// @return item ends position in selected axis
        [[nodiscard]] Position getOffset(Axis axis) const
        {
            return this->widgetArea.size(axis) + this->widgetArea.pos(axis);
        };
        /// @}

        /// adds timer to GUI item.
        /// @note this is needed so that timer for element would live as long as element lives.
        /// @details Timers can be attached to Item
        /// in order to pass on an ownership of timer to application/widget which uses its functionalities.
        void attachTimer(sys::Timer *timer)
        {
            timers.push_back(timer);
        }

        /// remove timer from item and as a result - destory it.
        void detachTimer(sys::Timer &timer);

        /// simple check function to determine if item is active && visible.
        /// @return true if item is active and visible. Otherwise false.
        inline bool isActive() const
        {
            return (activeItem && visible);
        }
        /// @brief
        virtual void accept(GuiVisitor &visitor);

      protected:
        /// On change of position or size this method will recalculate visible part of the widget
        /// considering widgets hierarchy and calculate absolute position of drawing primitives.
        virtual void updateDrawArea();
        /// builds draw commands for all of item's children
        /// @param `commandlist` : commands list of commands for renderer to draw elements on screen
        virtual void buildChildrenDrawList(std::list<Command> &commands) final;
        /// Pointer to navigation object. It is added when object is set for one of the directions
        gui::Navigation *navigationDirections = nullptr;

      private:
        /// list of attached timers to item.
        std::list<sys::Timer *> timers;
    };
    /// gets navigation direction (LEFT,RIGHT,UP,DOWN) based on incoming input event
    /// @param[in] evt : input event e.g. key pressed
    NavigationDirection inputToNavigation(const InputEvent &evt);
    /// checks whether input event is related to GUI navigation directions (LEFT,RIGHT,UP,DOWN)
    /// @param evt : input event e.g. key pressed
    bool isInputNavigation(const InputEvent &evt);
} /* namespace gui */
