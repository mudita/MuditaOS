// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>

#include "Layout.hpp"
#include "Rect.hpp"
#include <Alignment.hpp>
#include "LayoutSizeStore.hpp"
#include <log/log.hpp>

namespace gui
{

    class BoxLayout : public Rect, Layout
    {
      protected:
        struct BoxElement
        {
            BoxElement(Item *item) : item{item}, noUpdate{false} {};
            Item *item;
            bool noUpdate;
        };

        template <Axis axis> Length sizeUsed(Item *box, Area area = Area::Min)
        {
            Length sum = 0;

            std::for_each(box->children.begin(), box->children.end(), [&](auto &child) {
                sum += child->visible ? child->area(area).size(axis) + child->getMargins().getSumInAxis(axis) : 0;
            });
            return sum;
        };

        template <Axis axis> Length sizeUsedWithoutElem(Item *box, Item *elem, Area area = Area::Min)
        {
            Length sum = 0;

            std::for_each(box->children.begin(), box->children.end(), [&](auto &child) {
                if (child != elem) {
                    sum += child->visible ? child->area(area).size(axis) + child->getMargins().getSumInAxis(axis) : 0;
                }
            });
            return sum;
        };

        template <Axis axis> Length sizeLeft(Item *box, Area area = Area::Min)
        {
            return (sizeUsed<axis>(box, area) >= box->getSize(axis)) ? 0
                                                                     : box->getSize(axis) - sizeUsed<axis>(box, area);
        };

        template <Axis axis> Length sizeLeftWithoutElem(Item *box, Item *elem, Area area = Area::Min)
        {
            return (sizeUsedWithoutElem<axis>(box, elem, area) >= box->getSize(axis))
                       ? 0
                       : box->getSize(axis) - sizeUsedWithoutElem<axis>(box, elem, area);
        };

        template <Axis axis> void resizeItems();
        template <Axis axis>[[nodiscard]] Position getAxisAlignmentValue(Position calcPos, Length calcSize, Item *el);

        std::list<Item *> outOfDrawAreaItems;
        void addToOutOfDrawAreaList(Item *item);
        void addFromOutOfDrawAreaList();
        virtual void resizeItems();
        bool reverseOrder = false;

        template <Axis axis>[[nodiscard]] Length calculateElemResize(Item *el, Length &toSplit);
        template <Axis axis>
        [[nodiscard]] Length calculateElemAxisSize(Item *el, Length calculatedResize, Length &toSplit);
        template <Axis axis>[[nodiscard]] Length calculateElemOrtAxisSize(Item *el);
        template <Axis axis>
        [[nodiscard]] Position calculateElemAxisPosition(Item *el,
                                                         Length axisItemSize,
                                                         Position &startingPosition,
                                                         Position &leftPosition);
        template <Axis axis>[[nodiscard]] Position calculateElemOrtAxisPosition(Item *el, Length orthogonalItemSize);

        Item *getLastVisibleElement();

        /// get next navigation item including `from` item, ecludes not visible items and not acvite items
        std::list<Item *>::iterator nextNavigationItem(std::list<Item *>::iterator from);

      public:
        BoxLayout();
        BoxLayout(Item *parent, const uint32_t &x, const uint32_t &y, const uint32_t &w, const uint32_t &h);
        bool onInput(const InputEvent &inputEvent) override;
        virtual bool onFocus(bool state) override;
        virtual ~BoxLayout() = default;

        // virtual methods from Item
        void setAlignment(const Alignment &value) override;
        void addWidget(gui::Item *item) override;
        bool removeWidget(Item *item) override;
        bool erase(Item *item) override;
        void erase() override;
        [[nodiscard]] bool empty() const noexcept;
        /// add item if it will fit in box, return true on success
        /// axis sets direction to define space left in container
        template <Axis axis> void addWidget(Item *item);
        /// set navigation from last to fist element in box
        virtual void setNavigation();
        std::list<Item *>::iterator getNavigationFocusedItem();
        [[nodiscard]] unsigned int getFocusItemIndex() const;
        [[nodiscard]] unsigned int getVisibleChildrenCount();
        /// If requested causes box to change its structure parent may need to do some actions via this callback.
        std::function<void()> parentOnRequestedResizeCallback = nullptr;
        void setVisible(bool value) override;
        /// set visible but from previous scope... (page, element etc)
        void setVisible(bool value, bool previous);
        void setReverseOrder(bool value);
        [[nodiscard]] bool getReverseOrder();
        /// callback for situaton when we reached top/bottom/left/right of box
        /// if we want to do sth special (i.e. request new items)
        std::function<bool(const InputEvent &inputEvent)> borderCallback = nullptr;
        /// set focus on specified box element
        bool setFocusOnElement(unsigned int elementNumber);
        void setFocusOnLastElement();
        template <Axis axis> auto handleRequestResize(const Item *, Length request_w, Length request_h) -> Size;
        auto onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) -> bool override;
        /// Get primary sizes used in axis dominant layouts
        Length getPrimarySizeLeft();
        Length getPrimarySize();
    };

    class HBox : public BoxLayout
    {
      public:
        void resizeItems() override;
        HBox();
        HBox(Item *parent, const uint32_t &x = 0, const uint32_t &y = 0, const uint32_t &w = 0, const uint32_t &h = 0);
        virtual ~HBox() = default;
        virtual void addWidget(Item *item) override;
        auto handleRequestResize(const Item *, Length request_w, Length request_h) -> Size override;
    };

    class VBox : public BoxLayout
    {
      public:
        void resizeItems() override;
        VBox();
        VBox(Item *parent, const uint32_t &x = 0, const uint32_t &y = 0, const uint32_t &w = 0, const uint32_t &h = 0);
        virtual ~VBox() = default;
        virtual void addWidget(Item *item) override;
        auto handleRequestResize(const Item *, Length request_w, Length request_h) -> Size override;
    };

} /* namespace gui */
