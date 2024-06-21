// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WhatsNewFeaturesModel.hpp"
#include <ApplicationCommon.hpp>
#include <db/ServiceDB.hpp>
#include <db/WhatsNewMessages.hpp>
#include <service-db/Settings.hpp>
#include <service-db/agents/settings/SystemSettings.hpp>
#include <product/version.hpp>
#include <Utils.hpp>

namespace
{
    using namespace service::db::whatsnew;

    auto getVersionNumber(const std::string &version) -> std::optional<VersionNumber>
    {
        constexpr auto versionSize{3U};

        std::vector<std::string> strVector{utils::split(version, '.')};
        if (strVector.size() != versionSize) {
            return {};
        }

        std::vector<std::uint16_t> uintVector{};
        uintVector.reserve(versionSize);

        for (const auto &str : strVector) {
            if (!utils::is_number(str)) {
                return {};
            }
            uintVector.push_back(utils::getNumericValue<std::uint16_t>(str));
        }
        return VersionNumber{uintVector[0], uintVector[1], uintVector[2]};
    }

    auto sendDBRequest(sys::Service *serv, std::shared_ptr<sys::Message> &&msg) -> std::optional<messages::Response>
    {
        const auto ret = serv->bus.sendUnicastSync(std::move(msg), service::name::db, sys::BusProxy::defaultTimeout);
        if (ret.first == sys::ReturnCodes::Success) {
            if (auto resp = std::dynamic_pointer_cast<messages::Response>(ret.second)) {
                return *resp;
            }
        }
        return {};
    }
} // namespace

namespace app::whatsnew::models
{
    WhatsNewFeaturesModel::WhatsNewFeaturesModel(app::ApplicationCommon *app, settings::Settings *settings)
        : settings{settings}
    {
        const auto &lastVersion =
            this->settings->getValue(settings::SystemProperties::osCurrentVersion, settings::SettingsScope::Global);
        const auto &version = getVersionNumber(lastVersion);
        if (!version.has_value()) {
            LOG_ERROR("Failed to parse last version string!");
            return;
        }

        const auto &result = sendDBRequest(app, std::make_shared<messages::GetByVersion>(version.value()));
        if (result.has_value()) {
            for (const auto &record : result->records) {
                features.push_back(Feature{record.title, record.description, record.iconName});
            }
        }
    }

    auto WhatsNewFeaturesModel::getFeatures() -> std::vector<Feature>
    {
        return features;
    }

    auto WhatsNewFeaturesModel::setCurrentOsVersion() -> void
    {
        settings->setValue(settings::SystemProperties::osCurrentVersion, VERSION, settings::SettingsScope::Global);
    }
} // namespace app::whatsnew::models
