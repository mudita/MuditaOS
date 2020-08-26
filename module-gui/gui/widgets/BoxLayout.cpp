#include "BoxLayout.hpp"
#include <log/log.hpp>
#include <Label.hpp>
#include "BoxLayoutSizeStore.hpp"

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
        if (borderCallback && borderCallback(inputEvent)) {
            outOfDrawAreaItems.clear();
            return true;
        }
        // let item logic rule it
        return false;
    }

    bool BoxLayout::onFocus(bool state)
    {
        if (state)
            this->setVisible(state);
        else
            this->setFocusItem(nullptr);
        this->setNavigation();
        if (this->focusChangedCallback) {
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

    bool BoxLayout::removeWidget(Item *item)
    {
        bool ret = Rect::removeWidget(item);

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

    std::list<DrawCommand *> BoxLayout::buildDrawList()
    {
        auto el = Rect::buildDrawList();
        return el;
    }

    void BoxLayout::setVisible(bool value, bool previous)
    {
        visible = value; // maybe use parent setVisible(...)? would be better but which one?
        if (value == true) {
            resizeItems();         // move items in box in proper places
            setNavigation();       // set navigation through kids -> TODO handle out of last/first to parent
            if (children.size()) { // set first visible kid as focused item - TODO should check for actionItems too...
                /// this if back / front is crappy :|
                if (previous) {
                    for (auto el = children.rbegin(); el != children.rend(); ++el) {
                        if ((*el)->visible && (*el)->activeItem) {
                            setFocusItem(*el);
                            break;
                        }
                    }
                }
                else {
                    for (auto &el : children) {
                        if (el->visible && el->activeItem) {
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

    void BoxLayout::setReverseOrder(bool value)
    {
        reverseOrder = value;
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
        if (children.size() != 0) {
            for (auto it : outOfDrawAreaItems) {
                it->setVisible(true);
            }
        }
        outOfDrawAreaItems.clear();
        resizeItems();
    }

    // space left distposition `first is better` tactics
    // there could be other i.e. socialism: each element take in equal part up to it's max size
    // not needed now == not implemented
    template <Axis axis> void BoxLayout::resizeItems()
    {
        Position pos      = reverseOrder ? this->area().size(axis) : 0;
        Position pos_left = this->getSize(axis);
        Position to_split = sizeLeft<axis>(this);

        for (auto &el : children) {

            if (!el->visible)
                continue;

            auto axisItemPosition       = 0;
            auto orthogonalItemPosition = 0;
            auto axisItemSize           = 0;
            auto orthogonalItemSize     = 0;
            auto grantedSize            = sizeStore->get(el);
            // Check if item can be resized
            Position left_in_el = 0;
            if (!grantedSize.isZero()) {
                left_in_el = grantedSize.get(axis) - el->area(Area::Min).size(axis);
            }
            else {
                left_in_el = el->area(Area::Max).size(axis) - el->area(Area::Min).size(axis);
            }
            if (to_split > 0 && left_in_el > 0) {
                int32_t resize = std::min(left_in_el, to_split);
                axisItemSize   = el->area(Area::Min).size(axis) + resize;
                to_split -= resize;
            }
            else {
                axisItemSize = el->area(Area::Min).size(axis);
            }

            Length maxOrthogonalItemInParentSize =
                this->area(Area::Normal).size(orthogonal(axis)) <= el->getMargins().getSumInAxis(orthogonal(axis))
                    ? 0
                    : this->area(Area::Normal).size(orthogonal(axis)) - el->getMargins().getSumInAxis(orthogonal(axis));

            Length maxOrthogonalItemSize =
                el->area(Area::Max).size(orthogonal(axis)) > el->area(Area::Min).size(orthogonal(axis))
                    ? el->area(Area::Max).size(orthogonal(axis))
                    : el->area(Area::Min).size(orthogonal(axis));

            orthogonalItemSize = std::min(maxOrthogonalItemInParentSize, maxOrthogonalItemSize);

            // Check if there is still position left
            if (axisItemSize <= pos_left) {

                if (reverseOrder) {
                    pos -= el->getMargins().getMarginInAxis(axis, MarginInAxis::Second);
                    pos -= axisItemSize;
                    axisItemPosition = pos;
                    pos -= el->getMargins().getMarginInAxis(axis, MarginInAxis::First);
                }

                if (!reverseOrder) {
                    pos += el->getMargins().getMarginInAxis(axis, MarginInAxis::First);
                    axisItemPosition = pos;
                    pos += axisItemSize;
                    pos += el->getMargins().getMarginInAxis(axis, MarginInAxis::Second);
                }

                pos_left -= axisItemSize + el->getMargins().getSumInAxis(axis);
            }
            else {
                addToOutOfDrawAreaList(el);
            }

            // Recalculate lead Axis position if lead axis alignment provided.
            axisItemPosition = getAxisAlignmentValue<axis>(axisItemPosition, axisItemSize, el);

            // Calculate orthogonal Axis position based on Box Alignment or if not specified child Alignment.
            orthogonalItemPosition = el->getAxisAlignmentValue(orthogonal(axis), orthogonalItemSize);

            if (el->visible)
                el->setAreaInAxis(axis, axisItemPosition, orthogonalItemPosition, axisItemSize, orthogonalItemSize);
        }

        Rect::updateDrawArea();
    }

    template <Axis axis> void BoxLayout::addWidget(Item *item)
    {
        Rect::addWidget(item);
        resizeItems<axis>();
    }

    template <Axis axis> Position BoxLayout::getAxisAlignmentValue(Position calcPos, Length calcSize, Item *el)
    {
        auto offset = sizeLeftWithoutElem<axis>(this, el, Area::Normal) <= calcSize
                          ? 0
                          : sizeLeftWithoutElem<axis>(this, el, Area::Normal) - calcSize;

        switch (getAlignment(axis).vertical) {
        case gui::Alignment::Vertical::Top:
            if (reverseOrder) {
                return calcPos - offset;
            }
            break;
        case gui::Alignment::Vertical::Center:
            if (reverseOrder) {
                return calcPos - offset / 2;
            }
            else {
                return calcPos + offset / 2;
            }
            break;
        case gui::Alignment::Vertical::Bottom:
            if (!reverseOrder) {
                return calcPos + offset;
            }
            break;
        default:
            break;
        }

        switch (getAlignment(axis).horizontal) {
        case gui::Alignment::Horizontal::Left:
            if (reverseOrder) {
                return calcPos - offset;
            }
            break;
        case gui::Alignment::Horizontal::Center:
            if (reverseOrder) {
                return calcPos - offset / 2;
            }
            else {
                return calcPos + offset / 2;
            }
            break;
        case gui::Alignment::Horizontal::Right:
            if (!reverseOrder) {
                return calcPos + offset;
            }
            break;
        default:
            break;
        }

        return calcPos;
    }

    std::list<Item *>::iterator BoxLayout::nextNavigationItem(std::list<Item *>::iterator from)
    {
        return std::find_if(from, this->children.end(), [](auto &el) -> bool {
            if (el->visible && el->activeItem) {
                return true;
            }
            return false;
        });
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
        }
    }

    void BoxLayout::setFocusOnElement(unsigned int elementNumber)
    {
        unsigned int i = 0;
        for (auto child : children) {
            if (child->activeItem == true && child->visible == true) {

                if (elementNumber == i) {
                    child->setFocus(true);
                    focusItem = child;
                }
                else {
                    child->setFocus(false);
                }
                ++i;
            }
        }
    }

    template <Axis axis>
    auto BoxLayout::handleRequestResize(const Item *child, unsigned short request_w, unsigned short request_h) -> Size
    {
        if (parent != nullptr) {
            auto [w, h] = requestSize(request_w, request_h);
            request_w   = std::min(w, (Length)request_w);
            request_h   = std::min(h, (Length)request_h);
        }

        auto el = std::find(children.begin(), children.end(), child);
        if (el == std::end(children)) {
            return {0, 0};
        }

        Size granted = {std::min((*el)->area(Area::Max).w, request_w), std::min((*el)->area(Area::Max).h, request_h)};

        sizeStore->store(*el, granted);
        BoxLayout::resizeItems<axis>(); // vs mark dirty
        return granted;
    }

    void BoxLayout::setFocusOnLastElement()
    {
        auto last = true;
        for (auto child = children.rbegin(); child != children.rend(); child++) {

            if ((*child)->activeItem && (*child)->visible && last) {
                (*child)->setFocus(true);
                focusItem = (*child);
                last      = false;
            }
            else {
                (*child)->setFocus(false);
            }
        }
    }

    auto BoxLayout::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) -> bool
    {
        addFromOutOfDrawAreaList();
        resizeItems();
        setNavigation();

        return true;
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

    auto HBox::handleRequestResize(const Item *child, unsigned short request_w, unsigned short request_h) -> Size
    {
        return BoxLayout::handleRequestResize<Axis::X>(child, request_w, request_h);
    }

    Length HBox::getSizeLeft()
    {
        return sizeLeft<Axis::X>(this, Area::Normal);
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

    auto VBox::handleRequestResize(const Item *child, unsigned short request_w, unsigned short request_h) -> Size
    {
        return BoxLayout::handleRequestResize<Axis::Y>(child, request_w, request_h);
    }

    Length VBox::getSizeLeft()
    {
        return sizeLeft<Axis::Y>(this, Area::Normal);
    }

} /* namespace gui */
