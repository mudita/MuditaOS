// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WhatsNewAgent.hpp"
#include "db/WhatsNewMessages.hpp"

#include <i18n/i18n.hpp>
#include <Service/Service.hpp>
#include <purefs/filesystem_paths.hpp>

namespace
{
    using namespace service::db::whatsnew;

    inline std::string createTitleColumnName(const std::string &language)
    {
        constexpr auto titleSuffix = "_title";
        return language + titleSuffix;
    }

    inline std::string createDescriptionColumnName(const std::string &language)
    {
        constexpr auto descriptionSuffix = "_desc";
        return language + descriptionSuffix;
    }

    std::vector<Record> getRecordsByVersion(Database &db, const VersionNumber &version)
    {
        constexpr auto query = "SELECT %s, %s, Icon FROM whats_new "
                               "WHERE Major > %u OR "
                               "(Major = %u AND Minor > %u) OR "
                               "(Major = %u AND Minor = %u AND Patch > %u) "
                               "ORDER BY Major ASC, Minor ASC, Patch ASC;";

        const auto retQuery = db.query(query,
                                       createTitleColumnName(utils::getDisplayLanguage()).c_str(),
                                       createDescriptionColumnName(utils::getDisplayLanguage()).c_str(),
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
            ret.push_back(Record{(*retQuery)[0].getString(), (*retQuery)[1].getString(), (*retQuery)[2].getString()});
        } while (retQuery->nextRow());

        return ret;
    }
} // namespace

namespace service::db::agents
{
    WhatsNew::WhatsNew(sys::Service *parentService, const std::string &dbName)
        : DatabaseAgent{parentService}, dbName{dbName}, db{(purefs::dir::getDatabasesPath() / dbName).c_str()}
    {}

    void WhatsNew::registerMessages()
    {
        parentService->connect(whatsnew::messages::GetByVersion({}),
                               [this](const auto &req) { return handleGetRecordsByVersion(req); });
    }

    void WhatsNew::unRegisterMessages()
    {
        parentService->disconnect(typeid(whatsnew::messages::GetByVersion));
    }

    auto WhatsNew::getAgentName() -> const std::string
    {
        return dbName + "_agent";
    }

    sys::MessagePointer WhatsNew::handleGetRecordsByVersion(const sys::Message *req)
    {
        if (const auto msg = dynamic_cast<const whatsnew::messages::GetByVersion *>(req)) {
            const auto &records = getRecordsByVersion(db, msg->version);
            return std::make_shared<whatsnew::messages::Response>(records);
        }
        return std::make_shared<sys::ResponseMessage>();
    }
} // namespace service::db::agents
