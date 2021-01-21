// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-bluetooth/ServiceBluetooth.hpp>
#include "BluetoothWorker.hpp"
#include "BtCommand.hpp"
#include "log/log.hpp"
#include "interface/profiles/A2DP/A2DP.hpp"
#include "interface/profiles/HSP/HSP.hpp"
#include "BtKeysStorage.hpp"

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

[[nodiscard]] auto to_string(bluetooth::Error::Code code) -> std::string
{
    return utils::enumToString(code);
}
namespace queues
{
    constexpr inline auto io  = "qBtIO";
    constexpr inline auto cmd = "qBtCmds";
    constexpr inline auto btstack = "qBtStack";

    constexpr inline auto queueLength        = 10;
    constexpr inline auto triggerQueueLength = 3;

} // namespace queues

BluetoothWorker::BluetoothWorker(sys::Service *service)
    : Worker(service), service(service), currentProfile(std::make_shared<bluetooth::HSP>()),
      settings(static_cast<ServiceBluetooth *>(service)->settingsHolder),
      runLoop(std::make_unique<bluetooth::RunLoop>()), driver(std::make_unique<bluetooth::Driver>())
{
    init({
        {queues::io, sizeof(bluetooth::Message), queues::queueLength},
        {queues::cmd, sizeof(bluetooth::Command), queues::queueLength},
        {queues::btstack, sizeof(bool), queues::triggerQueueLength},
    });
    static_cast<ServiceBluetooth *>(service)->workerQueue = Worker::getQueueHandleByName(queues::cmd);
    runLoop->setTriggerQueue(Worker::getQueueHandleByName(queues::btstack));
    driver->init(runLoop->getRunLoopInstance());
}

BluetoothWorker::~BluetoothWorker()
{
    if (this->bt_worker_task != nullptr) {
        vTaskDelete(this->bt_worker_task);
    }
    LOG_INFO("Worker removed");
}

auto BluetoothWorker::run() -> bool
{
    LOG_INFO("-> BluetoothWorker run request");
    if (is_running) {
        return true;
    }
    if (Worker::run()) {
        is_running                          = true;
        auto el                             = queues[queueIO_handle];
        BlueKitchen::getInstance()->qHandle = el->GetQueueHandle();
        bluetooth::KeyStorage::settings     = settings;

        driver->registerHardwareErrorCallback(nullptr);
        bluetooth::GAP::register_scan();

        std::string name = "PurePhone";
        auto settingsName = std::get<std::string>(settings->getValue(bluetooth::Settings::DeviceName));
        if (settingsName.empty()) {
            LOG_ERROR("settings name empty!");
            settings->setValue(bluetooth::Settings::DeviceName, name);
            settingsName = name;
        }

        bluetooth::set_name(settingsName);
        bluetooth::GAP::set_visibility(
            std::visit(bluetooth::BoolVisitor(), settings->getValue(bluetooth::Settings::Visibility)));

        settings->setValue(bluetooth::Settings::State, static_cast<int>(BluetoothStatus::State::On));

        settings->onLinkKeyAdded = [this](std::string addr) {
            for (auto &device : bluetooth::GAP::devices) {
                if (bd_addr_to_str(device.address) == addr) {
                    // found paired device
                    pairedDevices.emplace_back(device);
                    settings->setValue(bluetooth::Settings::BondedDevices, SettingsSerializer::toString(pairedDevices));
                }
            }
        };

        return driver->run() == bluetooth::Error::Success;
    }
    else {
        return false;
    }
}

