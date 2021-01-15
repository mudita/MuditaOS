// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BluetoothWorker.hpp"
#include "BtCommand.hpp"
#include "log/log.hpp"
#include "interface/profiles/A2DP/A2DP.hpp"
#include "interface/profiles/HSP/HSP.hpp"
#include "BtKeysStorage.hpp"
extern "C"
{
#include "module-bluetooth/lib/btstack/src/btstack_util.h"
}
#include <btstack_run_loop_freertos.h>
#include <service-bluetooth/ServiceBluetooth.hpp>

#if DEBUG_BLUETOOTH_HCI_COMS == 1
#define logHciComs(...) LOG_DEBUG(__VA_ARGS__)
#else
#define logHciComs(...)
#endif

#if DEBUG_BLUETOOTH_HCI_BYTES == 1
#include <sstream>
#define logHciBytes(...) LOG_DEBUG(__VA_ARGS__)
#else
#define logHciBytes(...)
#endif

using namespace bsp;

const char *c_str(Bt::Error::Code code)
{
    switch (code) {
    case Bt::Error::Code::Success:
        return "Success";
    case Bt::Error::Code::NotReady:
        return "NotReady";
    case Bt::Error::Code::SystemError:
        return "SystemError";
    case Bt::Error::Code::LibraryError:
        return "LibraryError";
    }
    return "";
}
namespace queues
{
    constexpr inline auto io  = "qBtIO";
    constexpr inline auto cmd = "qBtCmds";

} // namespace queues

BluetoothWorker::BluetoothWorker(sys::Service *service)
    : Worker(service), service(service), currentProfile(std::make_shared<Bt::HSP>()),
      settings(static_cast<ServiceBluetooth *>(service)->settingsHolder)
{
    init({
        {queues::io, sizeof(Bt::Message), 10},
        {queues::cmd, sizeof(Bt::Command), 10},
    });
    static_cast<ServiceBluetooth *>(service)->workerQueue = Worker::getQueueHandleByName(queues::cmd);
}

BluetoothWorker::~BluetoothWorker()
{
    if (this->bt_worker_task != nullptr) {
        vTaskDelete(this->bt_worker_task);
    }
    LOG_INFO("Worker removed");
}

bool BluetoothWorker::run()
{
    LOG_INFO("-> BluetoothWorker run request");
    if (is_running) {
        return true;
    }
    if (Worker::run()) {
        is_running                          = true;
        auto el                             = queues[queueIO_handle];
        BlueKitchen::getInstance()->qHandle = el->GetQueueHandle();
        Bt::KeyStorage::settings            = settings;
        Bt::initialize_stack();
        Bt::register_hw_error_callback();
        Bt::GAP::register_scan();

        std::string name = "PurePhone";
        auto settingsName = std::get<std::string>(settings->getValue(Bluetooth::Settings::DeviceName));
        if (settingsName.empty()) {
            LOG_ERROR("settings name empty!");
            settings->setValue(Bluetooth::Settings::DeviceName, name);
            settingsName = name;
        }
        Bt::set_name(settingsName);
        Bt::GAP::set_visibility(
            std::visit(Bluetooth::BoolVisitor(), settings->getValue(Bluetooth::Settings::Visibility)));

        settings->setValue(Bluetooth::Settings::State, static_cast<int>(BluetoothStatus::State::On));
        settings->onLinkKeyAdded = [this](std::string addr) {
            for (auto &device : Bt::GAP::devices) {
                if (bd_addr_to_str(device.address) == addr) {
                    // found paired device
                    pairedDevices.emplace_back(device);
                    settings->setValue(Bluetooth::Settings::BondedDevices, SettingsSerializer::toString(pairedDevices));
                }
            }
        };

        Bt::run_stack(&this->bt_worker_task);
        return true;
    }
    else {
        return false;
    }
}

bool BluetoothWorker::scan()
{
    std::vector<Device> empty;
    Bt::GAP::setOwnerService(service);
    auto ret = Bt::GAP::scan();
    if (ret.err != Bt::Error::Success) {
        LOG_ERROR("Cant start scan!: %s %" PRIu32 "", c_str(ret.err), ret.lib_code);
        return false;
    }
    else {
        LOG_INFO("Scan started!");
        // open new scan window
        return true;
    }
}

