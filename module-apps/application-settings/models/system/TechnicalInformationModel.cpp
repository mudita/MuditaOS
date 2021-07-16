// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TechnicalInformationModel.hpp"

#include <application-settings/widgets/system/TechnicalInformationItem.hpp>
#include <application-settings/widgets/SettingsStyle.hpp>
#include <ListView.hpp>
#include <source/version.hpp>
#include <i18n/i18n.hpp>

TechnicalInformationModel::TechnicalInformationModel(std::unique_ptr<AbstractFactoryData> &&factoryData)
    : factoryData{std::move(factoryData)}
{
    createData();
}

auto TechnicalInformationModel::requestRecordsCount() -> unsigned int
{
    return internalData.size();
}

auto TechnicalInformationModel::getMinimalItemSpaceRequired() const -> unsigned int
{
    return style::technical_info::height + 2 * style::margins::big;
}

void TechnicalInformationModel::requestRecords(const uint32_t offset, const uint32_t limit)
{
    setupModel(offset, limit);
    list->onProviderDataUpdate();
}

auto TechnicalInformationModel::getItem(gui::Order order) -> gui::ListItem *
{
    return getRecord(order);
}

void TechnicalInformationModel::createData()
{
    internalData.push_back(
        new gui::TechnicalInformationItem(utils::translate("app_settings_tech_info_model"), factoryData->getModel()));

    internalData.push_back(new gui::TechnicalInformationItem(utils::translate("app_settings_tech_info_serial_number"),
                                                             factoryData->getSerial()));

    internalData.push_back(
        new gui::TechnicalInformationItem(utils::translate("app_settings_tech_info_os_version"), std::string(VERSION)));

    internalData.push_back(new gui::TechnicalInformationItem(utils::translate("app_settings_tech_info_imei"), imei));

#if DEVELOPER_SETTINGS_OPTIONS == 1
    internalData.push_back(new gui::TechnicalInformationItem(utils::translate("app_settings_tech_info_battery"),
                                                             factoryData->getBatteryRev()));

    internalData.push_back(new gui::TechnicalInformationItem(utils::translate("app_settings_tech_info_pcb_mb"),
                                                             factoryData->getPcb("mb")));

    internalData.push_back(new gui::TechnicalInformationItem(utils::translate("app_settings_tech_info_pcb_lm"),
                                                             factoryData->getPcb("lm")));

    internalData.push_back(new gui::TechnicalInformationItem(utils::translate("app_settings_tech_info_pcb_am"),
                                                             factoryData->getPcb("am")));

    internalData.push_back(new gui::TechnicalInformationItem(utils::translate("app_settings_tech_info_pcb_um"),
                                                             factoryData->getPcb("um")));
#endif // DEVELOPER_SETTINGS_OPTIONS

    for (auto item : internalData) {
        item->deleteByList = false;
    }
}
