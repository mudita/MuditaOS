#include <algorithm>

#include "ListView.hpp"
#include <log/log.hpp>

namespace gui
{

    ListViewScroll::ListViewScroll(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
        : Rect{parent, x, y, w, h}
    {

        setRadius(style::listview::scroll::radius);
        setFilled(true);
        setFillColor(style::listview::scroll::color);
        activeItem = false;
    }

    bool ListViewScroll::shouldShowScroll(int currentPageSize, int elementsCount)
    {
        return ((parent->widgetArea.w > style::listview::scroll::min_space) &&
                (parent->widgetArea.h > style::listview::scroll::min_space) && currentPageSize < elementsCount);
    }

    void ListViewScroll::update(int startIndex, int currentPageSize, int elementsCount)
    {
        if (shouldShowScroll(currentPageSize, elementsCount)) {

            uint32_t pagesCount = 1;
            if (currentPageSize) {
                pagesCount = (elementsCount % currentPageSize == 0) ? elementsCount / currentPageSize
                                                                    : elementsCount / currentPageSize + 1;
                if (pagesCount == 0) {
                    return;
                }
            }

            if (currentPageSize != 0 && pagesCount != 0) {
                uint32_t currentPage = startIndex / currentPageSize;
                uint32_t pageHeight  = parent->widgetArea.h / pagesCount;

                setPosition(parent->widgetArea.w - style::listview::scroll::margin, pageHeight * currentPage);
                setSize(style::listview::scroll::w, pageHeight);
                setVisible(true);
            }
        }
        else
            setVisible(false);
    }

    ListView::ListView()
    {

        body = new VBox{this, 0, 0, 0, 0};

        scroll = new ListViewScroll(this,
                                    style::listview::scroll::x,
                                    style::listview::scroll::y,
                                    style::listview::scroll::w,
                                    style::listview::scroll::h);
        type   = gui::ItemType::LIST;
    }

    ListView::ListView(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h) : Rect{parent, x, y, w, h}
    {

        this->setBorderColor(ColorNoColor);
        //        this->setListViewType(style::listview::Type::Continuous);

        body = new VBox{this, 0, 0, w, h};
        body->setBorderColor(ColorNoColor);
        body->setAxisAlignment(true);

        body->borderCallback = [this](const InputEvent &inputEvent) -> bool {
            if (inputEvent.state != InputEvent::State::keyReleasedShort) {
                return false;
            }
            if (inputEvent.keyCode == KeyCode::KEY_UP) {
                direction = style::listview::Direction::Top;
                return this->listPageEndReached();
            }
            else if (inputEvent.keyCode == KeyCode::KEY_DOWN) {
                direction = style::listview::Direction::Bottom;
                return this->listPageEndReached();
            }
            else {
                return false;
            }
        };

        scroll = new ListViewScroll(this,
                                    style::listview::scroll::x,
                                    style::listview::scroll::y,
                                    style::listview::scroll::w,
                                    style::listview::scroll::h);

        type = gui::ItemType::LIST;
    }

    ListView::~ListView()
    {
        clearItems();
    }

    void ListView::setElementsCount(int count)
    {
        elementsCount = count;
    }

    void ListView::setListViewType(style::listview::Type type)
    {
        listType = type;
    }

    void ListView::setMinimalPageSize(int size)
    {
        minimalPageSize = size;
    }

    void ListView::setItemSpanSize(int size)
    {
        itemSpanSize = size;
    }

    void ListView::setProvider(ListItemProvider *prov)
    {
        provider = prov;
        if (provider != nullptr) {
            provider->list = this;
            setElementsCount(provider->getItemCount());
        }
        refresh();
    }

    void ListView::clear()
    {
        clearItems();
        startIndex = 0;
    }

    void ListView::clearItems()
    {
        body->erase();
    }

    void ListView::refresh()
    {
        if (provider == nullptr) {
            LOG_ERROR("ListView Data provider not exist");
            return;
        }

        clearItems();

        elementsCount = provider->getItemCount();
        addItemsOnPage();

        setFocus();
        scroll->update(startIndex, currentPageSize, elementsCount);
        resizeWithScroll();
    }

    void ListView::onProviderDataUpdate()
    {
        refresh();
    }

    Order ListView::getOrderFromDirection()
    {
        if (direction == style::listview::Direction::Bottom)
            return Order::Next;

        return Order::Previous;
    }

    void ListView::recalculateStartIndex()
    {
        if (direction == style::listview::Direction::Top) {
            startIndex = startIndex - currentPageSize > 0 ? startIndex - currentPageSize : 0;
        }
    }

    void ListView::addSpanItem()
    {
        listSpanItem = new Span(Axis::Y, itemSpanSize);
        body->addWidget(listSpanItem);
    }

    void ListView::resizeWithScroll()
    {
        if (scroll->shouldShowScroll(currentPageSize, elementsCount)) {
            body->setSize(style::listview::item_width_with_scroll, body->getHeight());
        }
        else {
            body->setSize(style::window::default_body_width, body->getHeight());
        }
    }

    void ListView::addItemsOnPage()
    {
        currentPageSize = 0;

        ListItem *item = nullptr;

        while ((item = provider->getItem(getOrderFromDirection())) != nullptr) {

            body->addWidget(item);

            if (item->visible != true) {
                body->removeWidget(listSpanItem);
                break;
            }

            currentPageSize++;

            addSpanItem();
        }

        if (listSpanItem != nullptr)
            body->removeWidget(listSpanItem);

        recalculateStartIndex();
    }

    void ListView::setFocus()
    {
        setFocusItem(body);
    };

    std::list<DrawCommand *> ListView::buildDrawList()
    {
        // check if widget is visible
        if (visible == false) {
            return std::list<DrawCommand *>();
        }

        return gui::Rect::buildDrawList();
    }

    bool ListView::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        Rect::onDimensionChanged(oldDim, newDim);
        body->setSize(newDim.w, newDim.h);
        refresh();
        return true;
    }

