#pragma once

#include <cstdint>

#include "Layout.hpp"
#include "Rect.hpp"
#include <Alignment.hpp>
#include "LayoutSizeStore.hpp"
#include "log/log.hpp"

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

        template <Axis axis> Length sizeUsed(Item *it, Item::Area area = Item::Area::Min)
        {
            Length sum = 0;

            std::for_each(it->children.begin(), it->children.end(), [&](auto &el) {
                sum += el->visible ? el->area(area).size(axis) + el->getMargins().getSumInAxis(axis) : 0;
            });
            return sum;
        };

        template <Axis axis> Length sizeUsedWithoutElem(Item *it, Item *elem, Item::Area area = Item::Area::Min)
        {
            Length sum = 0;

            std::for_each(it->children.begin(), it->children.end(), [&](auto &el) {
                sum += el->visible ? el->area(area).size(axis) + el->getMargins().getSumInAxis(axis) : 0;
            });

            return sum <= elem->area(area).size(axis) ? 0 : sum - elem->area(area).size(axis);
        };

        template <Axis axis> Length sizeLeft(Item *it, Item::Area area = Item::Area::Min)
        {
            return (sizeUsed<axis>(it, area) >= it->getSize(axis)) ? 0 : it->getSize(axis) - sizeUsed<axis>(it, area);
        };

        template <Axis axis> Length sizeLeftWithoutElem(Item *it, Item *elem, Item::Area area = Item::Area::Min)
        {
            return (sizeUsedWithoutElem<axis>(it, elem, area) >= it->getSize(axis))
                       ? 0
                       : it->getSize(axis) - sizeUsedWithoutElem<axis>(it, elem, area);
        };

        template <Axis axis> void resizeItems();
        template <Axis axis>[[nodiscard]] Position getAxisAlignmentValue(Position calcPos, Length calcSize, Item *el);

        std::list<Item *> outOfDrawAreaItems;
        void addToOutOfDrawAreaList(Item *item);
        void addFromOutOfDrawAreaList();
        virtual void resizeItems();
        bool reverseOrder = false;

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
        std::list<DrawCommand *> buildDrawList() override;
        /// add item if it will fit in box, return true on success
        /// axis sets direction to define space left in container
        template <Axis axis> void addWidget(Item *item);
        /// set navigation from last to fist element in box
        virtual void setNavigation();
        std::list<Item *>::iterator getNavigationFocusedItem();
        void setVisible(bool value) override;
        /// set visible but from previous scope... (page, element etc)
        void setVisible(bool value, bool previous);
        void setReverseOrder(bool value);
        /// callback for situaton when we reached top/bottom/left/right of box
        /// if we want to do sth special (i.e. request new items)
        std::function<bool(const InputEvent &inputEvent)> borderCallback = nullptr;
        // set focus on specified box element
        void setFocusOnElement(unsigned int elementNumber);
        void setFocusOnLastElement();
        template <Axis axis>
        auto handleRequestResize(const Item *, unsigned short request_w, unsigned short request_h) -> Size;
        auto onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) -> bool override;
    };

    class HBox : public BoxLayout
    {
      public:
        void resizeItems() override;
        HBox();
        HBox(Item *parent, const uint32_t &x = 0, const uint32_t &y = 0, const uint32_t &w = 0, const uint32_t &h = 0);
        virtual ~HBox() = default;
        virtual void addWidget(Item *item) override;
        auto handleRequestResize(const Item *, unsigned short request_w, unsigned short request_h) -> Size override;
        Length getSizeLeft();
    };

    class VBox : public BoxLayout
    {
      public:
        void resizeItems() override;
        VBox();
        VBox(Item *parent, const uint32_t &x = 0, const uint32_t &y = 0, const uint32_t &w = 0, const uint32_t &h = 0);
        virtual ~VBox() = default;
        virtual void addWidget(Item *item) override;
        auto handleRequestResize(const Item *, unsigned short request_w, unsigned short request_h) -> Size override;
        Length getSizeLeft();
    };

} /* namespace gui */
