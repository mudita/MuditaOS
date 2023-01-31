// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BoxLayout.hpp"
#include "BoxLayoutSizeStore.hpp"
#include <InputEvent.hpp>
#include <Label.hpp>
#include <log/log.hpp>
#include "assert.h"

namespace gui
{
    BoxLayout::BoxLayout(Item *parent, const uint32_t &x, const uint32_t &y, const uint32_t &w, const uint32_t &h)
        : Rect(parent, x, y, w, h)
    {
        alignment = gui::Alignment(Alignment::Horizontal::None, Alignment::Vertical::None);
        sizeStore = std::make_unique<BoxLayoutSizeStore>();
    }

    BoxLayout::BoxLayout() : BoxLayout(nullptr, 0, 0, 0, 0)
    {}

    bool BoxLayout::onInput(const InputEvent &inputEvent)
    {
        if (inputCallback && inputCallback(*this, inputEvent)) {
            return true;
        }
        if (focusItem && focusItem->onInput(inputEvent)) {
            return true;
        }
        if (handleNavigation(inputEvent)) {
            return true;
        }
        if (borderCallback && isInputNavigation(inputEvent) && borderCallback(inputEvent)) {
            outOfDrawAreaItems.clear();
            return true;
        }
        // let item logic rule it
        return false;
    }

    bool BoxLayout::onFocus(bool state)
    {
        if (state) {
            this->setVisible(state);
        }
        else {
            this->setFocusItem(nullptr);
        }

        this->setNavigation();
        if (this->focusChangedCallback && state != focus) {
            this->focusChangedCallback(*this);
        }
        return true;
    }

    void BoxLayout::resizeItems()
    {}

    void BoxLayout::setAlignment(const Alignment &value)
    {
        if (alignment != value) {
            alignment = value;
            resizeItems();
        }
    }

    void BoxLayout::addWidget(Item *item)
    {
        Rect::addWidget(item);
        resizeItems();
    }

    template <Axis axis>
    void BoxLayout::addWidget(Item *item)
    {
        Rect::addWidget(item);
        resizeItems<axis>();
    }

    bool BoxLayout::removeWidget(Item *item)
    {
        const auto ret = Rect::removeWidget(item);

        outOfDrawAreaItems.remove(item);
        resizeItems();

        return ret;
    }

    bool BoxLayout::erase(Item *item)
    {
        auto ret = Item::erase(item);

        outOfDrawAreaItems.remove(item);
        resizeItems();

        return ret;
    }

    void BoxLayout::erase()
    {
        Item::erase();
    }

    bool BoxLayout::empty() const noexcept
    {
        return children.empty();
    }

    void BoxLayout::setVisible(bool value, bool previous)
    {
        visible = value; // maybe use parent setVisible(...)? would be better but which one?
        if (value) {
            resizeItems();         // move items in box in proper places
            setNavigation();       // set navigation through kids -> TODO handle out of last/first to parent
            if (children.size()) { // set first visible kid as focused item - TODO should check for actionItems too...
                /// this if back / front is crappy :|
                if (previous) {
                    for (auto el = children.rbegin(); el != children.rend(); ++el) {
                        if ((*el)->isActive()) {
                            setFocusItem(*el);
                            break;
                        }
                    }
                }
                else {
                    for (auto &el : children) {
                        if (el->isActive()) {
                            setFocusItem(el);
                            break;
                        }
                    }
                }
            }
        }
    }

    void BoxLayout::setVisible(bool value)
    {
        setVisible(value, false);
    }

    unsigned int BoxLayout::getVisibleChildrenCount()
    {
        assert(children.size() >= outOfDrawAreaItems.size());
        return children.size() - outOfDrawAreaItems.size();
    }

    void BoxLayout::setReverseOrder(bool value)
    {
        reverseOrder = value;
    }

    bool BoxLayout::getReverseOrder()
    {
        return reverseOrder;
    }

