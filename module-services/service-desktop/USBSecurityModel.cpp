// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-desktop/USBSecurityModel.hpp"

#include "Service/Service.hpp"
#include "service-desktop/DesktopMessages.hpp"
#include "service-desktop/WorkerDesktop.hpp"

#include <service-db/service-db/Settings.hpp>
#include <module-services/service-db/agents/settings/SystemSettings.hpp>

namespace sdesktop
{
    USBSecurityModel::USBSecurityModel(sys::Service *ownerSrv, settings::Settings *srvSettings)
    {
        settings = srvSettings;

        settings->registerValueChange(
            settings::SystemProperties::usbDevices,
            [this](const std::string &value) { bounded = parseDevices(value); },
            ::settings::SettingsScope::Global);

        settings->registerValueChange(
            ::settings::SystemProperties::lockPassHash,
            [this](const std::string &value) {
                // test
                lockPassHash = utils::getNumericValue<unsigned int>(value);
            },
            ::settings::SettingsScope::Global);
    }

    auto USBSecurityModel::isBound(DeviceID id) const -> bool
    {
        return bounded.find(id) != bounded.end();
    }

    auto USBSecurityModel::addDevice(DeviceID id, Passcode passcode) -> bool
    {
        if (endpointSecurity == EndpointSecurity::Block && !checkPasscode(passcode)) {
            return false;
        }

        bounded.insert(id);
        settings->setValue(settings::SystemProperties::usbDevices, dumpDevices(bounded));

        return true;
    }

    bool USBSecurityModel::checkPasscode(const Passcode &passcode)
    {
        static std::hash<unsigned int> hashEngine;
        auto hash = hashEngine(passcode);
        return hash == lockPassHash;
    }

    auto USBSecurityModel::isSecurityEnabled() const -> bool
    {
        return utils::getNumericValue<bool>(
            settings->getValue(settings::SystemProperties::usbSecurity, settings::SettingsScope::Global));
    }

    void USBSecurityModel::enableEndpointSecurity(bool securityEnabled)
    {
        settings->setValue(settings::SystemProperties::usbSecurity,
                           utils::to_string(securityEnabled),
                           settings::SettingsScope::Global);
    }

    bool USBSecurityModel::processHandshake(const sdesktop::usb::USBHandshake *handshake)
    {
        auto id  = handshake->getId();
        auto pwd = handshake->getPasscode();

        LOG_DEBUG("Process handshake: id=%s; pwd=%d;", id.c_str(), pwd);

        if (isBound(id)) {
            LOG_DEBUG("Bounded device, handshake successfull");
            return true;
        }

        if (addDevice(id, pwd)) {
            LOG_DEBUG("Bounding device successfull");
            return true;
        }

        LOG_DEBUG("Handshake failed");
        return false;
    }

    void USBSecurityModel::setEndpointSecurity(EndpointSecurity security)
    {
        endpointSecurity = security;
    }

    auto USBSecurityModel::getEndpointSecurity() const -> EndpointSecurity
    {
        return endpointSecurity;
    }

    std::set<DeviceID> USBSecurityModel::parseDevices(const std::string &value) const
    {
        std::istringstream iss{value};
        return {std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>()};
    }
    std::string USBSecurityModel::dumpDevices(const std::set<DeviceID> &devices) const
    {
        std::stringstream ss;
        std::copy(devices.begin(), devices.end(), std::ostream_iterator<DeviceID>(ss, " "));
        return ss.str();
    }
} // namespace sdesktop