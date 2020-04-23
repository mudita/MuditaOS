#include <algorithm>

#include "ListView.hpp"
#include <log/log.hpp>

namespace gui
{

    ListViewScroll::ListViewScroll(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
        : Rect{parent, x, y, w, h}
    {

        setRadius(style::listview::scroll_radius);
        setFilled(style::listview::scroll_fill);
        setFillColor(style::listview::scroll_color);
        activeItem = false;
    }

    void ListViewScroll::update(int startIndex, int listPageSize, int elementsCount)
    {
        if ((parent->widgetArea.w > 10) && (parent->widgetArea.h > 10)) {

            uint32_t pagesCount = 1;
            if (listPageSize) {
                pagesCount = (elementsCount % listPageSize == 0) ? elementsCount / listPageSize
                                                                 : elementsCount / listPageSize + 1;
                if (pagesCount == 0) {
                    return;
                }
            }
            uint32_t currentPage = startIndex / listPageSize;
            uint32_t pageHeight  = parent->widgetArea.h / pagesCount;

            setPosition(parent->widgetArea.w - style::listview::scroll_margin, pageHeight * currentPage);
            setSize(style::listview::scroll_w, pageHeight);
        }
        //             not enough space - disable scroll
        else {
            setVisible(false);
        }
    }

    ListView::ListView()
    {

        body = new VBox{this, 0, 0, 0, 0};

        scroll = new ListViewScroll(this,
                                    style::listview::scroll_x,
                                    style::listview::scroll_y,
                                    style::listview::scroll_w,
                                    style::listview::scroll_h);
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
                direction = Direction::Bot;
                return this->listPageEndReached();
            }
            else {
                return false;
            }
        };

        scroll = new ListViewScroll(this,
                                    style::listview::scroll_x,
                                    style::listview::scroll_y,
                                    style::listview::scroll_w,
                                    style::listview::scroll_h);
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

    void ListView::setProvider(ListItemProvider *prov)
    {
        provider = prov;
    }

    void ListView::clear()
    {
        clearItems();
        startIndex                  = 0;
        provider->listDataAvailable = false;
    }

    void ListView::clearItems()
    {
        auto temp = body->children.size();
        for (unsigned int i = 0; i < temp; i++) {

            auto rm = body->children.back();
            body->removeWidget(rm);
            delete (rm);
        }
    }

    void ListView::refresh()
    {

        if (provider->listDataAvailable) {

            if (provider == nullptr) {
                LOG_ERROR("ListViev Data provider not exist");
                return;
            }

            // remove old items
            clearItems();

            elementsCount = provider->getItemCount();
            addItemsOnPage();

            setFocus();
            scroll->update(startIndex, currentPageSize, elementsCount);

            // set flag that data has been used
            provider->listDataAvailable = false;
        }
    }

    void ListView::addItemsOnPage()
    {
        auto itemsOnPage = 0;

        while (true) {

            ListItem *item = provider->getItem(itemsOnPage);
            if (item == nullptr)
                break;

            item->setSize(item->minWidth, item->minHeight);
            body->addWidget(item);

            if (item->visible != true) {
                currentPageSize = itemsOnPage;
                break;
            }

            itemsOnPage++;

            listSpanItem = new Span(Axis::Y, style::listview::list_span);
            body->addWidget(listSpanItem);
        }
    }

    void ListView::setFocus()
    {
        if (direction == Direction::Top) {
            setFocusItem(body);
            body->setFocusOnLastElement();
        }
        if (direction == Direction::Bot) {
            setFocusItem(body);
        }
    };

    ListItem *ListView::getSelectedItem()
    {
        return nullptr;
    }

    std::list<DrawCommand *> ListView::buildDrawList()
    {

        // check if widget is visible
        if (visible == false) {
            return std::list<DrawCommand *>();
        }

        refresh();

        return gui::Rect::buildDrawList();
    }

    bool ListView::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        Rect::onDimensionChanged(oldDim, newDim);
        scroll->update(startIndex, currentPageSize, elementsCount);
        return true;
    }

    bool ListView::onInput(const InputEvent &inputEvent)

    {
        return body->onInput(inputEvent);
    }

    bool ListView::listPageEndReached()
    {

        auto calculateLimit = [&]() {
            auto minLimit = (2 * currentPageSize > 4 ? 2 * currentPageSize : 4);

            return (minLimit + startIndex <= elementsCount ? minLimit : elementsCount - startIndex);
        };

        if (direction == Direction::Bot) {

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