    Length BoxLayout::getPrimarySize()
    {
        if (type == ItemType::HBOX) {
            return getSize(Axis::X);
        }
        else if (type == ItemType::VBOX) {
            return getSize(Axis::Y);
        }

        return 0;
    }

    Length BoxLayout::getPrimarySizeLeft()
    {
        if (type == ItemType::HBOX) {
            return sizeLeft<Axis::X>(this, Area::Normal);
        }
        else if (type == ItemType::VBOX) {
            return sizeLeft<Axis::Y>(this, Area::Normal);
        }

        return 0;
    }

    void BoxLayout::addToOutOfDrawAreaList(Item *it)
    {
        if (it->visible) {
            outOfDrawAreaItems.push_back(it);
            it->visible = false;
        }
    }

    void BoxLayout::addFromOutOfDrawAreaList()
    {
        if (!children.empty()) {
            for (auto it : outOfDrawAreaItems) {
                it->setVisible(true);
                it->setFocusItem(nullptr);
            }
        }

        outOfDrawAreaItems.clear();
        resizeItems();
    }

    // space left distposition `first is better` tactics
    // there could be other i.e. socialism: each element take in equal part up to it's max size
    // not needed now == not implemented
    template <Axis axis>
    void BoxLayout::resizeItems()
    {
        Position startingPosition = reverseOrder ? this->area().size(axis) : 0;
        Position leftPosition     = this->getSize(axis);
        Length toSplit            = sizeLeft<axis>(this);

        for (auto &el : children) {

            if (!el->visible) {
                continue;
            }

            auto axisItemPosition       = 0;
            auto orthogonalItemPosition = 0;
            auto axisItemSize           = 0;
            auto calculatedResize       = 0;
            auto orthogonalItemSize     = 0;

            // 1. Calculate element axis resize.
            calculatedResize = calculateElemResize<axis>(el, toSplit);

            // 2. Check if new size in axis can be applied and set it or use element minimal size in axis.
            axisItemSize = calculateElemAxisSize<axis>(el, calculatedResize, toSplit);

            // 3. Calculate orthogonal axis size.
            orthogonalItemSize = calculateElemOrtAxisSize<axis>(el);

            // 4. Calculate element position in axis and apply in axis alignment.
            axisItemPosition = calculateElemAxisPosition<axis>(el, axisItemSize, startingPosition, leftPosition);

            // 5. Calculate element orthogonal axis position based on Layout alignment or on element alignment.
            orthogonalItemPosition = calculateElemOrtAxisPosition<axis>(el, orthogonalItemSize);

            // 6. If element still visible (not added to outOfDrawAreaList) set it Area with calculated values.
            if (el->visible) {
                el->setAreaInAxis(axis, axisItemPosition, orthogonalItemPosition, axisItemSize, orthogonalItemSize);
            }
        }

        Rect::updateDrawArea();
    }

    template <Axis axis>
    Length BoxLayout::calculateElemResize(Item *el, Length &toSplit)
    {
        auto grantedSize        = sizeStore->get(el);
        Length calculatedResize = 0;

        // Check if element has stored requested size in axis.
        if (!grantedSize.isZero()) {
            calculatedResize = grantedSize.get(axis) < el->area(Area::Min).size(axis)
                                   ? 0
                                   : grantedSize.get(axis) - el->area(Area::Min).size(axis);

            // If requested size bigger than left size in layout push out last visible element in layout into
            // outOfDrawAreaList.
            if (sizeLeft<axis>(this) < calculatedResize) {
                addToOutOfDrawAreaList(getLastVisibleElement());
                toSplit = sizeLeft<axis>(this);
            }
        }
        else {
            // If not calculate possible size in axis from min-max difference.
            calculatedResize = el->area(Area::Max).size(axis) < el->area(Area::Min).size(axis)
                                   ? 0
                                   : el->area(Area::Max).size(axis) - el->area(Area::Min).size(axis);
        }

        return calculatedResize;
    }

