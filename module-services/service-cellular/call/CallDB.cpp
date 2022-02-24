// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CallDB.hpp"
#include "service-appmgr/data/CallActionsParams.hpp"
#include <queries/notifications/QueryNotificationsIncrement.hpp>
#include <service-appmgr/Controller.hpp>

CallDB::CallDB(sys::Service &s) : owner(s)
{}

void CallDB::incrementNotAnsweredCallsNotification(const utils::PhoneNumber::View &number)
{
    DBServiceAPI::GetQuery(
        &owner,
        db::Interface::Name::Notifications,
        std::make_unique<db::query::notifications::Increment>(NotificationsRecord::Key::Calls, number));
}
