// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApnSettingsModel.hpp"
#include <service-cellular/PacketDataCellularMessage.hpp>
#include <service-cellular/ServiceCellular.hpp>

ApnSettingsModel::ApnSettingsModel(app::Application *application) : application{application}
{}

void ApnSettingsModel::requestAPNList()
{
    sys::Bus::SendUnicast(std::make_shared<CellularGetAPNMessage>(), ServiceCellular::serviceName, application);
}

void ApnSettingsModel::saveAPN(std::shared_ptr<packet_data::APN::Config> apn)
{
    sys::Bus::SendUnicast(std::make_shared<CellularSetAPNMessage>(apn), ServiceCellular::serviceName, application);
}

void ApnSettingsModel::removeAPN(std::shared_ptr<packet_data::APN::Config> apn)
{}

void ApnSettingsModel::setAsDefaultAPN(std::shared_ptr<packet_data::APN::Config> apn)
{
    apn->apnType = packet_data::APN::APNType::Default;
    sys::Bus::SendUnicast(std::make_shared<CellularSetAPNMessage>(apn), ServiceCellular::serviceName, application);
}
