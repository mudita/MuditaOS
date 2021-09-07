// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "GAP.hpp"

#include <log.hpp>
#include <service-bluetooth/BluetoothMessage.hpp>
#include <service-bluetooth/messages/ResponseVisibleDevices.hpp>
#include <service-bluetooth/messages/Unpair.hpp>
#include <service-bluetooth/messages/Passkey.hpp>
#include <application-settings/ApplicationSettings.hpp>
extern "C"
{
#include "btstack.h"
#include "hci.h"
};
namespace bluetooth
{
    std::string GAP::currentlyProcessedDeviceAddr;
    sys::Service *GAP::ownerService = nullptr;
    std::vector<Devicei> GAP::devices;
    btstack_packet_callback_registration_t GAP::cb_handler;
    ScanState GAP::state;

    auto GAP::registerScan() -> Error
    {
        LOG_INFO("GAP register scan!");
        /// -> this have to be called prior to power on!
        hci_set_inquiry_mode(INQUIRY_MODE_RSSI_AND_EIR);
        cb_handler.callback = &packetHandler;
        hci_add_event_handler(&cb_handler);
        return Error();
    }

    auto GAP::scan() -> Error
    {
        if (hci_get_state() == HCI_STATE_WORKING) {
            devices.clear();
            if (auto ret = startScan(); ret != 0) {
                LOG_ERROR("Start scan error!: 0x%X", ret);
                return Error(Error::LibraryError, ret);
            }
        }
        else {
            return Error(Error::NotReady);
        }
        return Error();
    }

    void GAP::stopScan()
    {
        gap_inquiry_force_stop();
        LOG_INFO("Scan stopped!");
    }

    void GAP::setVisibility(bool visibility)
    {
        gap_discoverable_control(static_cast<std::uint8_t>(visibility));
        LOG_INFO("Visibility: %s", visibility ? "true" : "false");
    }

    auto GAP::pair(std::uint8_t *addr, std::uint8_t protectionLevel) -> bool
    {
        if (hci_get_state() == HCI_STATE_WORKING) {
            currentlyProcessedDeviceAddr = bd_addr_to_str(addr);
            return gap_dedicated_bonding(addr, protectionLevel) == 0;
        }
        return false;
    }

    auto getDeviceIndexForAddress(const std::vector<Devicei> &devs, bd_addr_t addr) -> int
    {
        auto result = std::find_if(std::begin(devs), std::end(devs), [addr](const Devicei &device) {
            return bd_addr_cmp(addr, device.address) == 0;
        });

        if (result == std::end(devs)) {
            return -1;
        }

        return std::distance(std::begin(devs), result);
    }

    void GAP::sendDevices()
    {
        auto msg = std::make_shared<message::bluetooth::ResponseVisibleDevices>(devices);
        ownerService->bus.sendUnicast(msg, "ApplicationSettings");
        ownerService->bus.sendUnicast(std::move(msg), app::name_settings);
    }

    auto GAP::startScan() -> int
    {
        LOG_INFO("Starting inquiry scan..");
        return gap_inquiry_start(inquiryIntervalSeconds);
    }

    auto GAP::remoteNameToFetch() -> bool
    {
        auto result = std::find_if(std::begin(devices), std::end(devices), [](Devicei &device) {
            return device.state == REMOTE_NAME_REQUEST;
        });

        return result != std::end(devices);
    }

    void GAP::fetchRemoteName()
    {
        for (auto &device : bluetooth::GAP::devices) {
            if (device.state == REMOTE_NAME_REQUEST) {
                device.state = REMOTE_NAME_INQUIRED;
                LOG_INFO("Get remote name...");
                gap_remote_name_request(device.address, device.pageScanRepetitionMode, device.clockOffset | 0x8000);
                return;
            }
        }
    }

    void GAP::continueScanning()
    {
        if (remoteNameToFetch()) {
            fetchRemoteName();
            return;
        }
        startScan();
    }
    auto GAP::updateDeviceName(std::uint8_t *packet, bd_addr_t &addr) -> bool
    {
        reverse_bd_addr(&packet[3], addr);
        auto index = getDeviceIndexForAddress(devices, addr);
        if (index >= 0) {
            if (packet[2] == 0) {
                devices[index].state = REMOTE_NAME_FETCHED;
                devices[index].name  = std::string{reinterpret_cast<const char *>(&packet[9])};
                return true;
            }
            else {
                LOG_INFO("Failed to get name: page timeout");
            }
        }
        return false;
    }

    void GAP::addNewDevice(std::uint8_t *packet, bd_addr_t &addr)
    {
        Devicei device;
        device.setAddress(&addr);
        device.pageScanRepetitionMode = gap_event_inquiry_result_get_page_scan_repetition_mode(packet);
        device.clockOffset            = gap_event_inquiry_result_get_clock_offset(packet);
        device.classOfDevice          = gap_event_inquiry_result_get_class_of_device(packet);
        LOG_INFO("Device found ");
        LOG_INFO("with COD: 0x%06x, ", static_cast<unsigned int>(device.classOfDevice));
        LOG_INFO("pageScan %d, ", device.pageScanRepetitionMode);
        LOG_INFO("clock offset 0x%04x", device.clockOffset);

        if (gap_event_inquiry_result_get_rssi_available(packet) != 0u) {
            LOG_INFO(", rssi %d dBm", static_cast<int8_t>(gap_event_inquiry_result_get_rssi(packet)));
        }
        if (gap_event_inquiry_result_get_name_available(packet) != 0u) {
            auto name   = gap_event_inquiry_result_get_name(packet);
            device.name = std::string{reinterpret_cast<const char *>(name)};
            device.state = REMOTE_NAME_FETCHED;
        }
        else {
            device.state = REMOTE_NAME_REQUEST;
            device.name  = std::string{};
        }

        devices.emplace_back(std::move(device));
    }

