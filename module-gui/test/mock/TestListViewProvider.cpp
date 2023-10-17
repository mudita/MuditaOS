// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TestListViewProvider.hpp"
#include <log/log.hpp>

namespace gui
{
    TestListItem::TestListItem(unsigned int ID, bool deleteByList) : ID(ID)
    {
        setMinimumSize(testStyle::item_w, testStyle::item_h);
        setMaximumSize(testStyle::item_w, testStyle::item_h);

        this->deleteByList = deleteByList;
    };

    TestListViewProvider::TestListViewProvider()
    {
        for (unsigned int i = 0; i < testItemCount; i++) {
            internalData.push_back(new TestListItem(i, false));
        }
    }

    TestListViewProvider::~TestListViewProvider()
    {
        for (auto item : internalData) {
            delete item;
        }
        internalData.clear();
    }

    unsigned int TestListViewProvider::requestRecordsCount()
    {
        return testItemCount;
    }

    unsigned int TestListViewProvider::getMinimalItemSpaceRequired() const
    {

        return testItemMinimalHeight;
    }

    void TestListViewProvider::refreshList()
    {
        modelIndex = 0;
        list->onProviderDataUpdate();
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
            index = internalOffset + internalLimit - 1 - modelIndex;
        }
        if (order == gui::Order::Next) {
            index = internalOffset + modelIndex;
        }

        if (index < testItemCount) {

            if (order == gui::Order::Previous && index < internalOffset) {
                return nullptr;
            }

            if (dataSource == gui::TestListViewDataSource::External) {

                auto testItem = new TestListItem(index, true);
                testItem->setMargins(testItemMargins);

                if (notEqualItems) {
                    testItem->setMinimumSize(testStyle::item_w, testStyle::item_h + testStyle::item_h * index);
                }

                modelIndex++;

                return testItem;
            }
            else {

                if (notEqualItems) {
                    internalData[index]->setMinimumSize(testStyle::item_w,
                                                        testStyle::item_h + testStyle::item_h * index);
                }

                internalData[index]->setVisible(true);
                internalData[index]->setFocus(false);
                internalData[index]->clearNavigationItem(gui::NavigationDirection::UP);
                internalData[index]->clearNavigationItem(gui::NavigationDirection::DOWN);
                internalData[index]->setMargins(testItemMargins);

                modelIndex++;

                return internalData[index];
            }
        }
        return nullptr;
    }
}; // namespace gui