    template <Axis axis>
    Length BoxLayout::calculateElemAxisSize(Item *el, Length calculatedResize, Length &toSplit)
    {
        Length axisItemSize = 0;

        if (toSplit > 0 && calculatedResize > 0) {
            axisItemSize = el->area(Area::Min).size(axis) + std::min(calculatedResize, toSplit);
            toSplit -= std::min(calculatedResize, toSplit);
        }
        else {
            axisItemSize = el->area(Area::Min).size(axis);
        }

        return axisItemSize;
    }

    template <Axis axis>
    Length BoxLayout::calculateElemOrtAxisSize(Item *el)
    {
        // Get maximum size that element in orthogonal axis can occupy in current layout size.
        const Length maxOrthogonalItemInParentSize =
            static_cast<Position>(this->area(Area::Normal).size(orthogonal(axis))) <=
                    el->getMargins().getSumInAxis(orthogonal(axis))
                ? 0
                : this->area(Area::Normal).size(orthogonal(axis)) - el->getMargins().getSumInAxis(orthogonal(axis));

        // Get maximum size of element in orthogonal axis based on its min-max.
        const Length maxOrthogonalItemSize =
            el->area(Area::Max).size(orthogonal(axis)) > el->area(Area::Min).size(orthogonal(axis))
                ? el->area(Area::Max).size(orthogonal(axis))
                : el->area(Area::Min).size(orthogonal(axis));

        // Return minimal from both max sizes.
        return std::min(maxOrthogonalItemInParentSize, maxOrthogonalItemSize);
    }

    template <Axis axis>
    Position BoxLayout::calculateElemAxisPosition(Item *el,
                                                  Length axisItemSize,
                                                  Position &startingPosition,
                                                  Position &leftPosition)
    {
        auto axisItemPosition = 0;

        // Check if elements in axis can fit with margins in layout free space.
        if (static_cast<Position>(axisItemSize + el->getMargins().getSumInAxis(axis)) <= leftPosition) {

            if (reverseOrder) {
                startingPosition -= el->getMargins().getMarginInAxis(axis, MarginInAxis::Second);
                startingPosition -= axisItemSize;
                axisItemPosition = startingPosition;
                startingPosition -= el->getMargins().getMarginInAxis(axis, MarginInAxis::First);
            }

            if (!reverseOrder) {
                startingPosition += el->getMargins().getMarginInAxis(axis, MarginInAxis::First);
                axisItemPosition = startingPosition;
                startingPosition += axisItemSize;
                startingPosition += el->getMargins().getMarginInAxis(axis, MarginInAxis::Second);
            }

            leftPosition -= axisItemSize + el->getMargins().getSumInAxis(axis);

            // Recalculate element axis position if axis alignment provided.
            axisItemPosition = getAxisAlignmentValue<axis>(axisItemPosition, axisItemSize, el);
        }
        else {
            // If not add it to outOfDrawAreaList.
            addToOutOfDrawAreaList(el);
        }

        return axisItemPosition;
    }

    template <Axis axis>
    Position BoxLayout::calculateElemOrtAxisPosition(Item *el, Length orthogonalItemSize)
    {
        return el->getAxisAlignmentValue(orthogonal(axis), orthogonalItemSize);
    }

