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
        TestListItem();
        ~TestListItem() = default;
    };

    class TestListViewProvider : public ListItemProvider
    {
        unsigned int internalOffset = 0;
        unsigned int internalLimit  = 0;

      public:
        int testItemCount                  = 10;
        unsigned int testItemMinimalHeight = 100;

        TestListViewProvider() = default;

        virtual ~TestListViewProvider() = default;

        int getItemCount() const override;

        unsigned int getMinimalItemHeight() override;

        ListItem *getItem(Order order) override;

        void requestRecords(const uint32_t offset, const uint32_t limit) override;
    };
} // namespace gui
