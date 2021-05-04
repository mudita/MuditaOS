// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <map>

namespace db
{
    class NotificationMessage;
    class QueryResult;
} // namespace db

namespace app
{
    class Application;

    class DBNotificationsBaseHandler
    {
      public:
        enum class Type
        {
            notReadSMS,
            notReadCall
        };

        [[nodiscard]] virtual bool hasNotification(Type type) const = 0;
        virtual ~DBNotificationsBaseHandler()                       = default;
    };

    class DBNotificationsHandler : public DBNotificationsBaseHandler
    {
        Application *ownerApp = nullptr;
        std::map<Type, int> notifications;

        void setNotification(Type type, int value);

      public:
        explicit DBNotificationsHandler(Application *ownerApp);

        void initHandler();
        bool handle(db::NotificationMessage *msg);
        bool handle(db::QueryResult *msg);

        [[nodiscard]] bool hasNotification(Type type) const override;
    };

} // namespace app
