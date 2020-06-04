#include "TestListViewProvider.hpp"
#include <log/log.hpp>

namespace gui
{

    TestListItem::TestListItem()
    {

        setMinimumSize(testStyle::item_w, testStyle::item_h);
        setMaximumSize(testStyle::item_w, testStyle::item_h);
    };

    int TestListViewProvider::getItemCount() const
    {

        return testItemCount;
    }

    unsigned int TestListViewProvider::getMinimalItemHeight()
    {

        return testItemMinimalHeight;
    }

    void TestListViewProvider::requestRecords(const uint32_t offset, const uint32_t limit)
    {
        internalOffset = offset;
        internalLimit  = limit;
        list->onProviderDataUpdate();
    }

    gui::ListItem *TestListViewProvider::getItem(gui::Order order)
    {

        auto index = 0;
        if (order == gui::Order::Previous) {
            index = internalOffset + internalLimit - 1;
        }
        if (order == gui::Order::Next) {
            index = internalOffset;
        }

        LOG_DEBUG("tests: %d", index);

        auto testItem = new TestListItem();

        if (order == gui::Order::Previous) {
            internalOffset--;
        }
        if (order == gui::Order::Next) {
            internalOffset++;
        }

        return testItem;
    }

}; // namespace gui
