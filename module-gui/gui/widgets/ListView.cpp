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
    }

    ListView::ListView(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h) : Rect{parent, x, y, w, h}
    {

        this->setBorderColor(ColorNoColor);

        body = new VBox{this, 0, 0, w, h};
        body->setBorderColor(ColorNoColor);

        body->borderCallback = [this](const InputEvent &inputEvent) -> bool {
            if (inputEvent.state != InputEvent::State::keyReleasedShort) {
                return false;
            }
            if (inputEvent.keyCode == KeyCode::KEY_UP) {
                direction = Direction::Top;
                return this->listPageEndReached();
            }
            else if (inputEvent.keyCode == KeyCode::KEY_DOWN) {
                direction = Direction::Bottom;
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
    }

    ListView::~ListView()
    {
        clearItems();
    }

    void ListView::setElementsCount(int count)
    {
        elementsCount = count;
    }

    void ListView::setListViewType(ListViewType type)
    {
        listType = type;
    }

    void ListView::setItemSpanSize(int size)
    {
        itemSpanSize = size;
    }

    void ListView::setProvider(ListItemProvider *prov)
    {
        provider       = prov;
        provider->list = this;
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

    void ListView::resizeWithScroll()
    {
        if (scroll->shouldShowScroll(currentPageSize, elementsCount)) {
            body->setSize(style::listview::item_width_with_scroll, body->h());
        }
        else {
            body->setSize(style::window::default_body_width, body->h());
        }
    }

    void ListView::addItemsOnPage()
    {
        auto itemsOnPage = 0;

        ListItem *item = nullptr;

        while ((item = provider->getItem(itemsOnPage)) != nullptr) {

            item->setSize(item->minWidth, item->minHeight);
            item->setMinimumWidth(item->minWidth);
            item->setMinimumHeight(item->minHeight);

            body->addWidget(item);

            if (item->visible != true) {
                currentPageSize = itemsOnPage;
                break;
            }

            itemsOnPage++;

            listSpanItem = new Span(Axis::Y, itemSpanSize);
            body->addWidget(listSpanItem);
        }

        if (currentPageSize == 0)
            currentPageSize = itemsOnPage;
    }

    void ListView::setFocus()
    {
        if (direction == Direction::Top) {
            setFocusItem(body);
            body->setFocusOnLastElement();
        }
        if (direction == Direction::Bottom) {
            setFocusItem(body);
        }
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
        auto calculateLimit = [&]() {
            // Minimal arbitrary number of items requested from database. As ListView does not know how big elements are
            // before it gets them, requests twice size of current page with down limit of at least 4.
            auto minLimit = (2 * currentPageSize > 4 ? 2 * currentPageSize : 4);

            return (minLimit + startIndex <= elementsCount ? minLimit : elementsCount - startIndex);
        };

        if (direction == Direction::Bottom) {

            if (startIndex + currentPageSize >= elementsCount && listType == ListViewType::Continuous) {

                startIndex = 0;
            }
            else if (startIndex + currentPageSize >= elementsCount && listType == ListViewType::TopDown) {

                return true;
            }
            else {

                startIndex = startIndex <= elementsCount - currentPageSize
                                 ? startIndex + currentPageSize
                                 : elementsCount - (elementsCount - startIndex);
            }

            provider->requestRecords(startIndex, calculateLimit());
        }

        if (direction == Direction::Top) {

            if (startIndex == 0 && listType == ListViewType::Continuous) {

                startIndex = elementsCount - (elementsCount % currentPageSize);
            }
            else if (startIndex == 0 && listType == ListViewType::TopDown) {

                return true;
            }
            else {
                startIndex = startIndex - currentPageSize > 0 ? startIndex - currentPageSize : 0;
            }

            provider->requestRecords(startIndex, calculateLimit());
        }

        return true;
    }

} /* namespace gui */
