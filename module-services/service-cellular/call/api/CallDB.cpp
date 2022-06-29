// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CallDB.hpp"
#include "service-appmgr/data/CallActionsParams.hpp"
#include <queries/notifications/QueryNotificationsIncrement.hpp>
#include <service-appmgr/Controller.hpp>
#include "call/CellularCall.hpp"

CallDB::CallDB(sys::Service *s) : owner(s)
{}

void CallDB::incrementNotAnsweredCallsNotification(const utils::PhoneNumber::View &number)
{
    DBServiceAPI::GetQuery(
        owner,
        db::Interface::Name::Notifications,
        std::make_unique<db::query::notifications::Increment>(NotificationsRecord::Key::Calls, number));
}

void CallDB::startCall(CalllogRecord &rec)
{
    if (rec.ID == DB_ID_NONE) {
        auto call = DBServiceAPI::CalllogAdd(owner, rec);
        rec       = call;
    }
    else {
        auto ret = DBServiceAPI::CalllogUpdate(owner, rec);
        if (not ret) {
            throw std::runtime_error("CalllogUpdate failed");
        }
    }
    if (rec.ID == DB_ID_NONE) {
        throw std::runtime_error("CalllogAdd failed");
    }
}

void CallDB::endCall(CalllogRecord &rec)
{
    auto addCallToDB = [&](){
        if (rec.ID != DB_ID_NONE) {
            return false;
        }

        auto call = DBServiceAPI::CalllogAdd(owner, rec);
        rec = call;
        if (rec.ID == DB_ID_NONE) {
            throw std::runtime_error("CalllogAdd failed");
        }

        return true;
    };

    if (rec.type == CallType::CT_NONE && (addCallToDB() || DBServiceAPI::CalllogUpdate(owner, rec))) {
        DBServiceAPI::GetQuery(
            owner,
            db::Interface::Name::Notifications,
            std::make_unique<db::query::notifications::Increment>(NotificationsRecord::Key::Calls, rec.phoneNumber));
        return;
    }
}

bool CallDB::isNumberInFavourites(const utils::PhoneNumber::View &number)
{
    return DBServiceAPI::IsContactInFavourites(owner, number);
}
