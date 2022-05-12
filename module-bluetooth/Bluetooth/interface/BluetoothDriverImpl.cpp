// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BluetoothDriverImpl.hpp"

extern "C"
{
#include <btstack_event.h>
#include <btstack_util.h>
#include <bluetooth_company_id.h>
#include <btstack_memory.h>
#include <btstack_run_loop.h>
#include <hci.h>
#include <hci_dump.h>

#include <btstack_chipset_cc256x.h>
#include <btstack_link_key_db_memory.h>
}

#include <service-bluetooth/ServiceBluetooth.hpp>
#include <BtKeysStorage.hpp>

#ifdef TARGET_RT1051
#include <Bluetooth/glucode/btstack_uart_block_rt1051.h>
#else
extern "C"
{
#include <btstack_uart.h>

#include <btstack_run_loop_posix.h>
#include <btstack_tlv_posix.h>
}
#endif

namespace bluetooth
{
    hci_transport_config_uart_t Driver::config;
    PowerOnCallback Driver::powerOnCallback = nullptr;

#ifdef TARGET_RT1051
    [[maybe_unused]] auto Driver::runLoopInitTarget(const btstack_run_loop *loop) -> const btstack_uart_block_t *
    {
        btstack_run_loop_init(loop);
        const btstack_uart_block_t *uartDriver = btstack_uart_block_rt1051_instance();
        return uartDriver;
    }
#else

    [[maybe_unused]] auto Driver::runLoopInitLinux(const btstack_run_loop *) -> const btstack_uart_block_t *
    {
        btstack_run_loop_init(btstack_run_loop_posix_get_instance());
        config.device_name = "/dev/telit";
        LOG_INFO("H4 device: %s", config.device_name);
        const btstack_uart_block_t *uartDriver = btstack_uart_block_posix_instance();
        return uartDriver;
    }
#endif

    Driver::Driver(const btstack_run_loop *runLoop, sys::Service *ownerService)
        : runLoop{runLoop}, gap{std::make_unique<bluetooth::GAP>(ownerService)}
    {}

    auto Driver::init() -> Error::Code
    {
        btstack_memory_init();
        config = {
            .type          = HCI_TRANSPORT_CONFIG_UART,
            .baudrate_init = 115200,
            .baudrate_main = 0,
            .flowcontrol   = 1,
            .device_name   = nullptr,
        };
#ifdef TARGET_RT1051
        auto uartDriver = runLoopInitTarget(runLoop);
#else
        auto uartDriver = runLoopInitLinux(runLoop);
#endif

        const hci_transport_t *transport = hci_transport_h4_instance_for_uart(uartDriver);
        hci_init(transport, (void *)&config);

        hci_set_link_key_db(bluetooth::KeyStorage::getKeyStorage());
        hci_event_callback_registration.callback = &hci_packet_handler;
        hci_add_event_handler(&hci_event_callback_registration);

        gap_ssp_set_io_capability(SSP_IO_CAPABILITY_DISPLAY_YES_NO);
        gap_ssp_set_auto_accept(false);

        gap_set_class_of_device(0x64020C);
        LOG_DEBUG("BT worker run success");
        return Error::Success;
    }