    template <Axis axis>
    Position BoxLayout::getAxisAlignmentValue(Position calcPos, Length calcSize, Item *el)
    {
        auto offset = sizeLeftWithoutElem<axis>(this, el, Area::Normal) <= calcSize
                          ? 0
                          : sizeLeftWithoutElem<axis>(this, el, Area::Normal) - calcSize;

        switch (getAlignment(axis).vertical) {
        case gui::Alignment::Vertical::Top:
            if (reverseOrder) {
                return calcPos + el->getMargins().getSumInAxis(axis) - offset;
            }
            break;
        case gui::Alignment::Vertical::Center:
            if (reverseOrder) {
                return calcPos - ((offset - el->getMargins().getSumInAxis(axis)) / 2);
            }
            else {
                return calcPos + ((offset - el->getMargins().getSumInAxis(axis)) / 2);
            }
            break;
        case gui::Alignment::Vertical::Bottom:
            if (!reverseOrder) {
                return calcPos - el->getMargins().getSumInAxis(axis) + offset;
            }
            break;
        default:
            break;
        }

        switch (getAlignment(axis).horizontal) {
        case gui::Alignment::Horizontal::Left:
            if (reverseOrder) {
                return calcPos + el->getMargins().getSumInAxis(axis) - offset;
            }
            break;
        case gui::Alignment::Horizontal::Center:
            if (reverseOrder) {
                return calcPos - ((offset - el->getMargins().getSumInAxis(axis)) / 2);
            }
            else {
                return calcPos + ((offset - el->getMargins().getSumInAxis(axis)) / 2);
            }
            break;
        case gui::Alignment::Horizontal::Right:
            if (!reverseOrder) {
                return calcPos - el->getMargins().getSumInAxis(axis) + offset;
            }
            break;
        default:
            break;
        }

        return calcPos;
    }

    std::list<Item *>::iterator BoxLayout::nextNavigationItem(std::list<Item *>::iterator from)
    {
        return std::find_if(from, this->children.end(), [](const auto &el) { return el->isActive(); });
    }

    std::list<Item *>::iterator BoxLayout::getNavigationFocusedItem()
    {
        return std::find(this->children.begin(), this->children.end(), this->getFocusItem());
    }

    void BoxLayout::setNavigation()
    {
        auto previous = nextNavigationItem(children.begin()), next = children.end();
        if (type == ItemType::VBOX) {
            while ((previous != children.end()) &&
                   ((next = nextNavigationItem(std::next(previous))) != std::end(children))) {
                (*previous)->setNavigationItem(reverseOrder ? NavigationDirection::UP : NavigationDirection::DOWN,
                                               *next);
                (*next)->setNavigationItem(reverseOrder ? NavigationDirection::DOWN : NavigationDirection::UP,
                                           *previous);
                previous = next;
            }

            if (previous != children.end()) {
                if ((*previous) != nullptr) {
                    (*previous)->setNavigationItem(reverseOrder ? NavigationDirection::UP : NavigationDirection::DOWN,
                                                   nullptr);
                }
            }
        }

        if (type == ItemType::HBOX) {
            while ((previous != children.end()) &&
                   ((next = nextNavigationItem(std::next(previous))) != std::end(children))) {
                (*previous)->setNavigationItem(reverseOrder ? NavigationDirection::LEFT : NavigationDirection::RIGHT,
                                               *next);
                (*next)->setNavigationItem(reverseOrder ? NavigationDirection::RIGHT : NavigationDirection::LEFT,
                                           *previous);
                previous = next;
            }

            if (previous != children.end()) {
                if ((*previous) != nullptr) {
                    (*previous)->setNavigationItem(
                        reverseOrder ? NavigationDirection::LEFT : NavigationDirection::RIGHT, nullptr);
                }
            }
        }
    }

    template <Axis axis>
    Size BoxLayout::handleRequestResize(const Item *child, Length request_w, Length request_h)
    {
        if (parent != nullptr) {
            auto [w, h] = requestSize(request_w, request_h);
            request_w   = std::min(w, static_cast<Length>(request_w));
            request_h   = std::min(h, static_cast<Length>(request_h));
        }

        auto el = std::find(children.begin(), children.end(), child);
        if (el == std::end(children)) {
            return {0, 0};
        }

        Size granted = {std::min((*el)->area(Area::Max).w, request_w), std::min((*el)->area(Area::Max).h, request_h)};

        // Currently not used option for Layouts that don't push out objects.
        if (!sizeStore->getReleaseSpaceFlag()) {
            if ((granted.get(axis) + (*el)->getMargins().getSumInAxis(axis)) >=
                sizeLeftWithoutElem<axis>(this, *el, Area::Min)) {

                granted = Size((*el)->area(Area::Normal).w, (*el)->area(Area::Normal).h);
            }
        }

        // If granted size decreased check if pushed out elements can fit
        if (sizeStore->isSizeSmaller(*el, granted, axis)) {
            addFromOutOfDrawAreaList();
        }

        sizeStore->store(*el, granted);
        resizeItems(); // vs mark dirty
        setNavigation();

        if (parentOnRequestedResizeCallback != nullptr) {
            parentOnRequestedResizeCallback();
        }

        return granted;
    }

