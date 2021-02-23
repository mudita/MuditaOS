// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CommandHandler.hpp"

#include <utility>
#include <service-bluetooth/ServiceBluetooth.hpp>
#include <service-bluetooth/SettingsHolder.hpp>

#include <Service/Service.hpp>

#include "Device.hpp"
#include "BtCommand.hpp"

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
                                   std::shared_ptr<bluetooth::Profile> currentProfile,
                                   std::shared_ptr<bluetooth::AbstractDriver> driver)
        : service{service}, settings{std::move(settings)}, currentProfile{std::move(currentProfile)}, driver{std::move(
                                                                                                          driver)}
    {}

    Error::Code CommandHandler::handle(Command command)
    {
        switch (command.getType()) {
        case bluetooth::Command::PowerOn:
            return Error::Success;
        case bluetooth::Command::StartScan:
            return scan();
        case bluetooth::Command::StopScan:
            return stopScan();
        case bluetooth::Command::StartPan:
            return startPan();
        case bluetooth::Command::Pair:
            return pair(command.getArgument().value());
        case bluetooth::Command::VisibilityOn:
            return setVisibility(true);
        case bluetooth::Command::VisibilityOff:
            return setVisibility(false);
        case bluetooth::Command::ConnectAudio:
            return establishAudioConnection();
        case bluetooth::Command::DisconnectAudio:
            return disconnectAudioConnection();
        case bluetooth::Command::PowerOff:
            return Error::Success;
        case bluetooth::Command::None:
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
        static_cast<ServiceBluetooth *>(service)->scanStartedCallback();
        // open new scan window
        return Error::Success;
    }

    Error::Code CommandHandler::stopScan()
    {
        driver->stopScan();
        static_cast<ServiceBluetooth *>(service)->scanStoppedCallback();
        return Error::Success;
    }

    Error::Code CommandHandler::startPan()
    {
        bluetooth::PAN::bnep_setup();
        if (const auto err = bluetooth::PAN::bnep_start(); err.err != bluetooth::Error::Success) {
            LOG_ERROR("PAN setup error: %s %" PRIu32, toString(err.err).c_str(), err.lib_code);
            return err.err;
        }
        return bluetooth::Error::Success;
    }

    Error::Code CommandHandler::setVisibility(bool visibility)
    {
        driver->setVisibility(visibility);
        settings->setValue(bluetooth::Settings::Visibility, static_cast<int>(visibility));
        return Error::Success;
    }

    Error::Code CommandHandler::establishAudioConnection()
    {
        currentProfile->setOwnerService(service);
        if (const auto status = currentProfile->init(); status != bluetooth::Error::Success) {
            return status;
        }

        currentProfile->connect();
        return Error::Success;
    }

    Error::Code CommandHandler::disconnectAudioConnection()
    {
        currentProfile->disconnect();
        return Error::Success;
    }
    Error::Code CommandHandler::pair(CommandArgument arg)
    {
        try {
            auto addrString = std::get<std::string>(arg);
            bd_addr_t addr;
            if (sscanf_bd_addr(addrString.c_str(), addr) != 0) {
                LOG_INFO("Pairing with %s", addrString.c_str());
                driver->pair(addr);
            }
            else {
                return Error::SystemError;
            }
        }
        catch (const std::bad_variant_access &) {
            return Error::SystemError;
        }
        return Error::Success;
    }
} // namespace bluetooth
