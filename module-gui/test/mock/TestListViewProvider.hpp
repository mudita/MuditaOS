// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <module-gui/gui/widgets/ListItem.hpp>
#include <module-gui/gui/widgets/ListView.hpp>
#include <module-gui/gui/widgets/ListItemProvider.hpp>

namespace testStyle
{

    const inline uint32_t list_x = 0;
    const inline uint32_t list_y = 0;
    const inline uint32_t list_w = 200;
    const inline uint32_t list_h = 600;

    const inline uint32_t item_w = 200;
    const inline uint32_t item_h = 100;
} // namespace testStyle

namespace gui
{

    class TestListItem : public ListItem
    {

      public:
        unsigned int ID = 0;

        TestListItem(unsigned int ID, bool deleteByList);
        ~TestListItem() = default;
    };

    enum class TestListViewDataSource
    {
        Internal,
        External
    };

    class TestListViewProvider : public ListItemProvider
    {
        int modelIndex              = 0;
        unsigned int internalOffset = 0;
        unsigned int internalLimit  = 0;
        std::vector<ListItem *> internalData;

      public:
        unsigned int testItemCount         = 10;
        unsigned int testItemMinimalHeight = 100;
        bool notEqualItems                 = false;
        TestListViewDataSource dataSource  = TestListViewDataSource::External;
        Margins testItemMargins            = Margins();

        TestListViewProvider();

        virtual ~TestListViewProvider();

        [[nodiscard]] unsigned int requestRecordsCount() override;

        unsigned int getMinimalItemSpaceRequired() const override;

        ListItem *getItem(Order order) override;

        void requestRecords(const uint32_t offset, const uint32_t limit) override;

        void refreshList();
    };
} // namespace gui
