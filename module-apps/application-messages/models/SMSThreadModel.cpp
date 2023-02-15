// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationMessages.hpp"
#include "MessagesStyle.hpp"
#include "SMSOutputWidget.hpp"
#include "SMSThreadModel.hpp"

#include <ListView.hpp>
#include <module-db/queries/messages/sms/QuerySMSGetForList.hpp>
#include <queries/messages/threads/QueryThreadGetByID.hpp>
#include <queries/notifications/QueryNotificationsDecrement.hpp>

namespace
{
    constexpr auto threadQueryTimeoutMs = 1000;
}

SMSThreadModel::SMSThreadModel(app::ApplicationCommon *app) : DatabaseModel(app), app::AsyncCallbackReceiver{app}
{
    smsInput = new gui::SMSInputWidget(application);
}

SMSThreadModel::~SMSThreadModel()
{
    delete smsInput;
}

unsigned int SMSThreadModel::getMinimalItemSpaceRequired() const
{
    return style::messages::smsOutput::default_h;
}

gui::ListItem *SMSThreadModel::getItem(gui::Order order)
{
    std::shared_ptr<SMSRecord> sms = getRecord(order);

    if (sms == nullptr) {
        return nullptr;
    }

    // Small hack to trick current model logic -> adding empty row into query result for Input Widget
    if (sms->type == SMSType::INPUT) {
        addReturnNumber();
        return smsInput;
    }

    return new gui::SMSOutputWidget(application, sms);
}

unsigned int SMSThreadModel::requestRecordsCount()
{
    return recordsCount;
}

void SMSThreadModel::requestRecords(uint32_t offset, uint32_t limit)
{
    auto query = std::make_unique<db::query::SMSGetForList>(smsThreadID, offset, limit, numberID);
    auto task  = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::SMS);
    task->setCallback([this](auto response) { return handleQueryResponse(response); });
    task->execute(application, this);
}

bool SMSThreadModel::updateRecords(std::vector<SMSRecord> records)
{
    DatabaseModel::updateRecords(std::move(records));
    list->onProviderDataUpdate();
    return true;
}

auto SMSThreadModel::handleQueryResponse(db::QueryResult *queryResult) -> bool
{
    auto msgResponse = dynamic_cast<db::query::SMSGetForListResult *>(queryResult);
    assert(msgResponse != nullptr);

    auto records_data = msgResponse->getResults();

    // If list record count has changed we need to rebuild list.
    if (recordsCount != (msgResponse->getCount() + 1)) {
        // Additional one element for SMSInputWidget.
        recordsCount = msgResponse->getCount() + 1;
        list->reSendLastRebuildRequest();

        markCurrentThreadAsRead();
        return false;
    }

    resetInputWidget();

    if (msgResponse->getDraft().isValid() && smsInput->inputText->isEmpty()) {
        smsInput->draft = msgResponse->getDraft().type == SMSType::DRAFT
                              ? std::optional<SMSRecord>{msgResponse->getDraft()}
                              : std::nullopt;
        smsInput->displayDraftMessage();
    }

    number = std::make_unique<utils::PhoneNumber::View>(msgResponse->getNumber());

    return this->updateRecords(std::move(records_data));
}

void SMSThreadModel::addReturnNumber()
{
    if (number != nullptr) {
        smsInput->number = std::move(number);
    }

    smsInput->activatedCallback = [this]([[maybe_unused]] gui::Item &item) {
        auto app = dynamic_cast<app::ApplicationMessages *>(application);
        assert(app != nullptr);
        assert(smsInput->number != nullptr);
        if (!app->handleSendSmsFromThread(*smsInput->number, smsInput->inputText->getText())) {
            LOG_ERROR("handleSendSmsFromThread failed");
        }
        smsInput->clearDraftMessage();
        return true;
    };
}

void SMSThreadModel::handleDraftMessage()
{
    smsInput->handleDraftMessage();
}

void SMSThreadModel::resetInputWidget()
{
    smsInput->setFocus(false);
    smsInput->setVisible(true);
    smsInput->clearNavigationItem(gui::NavigationDirection::UP);
    smsInput->clearNavigationItem(gui::NavigationDirection::DOWN);
}
void SMSThreadModel::markCurrentThreadAsRead()
{
    const auto [code, msg] = DBServiceAPI::GetQueryWithReply(application,
                                                             db::Interface::Name::SMSThread,
                                                             std::make_unique<db::query::ThreadGetByID>(smsThreadID),
                                                             threadQueryTimeoutMs);

    if (code == sys::ReturnCodes::Success && msg != nullptr) {
        const auto queryResponse = dynamic_cast<db::QueryResponse *>(msg.get());
        assert(queryResponse != nullptr);

        const auto resultResponse = queryResponse->getResult();
        const auto result         = dynamic_cast<db::query::ThreadGetByIDResult *>(resultResponse.get());
        assert(result != nullptr);

        if (const auto unreadMsgCount = result->getRecord()->unreadMsgCount; unreadMsgCount > 0) {
            DBServiceAPI::GetQuery(
                application,
                db::Interface::Name::SMSThread,
                std::make_unique<db::query::MarkAsRead>(smsThreadID, db::query::MarkAsRead::Read::True));

            DBServiceAPI::GetQuery(
                application,
                db::Interface::Name::Notifications,
                std::make_unique<db::query::notifications::Decrement>(NotificationsRecord::Key::Sms, unreadMsgCount));
        }
    }
}
