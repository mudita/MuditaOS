#pragma once


#include "DatabaseModel.hpp"
#include "Application.hpp"
#include "ListItemProvider.hpp"
#include "Interface/ThreadRecord.hpp"

#include <vector>

class BaseThreadRecordModel : public app::DatabaseModel<ThreadRecord>, public gui::ListItemProvider
{
  public:
    BaseThreadRecordModel() = delete;
    BaseThreadRecordModel(app::Application *app);

    unsigned int requestRecordsCount() override;
    bool updateRecords(std::unique_ptr<std::vector<ThreadRecord>> records,
                       const uint32_t offset,
                       const uint32_t limit,
                       uint32_t count) override;
    void requestRecords(const uint32_t offset, const uint32_t limit) override;

    app::Application *getApplication(void)
    {
        return application;
    };
};
