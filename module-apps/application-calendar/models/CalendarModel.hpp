#pragma once

#include "Application.hpp"
#include "ListItemProvider.hpp"

class CalendarModel : public gui::ListItemProvider
{
  public:
    CalendarModel(app::Application *app);
    virtual ~CalendarModel();

    /// TODO: Connect with database
    // virtual methods
    // void requestRecordsCount() override;
    // bool updateRecords(std::unique_ptr<std::vector< ?db_records? >> records,
    //                const uint32_t offset,
    //                const uint32_t limit,
    //                uint32_t count) override;
    void requestRecords(const uint32_t offset, const uint32_t limit) override;

    // virtual methods for ListViewProvider
    unsigned int getMinimalItemHeight() override;
    gui::ListItem *getItem(gui::Order order) override;
    int getItemCount() const override
    {
        /// TODO: Count items from database 
        return 7;
    };
};