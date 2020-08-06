#pragma once

#include "DatabaseModel.hpp"
#include "Application.hpp"
#include "ListItemProvider.hpp"
#include "Common/Query.hpp"
#include "Interface/ThreadRecord.hpp"

#include <vector>

class BaseThreadRecordModel : public app::DatabaseModel<ThreadRecord>,
                              public gui::ListItemProvider,
                              public db::QueryListener
{
  public:
    BaseThreadRecordModel() = delete;
    BaseThreadRecordModel(app::Application *app);

    unsigned int requestRecordsCount() override;
    bool updateRecords(std::unique_ptr<std::vector<ThreadRecord>> records) override;
    void requestRecords(const uint32_t offset, const uint32_t limit) override;

    app::Application *getApplication(void)
    {
        return application;
    };
};
