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
        modelIndex     = 0;
        internalOffset = offset;
        internalLimit  = limit;
        list->onProviderDataUpdate();
    }

    gui::ListItem *TestListViewProvider::getItem(gui::Order order)
    {

        unsigned int index = 0;
        if (order == gui::Order::Previous) {
            index = internalLimit - modelIndex - 1;
        }
        if (order == gui::Order::Next) {
            index = internalOffset + modelIndex;
        }

        if (index < testItemCount) {

            if (order == gui::Order::Previous && index < internalOffset) {
                return nullptr;
            }

            LOG_DEBUG("tests: %d", index);

            auto testItem = new TestListItem();
            testItem->ID  = index;

            modelIndex++;

            return testItem;
        }

        return nullptr;
    }

}; // namespace gui