    bool ListView::onInput(const InputEvent &inputEvent)
    {
        return body->onInput(inputEvent);
    }

    bool ListView::listPageEndReached()
    {
        auto minLimit = (2 * currentPageSize > minimalPageSize ? 2 * currentPageSize : minimalPageSize);

        auto calculateLimit = [&]() {
            // Minimal arbitrary number of items requested from database. As ListView does not know how big elements are
            // before it gets them, requests twice size of current page with down limit of at least 8 (minimalPageSize).
            if (direction == style::listview::Direction::Bottom)
                return (minLimit + startIndex <= elementsCount ? minLimit : elementsCount - startIndex);
            else
                return minLimit < startIndex ? minLimit : startIndex;
        };

        if (direction == style::listview::Direction::Bottom) {

            body->setReverseOrder(false);

            if (startIndex + currentPageSize >= elementsCount && listType == style::listview::Type::Continuous) {

                startIndex = 0;
            }
            else if (startIndex + currentPageSize >= elementsCount && listType == style::listview::Type::TopDown) {

                return true;
            }
            else {

                startIndex = startIndex <= elementsCount - currentPageSize
                                 ? startIndex + currentPageSize
                                 : elementsCount - (elementsCount - startIndex);
            }

            LOG_DEBUG("BOTTOM ---------- Start off: %u, offset send: %u, limit: %u, page: %u",
                      startIndex,
                      startIndex,
                      calculateLimit(),
                      currentPageSize);

            provider->requestRecords(startIndex, calculateLimit());
        }

        if (direction == style::listview::Direction::Top) {

            body->setReverseOrder(true);
            auto topFetchIndex = 0;

            if (startIndex == 0 && listType == style::listview::Type::Continuous) {

                topFetchIndex = elementsCount - (elementsCount % currentPageSize);
                startIndex    = elementsCount;
            }
            else if (startIndex == 0 && listType == style::listview::Type::TopDown) {

                return true;
            }
            else {
                topFetchIndex = startIndex - calculateLimit() > 0 ? startIndex - calculateLimit() : 0;
            }

            LOG_DEBUG("TOP ---------------- Start off: %u, offset send: %u, limit: %u, page: %u",
                      startIndex,
                      topFetchIndex,
                      calculateLimit(),
                      currentPageSize);

            provider->requestRecords(topFetchIndex, calculateLimit());
        }

        return true;
    }

} /* namespace gui */
