/*
 * BoxLayout.hpp
 *
 *  Created on: 16 maj 2019
 *      Author: robert
 */

#ifndef GUI_WIDGETS_BOXLAYOUT_HPP_
#define GUI_WIDGETS_BOXLAYOUT_HPP_

#include <cstdint>

#include "Layout.hpp"
#include "Rect.hpp"

namespace gui {

    class BoxLayout : public Rect
    {
      protected:
        struct BoxElement
        {
            BoxElement(Item *item) : item{item}, noUpdate{false} {};
            Item *item;
            bool noUpdate;
        };

        enum class BoxAxis
        {
            X,
            Y
        };

        std::function<uint32_t(BoxAxis, Item *)> inAxisSpace = [](BoxAxis axis, Item *it) -> uint32_t {
            return (axis == BoxAxis::Y) ? it->widgetArea.h : it->widgetArea.w;
        };
        std::function<uint16_t &(BoxAxis, Item *)> inAxisSize = [](BoxAxis axis, Item *it) -> uint16_t & {
            return (axis == BoxAxis::Y) ? (it->widgetArea.h) : (it->widgetArea.w);
        };
        std::function<int32_t(BoxAxis, Item *)> inAxisOffset = [](BoxAxis axis, Item *it) -> int32_t {
            return (axis == BoxAxis::Y) ? (it->offset_h()) : (it->offset_w());
        };
        std::function<uint32_t(BoxAxis, Item *)> inAxisMax = [](BoxAxis axis, Item *it) -> uint32_t {
            return (axis == BoxAxis::Y) ? (it->getMaxHeight()) : (it->getMaxWidth());
        };

        template <BoxAxis axis> void resizeItems();

        virtual void resizeItems();
        bool reverse_order = false;
        /// get next navigation item including `from` item, ecludes not visible items and not acvite items
        std::list<Item *>::iterator nextNavigationItem(std::list<Item *>::iterator from);

      public:
        BoxLayout();
        BoxLayout(Item *parent, const uint32_t &x, const uint32_t &y, const uint32_t &w, const uint32_t &h);
        virtual bool onFocus(bool state) override;
        virtual ~BoxLayout() = default;

        // virtual methods from Item
        void setPosition(const short &x, const short &y) override;
        void setSize(const short &w, const short &h) override;
        bool addWidget(gui::Item *item) override;
        virtual bool tryAddWidget(Item *item) = 0;
        bool removeWidget(Item *item) override;
        std::list<DrawCommand *> buildDrawList() override;
        /// add item if it will fit in box, return true on success
        /// axis sets direction to define space left in container
        template <BoxLayout::BoxAxis axis> bool tryAddWidget(Item *item);
        /// set navigation from last to fist element in box
        void setNavigation();
        void setVisible(bool value) override;
        /// set visible but from previous scope... (page, element etc)
        void setVisible(bool value, bool previous);
        void setReverseOrder(bool reverse_order)
        {
            if (this->reverse_order != reverse_order)
            {
                this->reverse_order = reverse_order;
                resizeItems();
            }
        }
    };

class HBox : public BoxLayout {
protected:
	void resizeItems() override;
public:
	HBox();
	HBox( Item* parent, const uint32_t& x, const uint32_t& y, const uint32_t& w, const uint32_t& h);
    virtual ~HBox() = default;
    virtual bool tryAddWidget(Item *item) override;
};

class VBox : public BoxLayout {
public:
	void resizeItems() override;
	VBox();
	VBox( Item* parent, const uint32_t& x, const uint32_t& y, const uint32_t& w, const uint32_t& h);
	virtual ~VBox() = default;
    virtual bool tryAddWidget(Item *item) override;
};

} /* namespace gui */

#endif /* GUI_WIDGETS_BOXLAYOUT_HPP_ */
