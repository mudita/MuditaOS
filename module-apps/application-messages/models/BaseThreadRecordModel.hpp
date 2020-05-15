#pragma once

#include <vector>

#include "DatabaseModel.hpp"
#include "Application.hpp"
#include "ListItemProvider.hpp"
#include "Interface/ThreadRecord.hpp"

class BaseThreadRecordModel : public app::DatabaseModel<ThreadRecord>, public gui::ListItemProvider
{
  public:
    BaseThreadRecordModel() = delete;
    BaseThreadRecordModel(app::Application *app);

    void requestRecordsCount() override;
    bool updateRecords(std::unique_ptr<std::vector<ThreadRecord>> records,
                       const uint32_t offset,
                       const uint32_t limit,
                       uint32_t count) override;
    void requestRecords(const uint32_t offset, const uint32_t limit) override;

    int getItemCount() const override
    {
        return recordsCount;
    };

    app::Application *getApplication(void)
    {
        return application;
    };
};
