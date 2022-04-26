// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "GAP.hpp"
#include "Devices.hpp"
#include "GAP/used_events.hpp"

#include <Bluetooth/error_bluetooth.hpp>
#include <service-bluetooth/BluetoothMessage.hpp>
#include <service-bluetooth/messages/ResponseVisibleDevices.hpp>
#include <service-bluetooth/messages/Unpair.hpp>
#include <service-bluetooth/messages/Authenticate.hpp>
#include <service-bluetooth/Constants.hpp>
#include <log/log.hpp>
#include <memory>
extern "C"
{
#include "btstack.h"
#include "hci.h"
};
namespace bluetooth
{
    sys::Service *GAP::ownerService = nullptr;
    btstack_packet_callback_registration_t GAP::cb_handler;
    stack::state GAP::state;

    namespace gap
    {
        enum class state
        {
            scan_off = 0,
            scan_on,
        } static state;
    }

    static gap::Devices &devices()
    {
        static std::unique_ptr<gap::Devices> dev;
        if (not dev) {
            dev = std::make_unique<gap::Devices>();
        }
        return *dev;
    };

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
            if (gap::state == gap::state::scan_on) {
                stopScan();
            }
            devices().clear();
            if (auto ret = startScan(); ret != 0) {
                LOG_ERROR("Start scan error!: 0x%02X - %s", ret, error_cstr(ret));
                return Error(Error::LibraryError, ret);
            }
            gap::state = gap::state::scan_on;
        }
        else {
            return Error(Error::NotReady);
        }
        return Error();
    }

    void GAP::stopScan()
    {
        gap::state = gap::state::scan_off;
        gap_inquiry_force_stop();
        LOG_INFO("Scan stopped!");
    }

    void GAP::setVisibility(bool visibility)
    {
        gap_discoverable_control(static_cast<std::uint8_t>(visibility));
        LOG_INFO("Visibility: %s", visibility ? "true" : "false");
    }

    auto GAP::pair(Devicei device, std::uint8_t protectionLevel) -> bool
    {
        if (hci_get_state() == HCI_STATE_WORKING) {
            auto it = devices().find(device.address);
            if (it == devices().end()) {
                LOG_ERROR("device not found: %s", device.address_str());
                return false;
            }
            return gap_dedicated_bonding(device.address, protectionLevel) == 0;
        }
        return false;
    }

    void GAP::sendDevices()
    {
        auto msg = std::make_shared<message::bluetooth::ResponseVisibleDevices>(devices().getList());
        ownerService->bus.sendMulticast(std::move(msg), sys::BusChannel::BluetoothNotifications);
    }

    auto GAP::startScan() -> int
    {
        LOG_INFO("Starting inquiry scan..");
        return gap_inquiry_start(inquiryIntervalSeconds);
    }

    void GAP::continueScanning()
    {
        if (const auto &it = devices().find(REMOTE_NAME_REQUEST); it != devices().end()) {
            LOG_INFO("Get remote name for %s", it->name.data());
            it->state = REMOTE_NAME_INQUIRED;
            gap_remote_name_request(it->address, it->pageScanRepetitionMode, it->clockOffset | 0x8000);
            return;
        }
        if (gap::state == gap::state::scan_on) {
            startScan();
        }
    }

    auto GAP::updateDeviceName(std::uint8_t *packet, bd_addr_t &addr) -> bool
    {
        reverse_bd_addr(&packet[3], addr);
        if (auto it = devices().find(addr); it != devices().end()) {
            it->state = packet[2] ? REMOTE_NAME_FAILURE : REMOTE_NAME_FETCHED;
            if (it->state != REMOTE_NAME_FAILURE) {
                strcpy(it->name.data(), reinterpret_cast<const char *>(&packet[9]));
            }
            return it->state == REMOTE_NAME_FETCHED;
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
        LOG_INFO("with address: %s, ", device.address_str());
        LOG_INFO("with COD: 0x%06x, ", static_cast<unsigned int>(device.classOfDevice));
        LOG_INFO("pageScan %d, ", device.pageScanRepetitionMode);
        LOG_INFO("clock offset 0x%04x", device.clockOffset);

        if (gap_event_inquiry_result_get_rssi_available(packet) != 0u) {
            LOG_INFO(", rssi %d dBm", static_cast<int8_t>(gap_event_inquiry_result_get_rssi(packet)));
        }
        if (gap_event_inquiry_result_get_name_available(packet) != 0u) {
            if (const auto nameLen = gap_event_inquiry_result_get_name_len(packet); nameLen > Device::NameBufferSize) {
                LOG_ERROR("Can't add new device - name length is too large.");
                return;
            }
            auto name = gap_event_inquiry_result_get_name(packet);
            strcpy(device.name.data(), reinterpret_cast<const char *>(name));
            device.state = REMOTE_NAME_FETCHED;
        }
        else {
            bd_addr_t devAddr;
            gap_event_inquiry_result_get_bd_addr(packet, devAddr);
            device.state = REMOTE_NAME_REQUEST;
            strcpy(device.name.data(), bd_addr_to_str(devAddr));
        }

        devices().put(std::move(device));
    }

    void GAP::processInquiryResult(std::uint8_t *packet)
    {
        bd_addr_t addr;
        gap_event_inquiry_result_get_bd_addr(packet, addr);
        auto it = devices().find(addr);
        if (it != devices().end()) {
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
        devices().for_each([](Devicei &d) {
            if (d.state == REMOTE_NAME_INQUIRED) {
                d.state = REMOTE_NAME_REQUEST;
            }
        });
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
        auto it     = devices().find(addr);
        auto msg = std::make_shared<BluetoothPairResultMessage>(it != devices().end() ? *it : Devicei(), result == 0u);
        ownerService->bus.sendUnicast(std::move(msg), service::name::bluetooth);
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
    void GAP::activeStateHandler(std::uint8_t eventType, std::uint8_t *packet, std::uint16_t size)
    {
        if (not(eventType == HCI_EVENT_TRANSPORT_PACKET_SENT || eventType == HCI_EVENT_COMMAND_STATUS ||
                eventType == HCI_EVENT_INQUIRY_COMPLETE || eventType == HCI_EVENT_COMMAND_COMPLETE ||
                eventType == HCI_EVENT_NUMBER_OF_COMPLETED_PACKETS)) {
            LOG_DEBUG("event: 0x%02X - %s - size: %" PRIu16, eventType, evt_cstr(eventType), size);
        }
        switch (eventType) {
        case HCI_EVENT_TRANSPORT_PACKET_SENT:
            break;
        case HCI_EVENT_EXTENDED_INQUIRY_RESPONSE:
            break;
        case GAP_EVENT_PAIRING_STARTED:
            break;

        case HCI_EVENT_READ_REMOTE_SUPPORTED_FEATURES_COMPLETE:
            break;

        case HCI_EVENT_USER_CONFIRMATION_REQUEST: {
            bd_addr_t addr;
            hci_event_user_confirmation_request_get_bd_addr(packet, addr);
            auto conn = hci_connection_for_bd_addr_and_type(addr, BD_ADDR_TYPE_ACL);
            if (conn == nullptr) {
                break;
            }

            auto it   = devices().find(addr);
            if (it == devices().end()) {
                gap_remote_name_request(addr, PAGE_SCAN_MODE_STANDARD, 0);
                it = devices().put(addr);
            }
            it->isPairingSSP = true;

            if (conn->io_cap_response_io == SSP_IO_CAPABILITY_NO_INPUT_NO_OUTPUT) {
                gap_ssp_confirmation_response(addr);
                break;
            }

            auto code = hci_event_user_passkey_notification_get_numeric_value(packet);
            auto msg = std::make_shared<::message::bluetooth::RequestAuthenticate>(
                *it,
                bluetooth::AuthenticateType::PairCancel,
                (code != 0) ? static_cast<std::optional<unsigned long>>(code) : std::nullopt);
            ownerService->bus.sendMulticast(std::move(msg), sys::BusChannel::BluetoothNotifications);
        } break;

        case HCI_EVENT_PIN_CODE_REQUEST: {
            bd_addr_t addr;
            hci_event_pin_code_request_get_bd_addr(packet, addr);
            auto it = devices().find(addr);
            if (it == devices().end()) {
                gap_remote_name_request(addr, PAGE_SCAN_MODE_STANDARD, 0);
                it = devices().put(addr);
            }
            it->isPairingSSP = false;

            auto msg =
                std::make_shared<::message::bluetooth::RequestAuthenticate>(*it, bluetooth::AuthenticateType::Passkey);
            ownerService->bus.sendMulticast(std::move(msg), sys::BusChannel::BluetoothNotifications);
        } break;

        case HCI_EVENT_READ_REMOTE_EXTENDED_FEATURES_COMPLETE: {
            uint16_t handle        = little_endian_read_16(packet, 3);
            hci_connection_t *conn = hci_connection_for_handle(handle);
            auto yes               = gap_ssp_supported_on_both_sides(conn->con_handle);
            auto it                = devices().find(conn->address);
            if (it == devices().end()) {
                return;
            }
            it->isPairingSSP = yes;
        } break;

        case GAP_EVENT_INQUIRY_RESULT:
            processInquiryResult(packet);
            break;

        case GAP_EVENT_INQUIRY_COMPLETE:
            processInquiryComplete();
            break;
        case HCI_EVENT_USER_PASSKEY_REQUEST: {
            bd_addr_t addr;
            hci_event_user_passkey_request_get_bd_addr(packet, addr);
            auto it = devices().find(addr);
            if (it == devices().end()) {
                gap_remote_name_request(addr, PAGE_SCAN_MODE_STANDARD, 0);
                it = devices().put(addr);
            }
            it->isPairingSSP = true;

            ownerService->bus.sendMulticast(
                std::make_shared<::message::bluetooth::RequestAuthenticate>(*it, bluetooth::AuthenticateType::Passkey),
                sys::BusChannel::BluetoothNotifications);
        } break;
        case HCI_EVENT_REMOTE_NAME_REQUEST_COMPLETE: {
            bd_addr_t addr;
            hci_event_remote_name_request_complete_get_bd_addr(packet, addr);
            processNameRequestComplete(packet, addr);
        } break;
        case GAP_EVENT_DEDICATED_BONDING_COMPLETED:
            bd_addr_t addr;
            reverse_bd_addr(&packet[3], addr);
            processDedicatedBondingCompleted(packet, addr);
            break;
        case HCI_EVENT_SIMPLE_PAIRING_COMPLETE: {
            bd_addr_t addr;
            hci_event_simple_pairing_complete_get_bd_addr(packet, addr);
            processSimplePairingCompleted(packet, addr);
        } break;
        case GAP_EVENT_PAIRING_COMPLETE:
            LOG_DEBUG("status: 0x%02X", packet[10]);
            break;
        default:
            break;
        }
    }

    void GAP::initStateHandler(std::uint8_t eventType, std::uint8_t *packet)
    {
        if (eventType == BTSTACK_EVENT_STATE) {
            if (btstack_event_state_get_state(packet) == HCI_STATE_WORKING) {
                state = stack::state::working;
            }
        }
    }

    void GAP::packetHandler(std::uint8_t packet_type, std::uint16_t channel, std::uint8_t *packet, std::uint16_t size)
    {

        if (packet_type != HCI_EVENT_PACKET) {
            return;
        }
        const auto eventType = hci_event_packet_get_type(packet);
        switch (state) {
        case stack::state::init:
            initStateHandler(eventType, packet);
            break;
        case stack::state::working:
            activeStateHandler(eventType, packet, size);
            break;
        default:
            break;
        }
    }

    GAP::GAP(sys::Service *owner)
    {
        ownerService = owner;
        state        = stack::state::init;
    }

    auto GAP::getDevicesList() -> std::vector<Devicei>
    {
        return devices().getList();
    }

    auto GAP::unpair(Devicei device) -> bool
    {
        LOG_INFO("Unpairing device");
        gap_drop_link_key_for_bd_addr(device.address);

        LOG_INFO("Device unpaired");
        ownerService->bus.sendMulticast(std::make_shared<message::bluetooth::UnpairResult>(device, true),
                                        sys::BusChannel::BluetoothNotifications);
        return true;
    }

    void GAP::respondPinCode(const std::string &pin, Devicei d)
    {
        LOG_DEBUG("pairing response for device: %s pin: %s is SSP? %s",
                  d.address_str(),
                  pin.c_str(),
                  d.isPairingSSP ? "yes" : "no");
        if (!d.isPairingSSP) {
            gap_pin_code_response(d.address, pin.c_str());
            return;
        }

        unsigned int passkey = 0;
        try {
            passkey = stoi(pin);
            LOG_DEBUG("Sending %06u as a passkey", passkey);
        }
        catch (const std::invalid_argument &e) {
            LOG_ERROR("STOI error: %s", e.what());
        }

        gap_ssp_passkey_response(d.address, passkey);
    }

    void GAP::processSimplePairingCompleted(std::uint8_t *packet, bd_addr_t &addr)
    {
        auto status = hci_event_simple_pairing_complete_get_status(packet);
        auto it     = devices().find(addr);
        LOG_INFO("HCI_EVENT_SIMPLE_PAIRING_COMPLETE: 0x%02X - %s - device found: %s : address:  %s",
                 status,
                 error_cstr(status),
                 it != devices().end() ? "found" : "fail",
                 bd_addr_to_str(addr));
        if (it == devices().end()) {
            auto msg = std::make_shared<BluetoothPairResultMessage>(Devicei(), false);
            ownerService->bus.sendUnicast(std::move(msg), service::name::bluetooth);
            return;
        }
        auto msg = std::make_shared<BluetoothPairResultMessage>(*it, status == ERROR_CODE_SUCCESS);
        ownerService->bus.sendUnicast(std::move(msg), service::name::bluetooth);
    }
    void GAP::finishCodeComparison(bool accepted, Devicei d)
    {
        if (accepted) {
            gap_ssp_confirmation_response(d.address);
        }
        else {
            gap_ssp_confirmation_negative(d.address);
        }
    }
} // namespace bluetooth