    bool BoxLayout::setFocusOnElement(unsigned int elementNumber)
    {
        unsigned int i = 0;
        bool success   = false;

        for (auto child : children) {
            if (child->isActive()) {

                if (elementNumber == i) {
                    child->setFocus(true);
                    focusItem = child;
                    success   = true;
                }
                else {
                    child->setFocus(false);
                }
                ++i;
            }
        }

        return success;
    }

    void BoxLayout::setFocusOnLastElement()
    {
        auto last = true;
        for (auto child = children.rbegin(); child != children.rend(); child++) {

            if ((*child)->isActive() && last) {
                (*child)->setFocus(true);
                focusItem = (*child);
                last      = false;
            }
            else {
                (*child)->setFocus(false);
            }
        }
    }

    unsigned int BoxLayout::getFocusItemIndex() const
    {
        auto index     = 0;
        auto focusItem = getFocusItem();

        for (auto child : children) {
            if (child == focusItem) {
                break;
            }
            if (child->isActive()) {
                index++;
            }
        }
        return index;
    }

    Item *BoxLayout::getLastVisibleElement()
    {
        for (auto child = children.rbegin(); child != children.rend(); child++) {
            if ((*child)->visible) {
                return (*child);
            }
        }
        return nullptr;
    }

    bool BoxLayout::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        addFromOutOfDrawAreaList();
        resizeItems();
        setNavigation();

        return Item::onDimensionChanged(oldDim, newDim);
    }

    void BoxLayout::handleContentChanged()
    {
        // Check if there is parent and it is Layout
        if (parent != nullptr && (dynamic_cast<gui::BoxLayout *>(parent) != nullptr)) {
            // If so invalidate content and request Content change to parent
            contentChanged = true;
            Item::informContentChanged();
        }
        else {
            resizeItems();
        }
    }

    HBox::HBox() : BoxLayout()
    {
        type = ItemType::HBOX;
    }

    HBox::HBox(Item *parent, const uint32_t &x, const uint32_t &y, const uint32_t &w, const uint32_t &h)
        : BoxLayout(parent, x, y, w, h)
    {
        type = ItemType::HBOX;
    }

    void HBox::resizeItems()
    {
        BoxLayout::resizeItems<Axis::X>();
    }

    void HBox::addWidget(Item *item)
    {
        BoxLayout::addWidget<Axis::X>(item);
    }

    Size HBox::handleRequestResize(const Item *child, Length request_w, Length request_h)
    {
        return BoxLayout::handleRequestResize<Axis::X>(child, request_w, request_h);
    }

    VBox::VBox() : BoxLayout()
    {
        type = ItemType::VBOX;
    }

    VBox::VBox(Item *parent, const uint32_t &x, const uint32_t &y, const uint32_t &w, const uint32_t &h)
        : BoxLayout(parent, x, y, w, h)
    {
        type = ItemType::VBOX;
    }

    void VBox::resizeItems()
    {
        BoxLayout::resizeItems<Axis::Y>();
    }

    void VBox::addWidget(Item *item)
    {
        BoxLayout::addWidget<Axis::Y>(item);
    }

    Size VBox::handleRequestResize(const Item *child, Length request_w, Length request_h)
    {
        return BoxLayout::handleRequestResize<Axis::Y>(child, request_w, request_h);
    }

} /* namespace gui */
