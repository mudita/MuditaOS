#pragma once

#include "Application.hpp"
#include <ListItemProvider.hpp>
#include <DatabaseModel.hpp>
#include <module-db/Interface/EventsRecord.hpp>

class AllEventsModel : public app::DatabaseModel<EventsRecord>, public gui::ListItemProvider
{
    app::Application *application  = nullptr;

  public:
    AllEventsModel(app::Application *app);
    virtual ~AllEventsModel() override = default;

    void setRecordsCount(const uint32_t count);
    void requestRecords(const uint32_t offset, const uint32_t limit) override;
    bool updateRecords(std::unique_ptr<std::vector<EventsRecord>> records) override;

    // virtual methods for ListViewProvider
    [[nodiscard]] unsigned int getMinimalItemHeight() const override;
    gui::ListItem *getItem(gui::Order order) override;
    [[nodiscard]] unsigned int requestRecordsCount() override;
};
