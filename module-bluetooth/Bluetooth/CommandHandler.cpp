// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CommandHandler.hpp"
#include <service-bluetooth/ServiceBluetooth.hpp>

#include <utility>
#include <service-bluetooth/SettingsHolder.hpp>
#include <Service/Service.hpp>

#include "Device.hpp"

#include <service-desktop/ServiceDesktopName.hpp>
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
        [[nodiscard]] auto toString(bluetooth::Result::Code code) -> std::string
        {
            return utils::enumToString(code);
        }
    } // namespace

    CommandHandler::CommandHandler(sys::Service *service,
                                   std::shared_ptr<bluetooth::SettingsHolder> settings,
                                   std::shared_ptr<bluetooth::BaseProfileManager> profileManager,
                                   std::shared_ptr<bluetooth::AbstractDriver> driver)
        : service{service}, settings{std::move(settings)}, profileManager{std::move(profileManager)}, driver{std::move(
                                                                                                          driver)}
    {
        this->driver->registerPowerOnCallback([profilePtr = this->profileManager]() { profilePtr->init(); });
    }

    Result::Code CommandHandler::scan()
    {

        if (const auto ret = driver->scan(); ret.result != bluetooth::Result::Code::Success) {
            LOG_ERROR("Can't start scan!: %s %d", toString(ret.result).c_str(), ret.libraryResult);
            return ret.result;
        }

        LOG_INFO("Scan started!");
        // open new scan window
        return Result::Code::Success;
    }

    Result::Code CommandHandler::stopScan()
    {
        LOG_INFO("Stopping scan!");
        driver->stopScan();
        return Result::Code::Success;
    }

    Result::Code CommandHandler::setVisibility(bool visibility)
    {
        driver->setVisibility(visibility);
        settings->setValue(bluetooth::Settings::Visibility, static_cast<int>(visibility));
        return Result::Code::Success;
    }

    Result::Code CommandHandler::connect(const DataVariant &data)
    {
        auto device = std::get<Devicei>(data);
        LOG_INFO("Connecting device");
        profileManager->connect(device);
        return Result::Code::Success;
    }

    Result::Code CommandHandler::disconnect()
    {
        LOG_INFO("Disconnecting device");
        profileManager->disconnect();
        return Result::Code::Success;
    }

    Result::Code CommandHandler::pair(const DataVariant &data)
    {
        auto device = std::get<Devicei>(data);
        LOG_INFO("Pairing...");
        auto errorCode = Result::Code::Success;
        driver->pair(device);
        LOG_INFO("Pairing result: %s", magic_enum::enum_name(errorCode).data());
        return errorCode;
    }

    Result::Code CommandHandler::unpair(const DataVariant &data)
    {
        auto device = std::get<Devicei>(data);
        LOG_INFO("Unpairing...");
        const auto errorCode = Result::Code::Success;
        driver->unpair(device);
        LOG_INFO("Unpairing result: %s", magic_enum::enum_name(errorCode).data());
        return errorCode;
    }

    Result::Code CommandHandler::availableDevices()
    {
        auto msg = std::make_shared<message::bluetooth::ResponseVisibleDevices>(bluetooth::GAP::getDevicesList());
        static_cast<ServiceBluetooth *>(service)->bus.sendUnicast(std::move(msg), service::name::service_desktop);
        return Result::Code::Success;
    }
} // namespace bluetooth
