// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AboutYourBellModel.hpp"

#include <BellSettingsStyle.hpp>
#include <widgets/advanced/AboutYourBellListItem.hpp>
#include <ProductConfig.hpp>

#include <ListView.hpp>
#include <product/version.hpp>
#include <purefs/filesystem_paths.hpp>
#include <sys/statvfs.h>

namespace app::bell_settings
{
    AboutYourBellModel::AboutYourBellModel(app::ApplicationCommon *app) : application(app)
    {
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

    void AboutYourBellModel::requestRecords(const uint32_t offset, const uint32_t limit)
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
        internalData.push_back(
            new gui::AboutYourBellListItem(utils::translate("app_bell_settings_advanced_about_product"),
                                           utils::translate("app_bell_settings_advanced_about_version"),
                                           gui::AboutYourBellListItem::TokenMap({{"$VERSION", std::string(VERSION)}})));

#if CONFIG_SHOW_MEMORY_INFO == 1
        struct statvfs stat;
        const auto result = statvfs(purefs::dir::getRootDiskPath().c_str(), &stat);
        if (result < 0) {
            LOG_ERROR("Getting memory info failed! result = %d", result);
            return;
        }

        const auto totalMB = (stat.f_frsize * stat.f_blocks) / 1024LLU / 1024LLU;
        const auto usedMB  = totalMB - (stat.f_bfree * stat.f_bsize) / 1024LLU / 1024LLU;
        internalData.push_back(new gui::AboutYourBellListItem(
            utils::translate("app_bell_settings_advanced_about_storage_title"),
            utils::translate("app_bell_settings_advanced_about_storage_text"),
            gui::AboutYourBellListItem::TokenMap(
                {{"$USED_MEMORY", std::to_string(usedMB)}, {"$TOTAL_MEMORY", std::to_string(totalMB)}})));
#endif

        internalData.push_back(
            new gui::AboutYourBellListItem(utils::translate("app_bell_settings_advanced_about_info_title"),
                                           utils::translate("app_bell_settings_advanced_about_info_text")));

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