void BluetoothWorker::stopScan()
{
    Bt::GAP::stop_scan();
}

void BluetoothWorker::setVisibility(bool visibility)
{
    Bt::GAP::set_visibility(visibility);
    settings->setValue(Bluetooth::Settings::Visibility, visibility);
}

bool BluetoothWorker::start_pan()
{
    Bt::PAN::bnep_setup();
    auto err = Bt::PAN::bnep_start();
    if (err.err != Bt::Error::Success) {
        LOG_ERROR("PAN setup error: %s %" PRIu32, c_str(err.err), err.lib_code);
    }
    return false;
}
bool BluetoothWorker::handleCommand(QueueHandle_t queue)
{
    Bt::Command command;
    if (xQueueReceive(queue, &command, 0) != pdTRUE) {
        LOG_ERROR("Queue receive failure!");
        return false;
    }
    switch (command) {
    case Bt::PowerOn:

        break;
    case Bt::StartScan:
        scan();
        break;
    case Bt::StopScan:
        stopScan();
        break;
    case Bt::VisibilityOn:
        setVisibility(true);
        break;
    case Bt::VisibilityOff:
        setVisibility(false);
        break;
    case Bt::ConnectAudio:
        establishAudioConnection();
        break;
    case Bt::DisconnectAudio:
        disconnectAudioConnection();
        break;
    case Bt::PowerOff:
        break;
    }
    return true;
}
bool BluetoothWorker::handleMessage(uint32_t queueID)
{
    QueueHandle_t queue = queues[queueID]->GetQueueHandle();
    if (queueID == queueService) {
        LOG_DEBUG("not interested");
        return true;
    }

    if (queueID == queueCommands) {
        handleCommand(queue);
        return true;
    }

    if (queueID != queueIO_handle) {
        LOG_ERROR("Wrong queue! %" PRIu32, queueID);
        return false;
    }

    Bt::Message notification = Bt::Message::EvtErrorRec;
    if (xQueueReceive(queue, &notification, 0) != pdTRUE) {
        LOG_ERROR("Queue receive failure!");
        return false;
    }
    auto bt = BlueKitchen::getInstance();
    switch (notification) {
    case Bt::Message::EvtSending:
        logHciComs("[evt] sending");
        break;
    case Bt::Message::EvtSent:
        logHciComs("[evt] sent");
        if (bt->write_done_cb) {
            btstack_run_loop_freertos_execute_code_on_main_thread(reinterpret_cast<void (*)(void *)>(bt->write_done_cb),
                                                                  nullptr);
        }
        break;
    case Bt::Message::EvtReceiving:
        logHciComs("[evt] receiving");
        break;
    case Bt::Message::EvtReceived: {
        logHciBytes("[evt] BT DMA received <-- [%ld]>%s<",
                    bt->read_len,
                    [&]() -> std::string {
                        std::stringstream ss;
                        for (int i = 0; i < bt->read_len; ++i) {
                            ss << " 0x" << std::hex << (int)*(bt->read_buff + i);
                        }
                        return ss.str();
                    }()
                                 .c_str());

        bt->read_len = 0;

        if (bt->read_ready_cb) {
            btstack_run_loop_freertos_execute_code_on_main_thread(reinterpret_cast<void (*)(void *)>(bt->read_ready_cb),
                                                                  nullptr);
        }
    } break;
    case Bt::Message::EvtSendingError:
    case Bt::Message::EvtReceivingError:
    case Bt::Message::EvtUartError:
    case Bt::Message::EvtRecUnwanted:
        LOG_ERROR("Uart error [%d]: %s", notification, Bt::MessageCstr(notification));
        break;
    default:
        LOG_ERROR("ERROR");
    }

    return true;
}

bool BluetoothWorker::establishAudioConnection()
{
    currentProfile->setOwnerService(service);
    if (currentProfile->init() != Bt::Error::Success) {
        return false;
    }
    currentProfile->connect();
    return true;
}
bool BluetoothWorker::disconnectAudioConnection()
{
    currentProfile->disconnect();
    return true;
}
void BluetoothWorker::setDeviceAddress(bd_addr_t addr)
{
    Bt::GAP::do_pairing(addr);
    currentProfile->setDeviceAddress(addr);
}
