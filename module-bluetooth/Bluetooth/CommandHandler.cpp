// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CommandHandler.hpp"
#include <service-bluetooth/ServiceBluetooth.hpp>

#include <utility>
#include <service-bluetooth/SettingsHolder.hpp>
#include <Service/Service.hpp>

#include "Device.hpp"

#include <service-desktop/Constants.hpp>
#include <service-bluetooth/messages/ResponseVisibleDevices.hpp>
#include "GAP/GAP.hpp"

extern "C"
{
#include <module-bluetooth/lib/btstack/src/btstack_util.h>
}

namespace bluetooth
{
    namespace
    {
        [[nodiscard]] auto toString(bluetooth::Error::Code code) -> std::string
        {
            return utils::enumToString(code);
        }
    } // namespace

    CommandHandler::CommandHandler(sys::Service *service,
                                   std::shared_ptr<bluetooth::SettingsHolder> settings,
                                   std::shared_ptr<bluetooth::ProfileManager> profileManager,
                                   std::shared_ptr<bluetooth::AbstractDriver> driver)
        : service{service}, settings{std::move(settings)}, profileManager{std::move(profileManager)}, driver{std::move(
                                                                                                          driver)}
    {
        this->driver->registerPowerOnCallback([profilePtr = this->profileManager]() { profilePtr->init(); });
    }

    Error::Code CommandHandler::handle(Command &command)
    {
        switch (command.getType()) {
        case bluetooth::Command::PowerOn:
            return Error::Success;
        case bluetooth::Command::StartScan:
            return scan();
        case bluetooth::Command::getDevicesAvailable:
            return availableDevices();
        case bluetooth::Command::StopScan:
            return stopScan();
        case bluetooth::Command::Pair:
            return pair(command.getData());
        case bluetooth::Command::Unpair:
            return unpair(command.getData());
        case bluetooth::Command::VisibilityOn:
            return setVisibility(true);
        case bluetooth::Command::VisibilityOff:
            return setVisibility(false);
        case bluetooth::Command::ConnectAudio:
            return establishAudioConnection(command.getData());
        case bluetooth::Command::DisconnectAudio:
            return disconnectAudioConnection();
        case bluetooth::Command::PowerOff:
            profileManager->deInit();
            return Error::Success;
        case bluetooth::Command::None:
            return Error::Success;
        case Command::StartRinging:
            return profileManager->startRinging();
        case Command::StopRinging:
            return profileManager->stopRinging();
        case Command::StartRouting:
            return profileManager->initializeCall();
        case Command::CallAnswered:
            return profileManager->callAnswered();
        case Command::CallTerminated:
            return profileManager->terminateCall();
        case Command::CallStarted:
            return profileManager->callStarted(command.getData());
        case Command::IncomingCallNumber:
            return profileManager->setIncomingCallNumber(command.getData());
        case Command::SignalStrengthData:
            return profileManager->setSignalStrengthData(command.getData());
        case Command::OperatorNameData:
            return profileManager->setOperatorNameData(command.getData());
        case Command::BatteryLevelData:
            return profileManager->setBatteryLevelData(command.getData());
        case Command::NetworkStatusData:
            return profileManager->setNetworkStatusData(command.getData());
        case Command::StartStream:
            profileManager->start();
            return Error::Success;
        case Command::StopStream:
            profileManager->stop();
            return Error::Success;
        }
        return Error::LibraryError;
    }

    Error::Code CommandHandler::scan()
    {

        if (const auto ret = driver->scan(); ret.err != bluetooth::Error::Success) {
            LOG_ERROR("Cant start scan!: %s %" PRIu32 "", toString(ret.err).c_str(), ret.lib_code);
            return ret.err;
        }

        LOG_INFO("Scan started!");
        // open new scan window
        return Error::Success;
    }

    Error::Code CommandHandler::stopScan()
    {
        LOG_INFO("Stopping scan!");
        driver->stopScan();
        return Error::Success;
    }

    Error::Code CommandHandler::setVisibility(bool visibility)
    {
        driver->setVisibility(visibility);
        settings->setValue(bluetooth::Settings::Visibility, static_cast<int>(visibility));
        return Error::Success;
    }

    Error::Code CommandHandler::establishAudioConnection(const DataVariant &data)
    {
        auto device = std::get<Devicei>(data);
        LOG_INFO("Connecting audio");
        profileManager->connect(device);
        return Error::Success;
    }

    Error::Code CommandHandler::disconnectAudioConnection()
    {
        LOG_INFO("Disconnecting audio");
        profileManager->disconnect();
        return Error::Success;
    }
    Error::Code CommandHandler::pair(const DataVariant &data)
    {
        auto device = std::get<Devicei>(data);
        LOG_INFO("Pairing...");
        const auto errorCode = driver->pair(device) ? Error::Success : Error::LibraryError;
        LOG_INFO("Pairing result: %s", magic_enum::enum_name(errorCode).data());
        return errorCode;
    }
    Error::Code CommandHandler::unpair(const DataVariant &data)
    {
        auto device = std::get<Devicei>(data);
        LOG_INFO("Unpairing...");
        const auto errorCode = driver->unpair(device) ? Error::Success : Error::LibraryError;
        LOG_INFO("Unpairing result: %s", magic_enum::enum_name(errorCode).data());
        return errorCode;
    }

    Error::Code CommandHandler::availableDevices()
    {
        auto msg = std::make_shared<message::bluetooth::ResponseVisibleDevices>(bluetooth::GAP::getDevicesList());
        static_cast<ServiceBluetooth *>(service)->bus.sendUnicast(std::move(msg), service::name::service_desktop);

        return Error::Success;
    }

} // namespace bluetooth
