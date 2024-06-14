// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WhatsNewModel.hpp"
#include <ApplicationCommon.hpp>
#include <db/ServiceDB.hpp>
#include <db/WhatsNewMessages.hpp>
#include <service-db/Settings.hpp>
#include <service-db/agents/settings/SystemSettings.hpp>
#include <product/version.hpp>
#include <Utils.hpp>

namespace
{
    using namespace service::db::whatsNew;
    constexpr auto versionSize{3U};

    std::optional<VersionNumber> getVersionNumber(std::string version)
    {
        std::vector<std::string> strVector{utils::split(version, '.')};
        if (strVector.size() != versionSize) {
            return std::nullopt;
        }

        std::vector<std::uint16_t> uintVector{};
        uintVector.reserve(versionSize);

        for (auto &str : strVector) {
            if (!utils::is_number(str)) {
                return std::nullopt;
            }
            uintVector.push_back(utils::getNumericValue<std::uint16_t>(str));
        }
        return VersionNumber{.major{uintVector[0]}, .minor{uintVector[1]}, .patch{uintVector[2]}};
    }

    std::optional<messages::Response> sendDBRequest(sys::Service *serv, std::shared_ptr<sys::Message> &&msg)
    {
        const auto ret = serv->bus.sendUnicastSync(std::move(msg), service::name::db, sys::BusProxy::defaultTimeout);
        if (ret.first == sys::ReturnCodes::Success) {
            if (auto resp = std::dynamic_pointer_cast<messages::Response>(ret.second)) {
                return *resp;
            }
        }
        return std::nullopt;
    }
} // namespace

namespace app::whatsNew::models
{
    WhatsNewModel::WhatsNewModel(app::ApplicationCommon *app, settings::Settings *settings)
        : app{app}, settings{settings}
    {
        const auto lastVersion =
            settings->getValue(settings::SystemProperties::osCurrentVersion, settings::SettingsScope::Global);
        const auto version = getVersionNumber(lastVersion);
        if (!version.has_value()) {
            return;
        }
        const auto result = sendDBRequest(app, std::make_shared<messages::GetByVersion>(version.value()));
        if (result.has_value()) {
            for (auto &record : result->records) {
                LOG_ERROR("*** changes: %s iconName: %s ***", record.description.c_str(), record.iconName.c_str());
                features.push_back({.description = record.description, .iconName = record.iconName});
            }
        }
    }

} // namespace app::whatsNew::models
