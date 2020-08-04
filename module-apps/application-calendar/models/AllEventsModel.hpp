#pragma once

#include "Application.hpp"
#include <ListItemProvider.hpp>

class AllEventsModel : public gui::ListItemProvider
{
    static const int tempItemCount = 10;
    app::Application *application  = nullptr;

  public:
    AllEventsModel(app::Application *app);
    virtual ~AllEventsModel() override = default;

    void requestRecords(const uint32_t offset, const uint32_t limit) override;

    // virtual methods for ListViewProvider
    [[nodiscard]] unsigned int getMinimalItemHeight() const override;
    gui::ListItem *getItem(gui::Order order) override;
    [[nodiscard]] unsigned int requestRecordsCount() override
    {
        return tempItemCount;
    };
};
