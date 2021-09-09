// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "SMSInputWidget.hpp"

#include <DatabaseModel.hpp>
#include <Application.hpp>
#include <ListItemProvider.hpp>
#include <Interface/SMSRecord.hpp>

class SMSThreadModel : public app::DatabaseModel<SMSRecord>,
                       public gui::ListItemProvider,
                       public app::AsyncCallbackReceiver
{
  public:
    unsigned int smsThreadID      = 0;
    gui::SMSInputWidget *smsInput = nullptr;
    std::unique_ptr<utils::PhoneNumber::View> number;
    unsigned int numberID = DB_ID_NONE;

    SMSThreadModel(app::ApplicationCommon *app);
    ~SMSThreadModel() override;

    void addReturnNumber();
    void handleDraftMessage();
    void resetInputWidget();

    auto handleQueryResponse(db::QueryResult *) -> bool;

    unsigned int requestRecordsCount() override;
    bool updateRecords(std::vector<SMSRecord> records) override;
    void requestRecords(uint32_t offset, uint32_t limit) override;
    unsigned int getMinimalItemSpaceRequired() const override;
    gui::ListItem *getItem(gui::Order order) override;
};
