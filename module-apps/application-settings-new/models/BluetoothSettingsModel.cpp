// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BluetoothSettingsModel.hpp"

#include <Constants.hpp>
#include <service-bluetooth/messages/Status.hpp>
#include <service-bluetooth/messages/SetStatus.hpp>

BluetoothSettingsModel::BluetoothSettingsModel(app::Application *application) : application{application}
{}

void BluetoothSettingsModel::requestStatus()
{
    application->bus.sendUnicast(std::make_shared<::message::bluetooth::RequestStatus>(), service::name::bluetooth);
}
void BluetoothSettingsModel::setStatus(bool desiredBluetoothState, bool desiredVisibility)
{
    BluetoothStatus status{.state = desiredBluetoothState ? BluetoothStatus::State::On : BluetoothStatus::State::Off,
                           .visibility = desiredVisibility};
    message::bluetooth::SetStatus setStatus(status);
    application->bus.sendUnicast(std::make_shared<::message::bluetooth::SetStatus>(std::move(setStatus)),
                                 service::name::bluetooth);
}
void BluetoothSettingsModel::stopScan()
{
    application->bus.sendUnicast(std::make_shared<BluetoothMessage>(BluetoothMessage::Request::StopScan),
                                 service::name::bluetooth);
}

void BluetoothSettingsModel::setAddrForAudioProfiles(std::string addr)
{
    application->bus.sendUnicast(std::make_shared<BluetoothAddrMessage>(std::move(addr)), service::name::bluetooth);
}
