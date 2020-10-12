#pragma once

#include "DatabaseModel.hpp"
#include "Application.hpp"
#include "ListItemProvider.hpp"
#include "Interface/SMSRecord.hpp"
#include <application-messages/widgets/SMSInputWidget.hpp>

class SMSThreadModel : public app::DatabaseModel<SMSRecord>, public gui::ListItemProvider
{
  public:
    unsigned int smsThreadID      = 0;
    gui::SMSInputWidget *smsInput = nullptr;
    std::unique_ptr<utils::PhoneNumber::View> number;

    SMSThreadModel(app::Application *app);
    ~SMSThreadModel() override;

    void addReturnNumber();
    void handleDraftMessage();
    void resetInputWidget();

    auto handleQueryResponse(db::QueryResult *) -> bool;

    unsigned int requestRecordsCount() override;
    bool updateRecords(std::vector<SMSRecord> records) override;
    void requestRecords(uint32_t offset, uint32_t limit) override;
    unsigned int getMinimalItemHeight() const override;
    gui::ListItem *getItem(gui::Order order) override;
};