    void Driver::hci_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size)
    {
        bd_addr_t addr;
        if (packet_type != HCI_EVENT_PACKET) {
            return;
        }
        switch (hci_event_packet_get_type(packet)) {
        case BTSTACK_EVENT_STATE:
            if (btstack_event_state_get_state(packet) != HCI_STATE_WORKING) {
                break;
            }
            gap_local_bd_addr(addr);
            LOG_INFO("BTstack up and running");
            bluetooth::KeyStorage::settings->setValue(bluetooth::Settings::State,
                                                      static_cast<int>(BluetoothStatus::State::On));
            if (powerOnCallback) {
                powerOnCallback();
            }
            break;
        case HCI_EVENT_COMMAND_COMPLETE:
            if (HCI_EVENT_IS_COMMAND_COMPLETE(packet, hci_read_local_name)) {
                if (hci_event_command_complete_get_return_parameters(packet)[0] != 0u) {
                    break;
                }
                // terminate, name 248 chars
                packet[6 + 248] = 0;
            }
            if (HCI_EVENT_IS_COMMAND_COMPLETE(packet, hci_read_local_version_information)) {
                local_version_information_handler(packet);
            }
            break;
        default:
            break;
        }
    }
    void Driver::local_version_information_handler(uint8_t *packet)
    {
        LOG_INFO("Local version information:");
        uint16_t hci_version    = packet[6];
        uint16_t hci_revision   = little_endian_read_16(packet, 7);
        uint16_t lmp_version    = packet[9];
        uint16_t manufacturer   = little_endian_read_16(packet, 10);
        uint16_t lmp_subversion = little_endian_read_16(packet, 12);
        LOG_INFO("- HCI Version    0x%04x", hci_version);
        LOG_INFO("- HCI Revision   0x%04x", hci_revision);
        LOG_INFO("- LMP Version    0x%04x", lmp_version);
        LOG_INFO("- LMP Subversion 0x%04x", lmp_subversion);
        LOG_INFO("- Manufacturer 0x%04x", manufacturer);
        switch (manufacturer) {
        case BLUETOOTH_COMPANY_ID_TEXAS_INSTRUMENTS_INC:
            LOG_INFO("Texas Instruments - CC256x compatible chipset.");
            if (lmp_subversion != btstack_chipset_cc256x_lmp_subversion()) {
                LOG_INFO("Error: LMP Subversion does not match initscript! ");
                LOG_INFO("Your initscripts is for %s chipset",
                         btstack_chipset_cc256x_lmp_subversion() < lmp_subversion ? "an older" : "a newer");
                LOG_INFO("Please update Makefile to include the appropriate bluetooth_init_cc256???.c file");
                return;
            }
            LOG_INFO("Using 921600 baud");
            config.baudrate_main = 921600;
            hci_set_chipset(btstack_chipset_cc256x_instance());
#ifdef ENABLE_EHCILL
            LOG_INFO("eHCILL enabled.");
#else
            LOG_INFO("eHCILL disable.");
#endif

            break;
        case BLUETOOTH_COMPANY_ID_NORDIC_SEMICONDUCTOR_ASA:
            LOG_INFO("Nordic Semiconductor nRF5 chipset.");
            break;
        default:
            LOG_INFO("Unknown manufacturer / manufacturer not supported yet.\n");
            break;
        }
    }
    auto Driver::run() -> Error::Code
    {
        auto ret = hci_power_control(HCI_POWER_ON);
        if (ret != 0) {
            LOG_ERROR("HCI power on failed, can't start Bluetooth!");
            return Error::LibraryError;
        }
        LOG_INFO("HCI turned on - run BtStack loop\n");
        btstack_run_loop_execute();
        return Error::Success;
    }

    void Driver::registerErrorCallback(const ErrorCallback &newCallback)
    {
        static ErrorCallback callback;
        callback = newCallback;
        hci_set_hardware_error_callback([](uint8_t val) -> void {
            LOG_ERROR("Bluetooth HW ERROR! %d", val);
            if (callback) {
                callback(val);
            }
        });
    }

    void Driver::registerPowerOnCallback(const PowerOnCallback &newCallback)
    {
        powerOnCallback = newCallback;
    }

    auto Driver::stop() -> Error::Code
    {
        auto ret = hci_power_control(HCI_POWER_OFF);
        if (ret != 0) {
            LOG_ERROR("Can't turn off Bluetooth Stack!");
        }
        bluetooth::KeyStorage::settings->setValue(bluetooth::Settings::State,
                                                  static_cast<int>(BluetoothStatus::State::Off));
        return ret != 0 ? Error::LibraryError : Error::Success;
    }
    auto Driver::scan() -> Error
    {
        return gap->scan();
    }
    void Driver::stopScan()
    {
        gap->stopScan();
    }
    void Driver::setVisibility(bool visibility)
    {
        gap->setVisibility(visibility);
    }
    void Driver::pair(Devicei device, std::uint8_t protectionLevel)
    {
        LOG_INFO("Device: %s, addr: %s", device.name.data(), device.address_str());
        gap->pair(device, protectionLevel);
    }

    void Driver::unpair(Devicei device)
    {
        gap->unpair(device);
    }

} // namespace bluetooth
