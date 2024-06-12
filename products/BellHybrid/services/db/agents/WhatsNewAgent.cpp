// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WhatsNewAgent.hpp"
#include "db/WhatsNewMessages.hpp"

#include <Service/Service.hpp>
#include <purefs/filesystem_paths.hpp>
#include <i18n/i18n.hpp>

namespace
{
    using namespace service::db::whatsNew;

    constexpr auto query = "SELECT %s, Icon FROM WhatsNew WHERE Major > %d OR (Major = %d AND Minor > %d) OR (Major = "
                           "%d AND Minor = %d AND Patch > %d)";

    std::vector<Record> getRecordsByVersion(Database *db, VersionNumber version)
    {
        const auto retQuery = db->query(query,
                                        utils::getDisplayLanguage().c_str(),
                                        version.major,
                                        version.major,
                                        version.minor,
                                        version.major,
                                        version.minor,
                                        version.patch);

        if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
            return {};
        }

        std::vector<Record> ret;
        ret.reserve(retQuery->getRowCount());

        do {
            ret.push_back({.description{(*retQuery)[0].getString()}, .iconName{(*retQuery)[1].getString()}});
        } while (retQuery->nextRow());

        return ret;
    }
} // namespace

namespace service::db::agents
{
    WhatsNew::WhatsNew(sys::Service *parentService, const std::string dbName)
        : DatabaseAgent{parentService}, dbName{dbName}, db{(purefs::dir::getDatabasesPath() / dbName).c_str()}
    {}

    void WhatsNew::registerMessages()
    {
        parentService->connect(whatsNew::messages::GetByVersion({}),
                               [this](const auto &req) { return handleGetRecordsByVersion(req); });
    }

    void WhatsNew::unRegisterMessages()
    {
        parentService->disconnect(typeid(whatsNew::messages::GetByVersion));
    }

    auto WhatsNew::getAgentName() -> const std::string
    {
        return dbName + "_agent";
    }

    sys::MessagePointer WhatsNew::handleGetRecordsByVersion(const sys::Message *req)
    {
        if (auto msg = dynamic_cast<const whatsNew::messages::GetByVersion *>(req)) {
            const auto records = getRecordsByVersion(&db, msg->version);
            return std::make_shared<whatsNew::messages::Response>(records);
        }
        return std::make_shared<sys::ResponseMessage>();
    }
} // namespace service::db::agents