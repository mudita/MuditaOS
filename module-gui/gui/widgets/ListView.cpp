#include "ListView.hpp"
#include <log/log.hpp>
#include "cassert"
#include <algorithm>

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

    void ListViewScroll::update(int startIndex, int currentPageSize, int elementsCount, int topMargin)
    {
        if (shouldShowScroll(currentPageSize, elementsCount)) {

            assert(elementsCount != 0);
            double scrollStep =
                static_cast<double>((parent->widgetArea.h - topMargin)) / static_cast<double>(elementsCount);

            auto scrollH = scrollStep * currentPageSize;
            auto scrollY = scrollStep * startIndex > 0 ? scrollStep * startIndex : topMargin;

            setArea(BoundingBox(
                parent->widgetArea.w - style::listview::scroll::margin, scrollY, style::listview::scroll::w, scrollH));
            setVisible(true);
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

    ListView::ListView(
        Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h, std::shared_ptr<ListItemProvider> prov)
        : Rect{parent, x, y, w, h}
    {

        this->setBorderColor(ColorNoColor);

        body = new VBox{this, 0, 0, w, h};
        body->setBorderColor(ColorNoColor);

        body->borderCallback = [this](const InputEvent &inputEvent) -> bool {
            if (inputEvent.state != InputEvent::State::keyReleasedShort) {
                return false;
            }
            if (inputEvent.keyCode == KeyCode::KEY_UP && pageLoaded) {
                direction = style::listview::Direction::Top;
                return this->listPageEndReached();
            }
            else if (inputEvent.keyCode == KeyCode::KEY_DOWN && pageLoaded) {
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

        setProvider(prov);

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

    void ListView::setScrollTopMargin(int value)
    {
        scrollTopMargin = value;
    }

    void ListView::setProvider(std::shared_ptr<ListItemProvider> prov)
    {
        provider = prov;
        if (provider != nullptr) {
            provider->list = this;
            setElementsCount(provider->getItemCount());
            provider->requestRecords(0, calculateLimit());
        }
        refresh();
    }

    std::shared_ptr<ListItemProvider> ListView::getProvider()
    {
        return provider;
    }

    void ListView::clear()
    {
        clearItems();
        body->setReverseOrder(false);
        startIndex = 0;
        direction  = style::listview::Direction::Bottom;
    }

    void ListView::clearItems()
    {
        while (auto el = body->children.back()) {

            if (el->type == ItemType::LIST_ITEM) {

                if (!dynamic_cast<ListItem *>(el)->deleteByList) {
                    body->removeWidget(el);
                }
                else {
                    body->erase(el);
                }
            }
            else
                body->erase(el);
        }
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
        scroll->update(startIndex, currentPageSize, elementsCount, scrollTopMargin);
        resizeWithScroll();
        body->axisAlignment();
        pageLoaded = true;
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
                break;
            }

            currentPageSize++;
        }

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

    int ListView::calculateMaxItemsOnPage()
    {
        assert(provider->getMinimalItemHeight() != 0);
        auto count = widgetArea.h / provider->getMinimalItemHeight();

        return count;
    }

    int ListView::calculateLimit()
    {
        auto minLimit =
            (2 * currentPageSize > calculateMaxItemsOnPage() ? 2 * currentPageSize : calculateMaxItemsOnPage());
        if (direction == style::listview::Direction::Bottom)
            return (minLimit + startIndex <= elementsCount ? minLimit : elementsCount - startIndex);
        else
            return minLimit < startIndex ? minLimit : startIndex;
    }

    bool ListView::listPageEndReached()
    {
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

            pageLoaded = false;
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

            pageLoaded = false;

            // If starting page size smaller than last page - fill first page with last page size.
            if (startIndex < currentPageSize) {

                provider->requestRecords(0, currentPageSize);
            }
            else {

                provider->requestRecords(topFetchIndex, calculateLimit());
            }
        }

        return true;
    }

} /* namespace gui */
