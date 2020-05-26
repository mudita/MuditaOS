#pragma once

#include "Interface/SMSTemplateRecord.hpp"

#include <DatabaseModel.hpp>
#include <Application.hpp>
#include <ListItemProvider.hpp>

class SMSTemplateModel : public app::DatabaseModel<SMSTemplateRecord>, public gui::ListItemProvider
{
  public:
    SMSTemplateModel() = delete;
    SMSTemplateModel(app::Application *app);
    virtual ~SMSTemplateModel() = default;

    void requestRecordsCount() override;
    bool updateRecords(std::unique_ptr<std::vector<SMSTemplateRecord>> records,
                       const uint32_t offset,
                       const uint32_t limit,
                       uint32_t count) override;
    void requestRecords(const uint32_t offset, const uint32_t limit) override;

    gui::ListItem *getItem(gui::Order order) override;

    int getItemCount() const override
    {
        return recordsCount;
    };
};