auto BluetoothWorker::scan() -> bool
{
    std::vector<Device> empty;
    bluetooth::GAP::setOwnerService(service);
    auto ret = bluetooth::GAP::scan();
    if (ret.err != bluetooth::Error::Success) {
        LOG_ERROR("Cant start scan!: %s %" PRIu32 "", to_string(ret.err).c_str(), ret.lib_code);
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
    bluetooth::GAP::stop_scan();
}

void BluetoothWorker::setVisibility(bool visibility)
{
    bluetooth::GAP::set_visibility(visibility);
    settings->setValue(bluetooth::Settings::Visibility, visibility);
}

auto BluetoothWorker::start_pan() -> bool
{
    bluetooth::PAN::bnep_setup();
    auto err = bluetooth::PAN::bnep_start();
    if (err.err != bluetooth::Error::Success) {
        LOG_ERROR("PAN setup error: %s %" PRIu32, to_string(err.err).c_str(), err.lib_code);
    }
    return false;
}
auto BluetoothWorker::handleCommand(QueueHandle_t queue) -> bool
{
    bluetooth::Command command;
    if (xQueueReceive(queue, &command, 0) != pdTRUE) {
        LOG_ERROR("Queue receive failure!");
        return false;
    }
    switch (command) {
    case bluetooth::PowerOn:

        break;
    case bluetooth::StartScan:
        scan();
        break;
    case bluetooth::StopScan:
        stopScan();
        break;
    case bluetooth::VisibilityOn:
        setVisibility(true);
        break;
    case bluetooth::VisibilityOff:
        setVisibility(false);
        break;
    case bluetooth::ConnectAudio:
        establishAudioConnection();
        break;
    case bluetooth::DisconnectAudio:
        disconnectAudioConnection();
        break;
    case bluetooth::PowerOff:
        break;
    }
    return true;
}

auto BluetoothWorker::handleBtStackTrigger(QueueHandle_t queue) -> bool
{
    bool notification;
    if (xQueueReceive(queue, &notification, 0) != pdTRUE) {
        LOG_ERROR("Queue receive failure!");
        return false;
    }
    if (notification) {
        runLoop->process();

        return true;
    }
    else {
        return false;
    }
}

auto BluetoothWorker::handleMessage(uint32_t queueID) -> bool
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
    if (queueID == queueRunloopTrigger) {
        handleBtStackTrigger(queue);
        return true;
    }

    if (queueID != queueIO_handle) {
        LOG_ERROR("Wrong queue! %" PRIu32, queueID);
        return false;
    }

    auto notification = bluetooth::Message::EvtErrorRec;
    if (xQueueReceive(queue, &notification, 0) != pdTRUE) {
        LOG_ERROR("Queue receive failure!");
        return false;
    }
    auto bt = BlueKitchen::getInstance();
    switch (notification) {
    case bluetooth::Message::EvtSending:
        logHciComs("[evt] sending");
        break;
    case bluetooth::Message::EvtSent:
        logHciComs("[evt] sent");
        if (bt->write_done_cb) {
            bt->write_done_cb();
        }
        break;
    case bluetooth::Message::EvtReceiving:
        logHciComs("[evt] receiving");
        break;
    case bluetooth::Message::EvtReceived: {
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
            bt->read_ready_cb();
        }
    } break;
    case bluetooth::Message::EvtSendingError:
        [[fallthrough]];
    case bluetooth::Message::EvtReceivingError:
        [[fallthrough]];
    case bluetooth::Message::EvtUartError:
        [[fallthrough]];
    case bluetooth::Message::EvtRecUnwanted:
        LOG_ERROR("Uart error [%d]: %s", notification, bluetooth::MessageCstr(notification));
        break;
    default:
        LOG_ERROR("ERROR");
    }

    return true;
}

auto BluetoothWorker::establishAudioConnection() -> bool
{
    currentProfile->setOwnerService(service);
    if (currentProfile->init() != bluetooth::Error::Success) {
        return false;
    }
    currentProfile->connect();
    return true;
}
auto BluetoothWorker::disconnectAudioConnection() -> bool
{
    currentProfile->disconnect();
    return true;
}
void BluetoothWorker::setDeviceAddress(bd_addr_t addr)
{
    bluetooth::GAP::do_pairing(addr);
    currentProfile->setDeviceAddress(addr);
}
