// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CalllogModel.hpp"

#include "InputEvent.hpp"
#include "ListView.hpp"
#include "data/CallLogInternals.hpp"
#include "data/CallLogSwitchData.hpp"
#include "widgets/CalllogItem.hpp"

#include <service-appmgr/Controller.hpp>
#include "application-call/data/CallSwitchData.hpp"
#include <module-db/queries/calllog/QueryCalllogGet.hpp>

using namespace calllog;

CalllogModel::CalllogModel(app::Application *app) : DatabaseModel(app), app::AsyncCallbackReceiver(app)
{}

unsigned int CalllogModel::requestRecordsCount()
{
    return recordsCount;
}

void CalllogModel::requestRecords(uint32_t offset, uint32_t limit)
{
    auto query = std::make_unique<db::query::CalllogGet>(limit, offset);
    auto task  = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::Calllog);
    task->setCallback([this](auto response) {
        auto result = dynamic_cast<db::query::CalllogGetResult *>(response);
        if (result == nullptr) {
            return false;
        }
        return onCalllogRetrieved(result->getRecords(), result->getTotalCount());
    });
    task->execute(application, this);
}

bool CalllogModel::onCalllogRetrieved(const std::vector<CalllogRecord> &records, unsigned int repoCount)
{
    if (recordsCount != repoCount) {
        recordsCount = repoCount;
        list->reSendLastRebuildRequest();
        return false;
    }
    return updateRecords(records);
}

bool CalllogModel::updateRecords(std::vector<CalllogRecord> records)
{
    DatabaseModel::updateRecords(std::move(records));
    list->onProviderDataUpdate();
    return true;
}

unsigned int CalllogModel::getMinimalItemSpaceRequired() const
{
    return gui::clItemStyle::h;
}

gui::ListItem *CalllogModel::getItem(gui::Order order)
{
    std::shared_ptr<CalllogRecord> call = getRecord(order);
    if (!call) {
        return nullptr;
    }

    auto contact = DBServiceAPI::MatchContactByPhoneNumber(application, call->phoneNumber);
    call->name   = contact ? contact->getFormattedName() : UTF8(call->phoneNumber.getFormatted());

    auto item = new gui::CalllogItem(this);

    auto callCallback = [this, item](gui::Item & /*item*/, const gui::InputEvent &event) {
        if (event.isShortRelease(gui::KeyCode::KEY_LF)) {
            return app::manager::Controller::sendAction(
                application,
                app::manager::actions::Call,
                std::make_unique<app::ExecuteCallData>(item->getCall().phoneNumber));
        }
        return false;
    };

    item->focusChangedCallback = [=](gui::Item &item) {
        if (item.focus == true) {
            auto &callLogItem = dynamic_cast<gui::CalllogItem &>(item);
            if (callLogItem.getCall().presentation == PresentationType::PR_UNKNOWN) {
                // disable call button since the number is unknown
                application->getCurrentWindow()->setBottomBarActive(gui::BottomBar::Side::LEFT, false);
                item.inputCallback = nullptr;
            }
            else {
                application->getCurrentWindow()->setBottomBarActive(gui::BottomBar::Side::LEFT, true);
                item.inputCallback = callCallback;
            }
        }
        return true;
    };

    item->setCall(call);
    item->activatedCallback = [=](gui::Item &item) {
        std::unique_ptr<gui::SwitchData> data = std::make_unique<calllog::CallLogSwitchData>(*call);
        application->switchWindow(calllog::settings::DetailsWindowStr, std::move(data));
        return true;
    };

    return item;
}
