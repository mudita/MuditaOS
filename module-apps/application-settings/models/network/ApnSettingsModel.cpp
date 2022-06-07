// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApnSettingsModel.hpp"
#include <service-cellular/ServiceCellular.hpp>

ApnSettingsModel::ApnSettingsModel(app::ApplicationCommon *application) : application{application}
{}

void ApnSettingsModel::requestAPNList()
{
    application->bus.sendUnicast(std::make_shared<cellular::GetAPNMessage>(), ServiceCellular::serviceName);
}

void ApnSettingsModel::saveAPN(const std::shared_ptr<packet_data::APN::Config> &apn)
{
    if (apn->contextId != packet_data::EmptyContextId) {
        CellularServiceAPI::SetAPN(application, *apn);
    }
    else {
        CellularServiceAPI::NewAPN(application, *apn);
    }
}

void ApnSettingsModel::removeAPN(const std::shared_ptr<packet_data::APN::Config> &apn)
{
    CellularServiceAPI::DeleteAPN(application, apn->contextId);
}

void ApnSettingsModel::setAsDefaultAPN(const std::shared_ptr<packet_data::APN::Config> &apn)
{
    apn->apnType = packet_data::APN::APNType::Default;
    application->bus.sendUnicast(std::make_shared<cellular::SetAPNMessage>(apn), ServiceCellular::serviceName);
}
