// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AboutYourBellModel.hpp"

#include <BellSettingsStyle.hpp>
#include <widgets/AboutYourBellListItem.hpp>
#include <ProductConfig.hpp>

#include <ListView.hpp>
#include <product/version.hpp>
#include <purefs/filesystem_paths.hpp>
#include <sys/statvfs.h>

namespace
{
    constexpr auto factoryDataSerialPath = "factory_data/serial";
    constexpr auto factoryDataVersionPath = "factory_data/device_version";
}

namespace app::bell_settings
{
    AboutYourBellModel::AboutYourBellModel(app::ApplicationCommon *app)
    {
        settings.init(service::ServiceProxy{app->weak_from_this()});
        createData();
    }

    auto AboutYourBellModel::requestRecordsCount() -> unsigned int
    {
        return internalData.size();
    }

    auto AboutYourBellModel::getMinimalItemSpaceRequired() const -> unsigned int
    {
        return gui::bell_settings_style::about_your_bell_window::height;
    }

    void AboutYourBellModel::requestRecords(std::uint32_t offset, std::uint32_t limit)
    {
        setupModel(offset, limit);
        list->onProviderDataUpdate();
    }

    auto AboutYourBellModel::getItem(gui::Order order) -> gui::ListItem *
    {
        return getRecord(order);
    }

    void AboutYourBellModel::createData()
    {
        const auto productSerialNumber = settings.getValue(factoryDataSerialPath, settings::SettingsScope::Global);
        const auto productVersion      = settings.getValue(factoryDataVersionPath, settings::SettingsScope::Global);
        const auto productName =
            utils::translate("app_bell_settings_about_product") + std::string(" ") + productVersion;

        internalData.push_back(
            new gui::AboutYourBellListItem(productName,
                                           utils::translate("app_bell_settings_about_version"),
                                           gui::AboutYourBellListItem::TokenMap({{"$VERSION", std::string(VERSION)}})));

        internalData.push_back(new gui::AboutYourBellListItem(utils::translate("app_bell_settings_about_serial_number"),
                                                              productSerialNumber));

#if CONFIG_SHOW_MEMORY_INFO == 1
        struct statvfs stat
        {};
        const auto result = statvfs(purefs::dir::getUserDiskPath().c_str(), &stat);
        if (result < 0) {
            LOG_ERROR("Getting memory info failed, error: %d!", result);
            return;
        }

        constexpr auto bytesInMebibyte = 1024LLU * 1024LLU;
        const auto totalMiB            = (stat.f_frsize * stat.f_blocks) / bytesInMebibyte;
        const auto usedMiB             = totalMiB - (stat.f_bfree * stat.f_bsize) / bytesInMebibyte;
        internalData.push_back(new gui::AboutYourBellListItem(
            utils::translate("app_bell_settings_about_storage_title"),
            utils::translate("app_bell_settings_about_storage_text"),
            gui::AboutYourBellListItem::TokenMap(
                {{"$USED_MEMORY", std::to_string(usedMiB)}, {"$TOTAL_MEMORY", std::to_string(totalMiB)}})));
#endif

        internalData.push_back(new gui::AboutYourBellListItem(utils::translate("app_bell_settings_about_info_title"),
                                                              utils::translate("app_bell_settings_about_info_text")));

        for (auto item : internalData) {
            item->deleteByList = false;
        }
    }

    void AboutYourBellModel::clearData()
    {
        list->reset();
        eraseInternalData();
    }
} // namespace app::bell_settings