    void GAP::processInquiryResult(std::uint8_t *packet, bd_addr_t &addr)
    {
        gap_event_inquiry_result_get_bd_addr(packet, addr);
        auto index = getDeviceIndexForAddress(devices, addr);
        if (index >= 0) {
            return; // already in our list
        }
        uint32_t classOfDevice = gap_event_inquiry_result_get_class_of_device(packet);
        LOG_INFO("Device CoD: %" PRIx32 "", classOfDevice);
        ///> Device has to support services: AUDIO for HFP and HSP profiles, and RENDERING for SNK of A2DP profile
        if (!(classOfDevice & TYPE_OF_SERVICE::REMOTE_SUPPORTED_SERVICES)) {
            LOG_INFO("Ignoring device with incompatible services: %s, ",
                     getListOfSupportedServicesInString(classOfDevice).c_str());
            return;
        }
        addNewDevice(packet, addr);
        sendDevices();
    }
    void GAP::processInquiryComplete()
    {
        for (auto &device : devices) {
            // retry remote name request
            if (device.state == REMOTE_NAME_INQUIRED) {
                device.state = REMOTE_NAME_REQUEST;
            }
        }
        continueScanning();
    }
    void GAP::processNameRequestComplete(std::uint8_t *packet, bd_addr_t &addr)
    {
        if (updateDeviceName(packet, addr)) {
            sendDevices();
        }
        continueScanning();
    }
    void GAP::processDedicatedBondingCompleted(std::uint8_t *packet, bd_addr_t &addr)
    {
        auto result = packet[2];

        auto msg = std::make_shared<BluetoothPairResultMessage>(currentlyProcessedDeviceAddr, result == 0u);
        currentlyProcessedDeviceAddr.clear();
        ownerService->bus.sendUnicast(std::move(msg), app::name_settings);
    }
    /* @text In ACTIVE, the following events are processed:
     *  - GAP Inquiry result event: BTstack provides a unified inquiry result that contain
     *    Class of Device (CoD), page scan mode, clock offset. RSSI and name (from EIR) are optional.
     *  - Inquiry complete event: the remote name is requested for devices without a fetched
     *    name. The state of a remote name can be one of the following:
     *    REMOTE_NAME_REQUEST, REMOTE_NAME_INQUIRED, or REMOTE_NAME_FETCHED.
     *  - Remote name request complete event: the remote name is stored in the table and the
     *    state is updated to REMOTE_NAME_FETCHED. The query of remote names is continued.
     */
    void GAP::activeStateHandler(std::uint8_t eventType, std::uint8_t *packet, bd_addr_t &addr)
    {
        switch (eventType) {

        case GAP_EVENT_INQUIRY_RESULT:
            processInquiryResult(packet, addr);
            break;

        case GAP_EVENT_INQUIRY_COMPLETE:
            processInquiryComplete();
            break;

        case HCI_EVENT_REMOTE_NAME_REQUEST_COMPLETE:
            processNameRequestComplete(packet, addr);
            break;
        case GAP_EVENT_DEDICATED_BONDING_COMPLETED:
            processDedicatedBondingCompleted(packet, addr);
            break;
        default:
            break;
        }
    }
    void GAP::initStateHandler(std::uint8_t eventType, std::uint8_t *packet)
    {
        if (eventType == BTSTACK_EVENT_STATE) {
            if (btstack_event_state_get_state(packet) == HCI_STATE_WORKING) {
                state = ScanState::active;
            }
        }
    }
    void GAP::packetHandler(std::uint8_t packet_type, std::uint16_t channel, std::uint8_t *packet, std::uint16_t size)
    {
        bd_addr_t addr;

        if (packet_type != HCI_EVENT_PACKET) {
            return;
        }
        if (hci_event_packet_get_type(packet) == HCI_EVENT_PIN_CODE_REQUEST) {
            bd_addr_t address;
            LOG_DEBUG("PIN code request!");
            hci_event_pin_code_request_get_bd_addr(packet, address);
            auto msg = std::make_shared<::message::bluetooth::RequestPasskey>();
            ownerService->bus.sendUnicast(std::move(msg), app::name_settings);
        }
        const auto eventType = hci_event_packet_get_type(packet);
        switch (state) {
        case ScanState::init:
            initStateHandler(eventType, packet);
            break;
        case ScanState::active:
            activeStateHandler(eventType, packet, addr);
            break;
        default:
            break;
        }
    }
    GAP::GAP(sys::Service *owner)
    {
        ownerService = owner;
        state        = ScanState::init;
    }
    auto GAP::getDevicesList() -> const std::vector<Devicei> &
    {
        return devices;
    }
    auto GAP::unpair(uint8_t *addr) -> bool
    {
        LOG_INFO("Unpairing device");
        gap_drop_link_key_for_bd_addr(addr);

        LOG_INFO("Device unpaired");
        std::string unpairedDevAddr{bd_addr_to_str(addr)};
        ownerService->bus.sendUnicast(
            std::make_shared<message::bluetooth::UnpairResult>(std::move(unpairedDevAddr), true), app::name_settings);
        return true;
    }
    void GAP::respondPinCode(const std::string &pin)
    {
        bd_addr_t address{};
        sscanf_bd_addr(currentlyProcessedDeviceAddr.c_str(), address);
        gap_pin_code_response(address, pin.c_str());
    }
} // namespace bluetooth
