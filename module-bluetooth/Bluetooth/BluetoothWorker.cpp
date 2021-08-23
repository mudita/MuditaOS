// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-bluetooth/ServiceBluetooth.hpp>
#include "BluetoothWorker.hpp"
#include "BtCommand.hpp"
#include <log.hpp>
#include "interface/BluetoothDriverImpl.hpp"
#include "interface/profiles/A2DP/A2DP.hpp"
#include "interface/profiles/HSP/HSP.hpp"
#include "audio/BluetoothAudioDevice.hpp"
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

namespace queues
{
    constexpr inline auto io      = "qBtIO";
    constexpr inline auto cmd     = "qBtCmds";
    constexpr inline auto btstack = "qBtStack";

    constexpr inline auto queueLength        = 10;
    constexpr inline auto triggerQueueLength = 3;
} // namespace queues

namespace
{
    constexpr auto BluetoothWorkerStackDepth = 3072U;

    class DeviceRegistration
    {
      public:
        using OnLinkKeyAddedCallback = std::function<void(const std::string &)>;

        DeviceRegistration(std::shared_ptr<bluetooth::SettingsHolder> settings, OnLinkKeyAddedCallback &&onLinkKeyAdded)
            : settings{std::move(settings)}, onLinkKeyAdded{std::move(onLinkKeyAdded)}
        {}

        [[nodiscard]] auto operator()()
        {
            bluetooth::GAP::registerScan();

            auto settingsName = std::get<std::string>(settings->getValue(bluetooth::Settings::DeviceName));
            if (settingsName.empty()) {
                LOG_WARN("Settings name is empty!");
                constexpr auto name = "PurePhone";
                settings->setValue(bluetooth::Settings::DeviceName, name);
                settingsName = name;
            }
            bluetooth::set_name(settingsName);

            settings->onLinkKeyAdded = onLinkKeyAdded;
            return bluetooth::Error::Success;
        }

      private:
        std::shared_ptr<bluetooth::SettingsHolder> settings;
        OnLinkKeyAddedCallback onLinkKeyAdded;
    };

    auto createStatefulController(sys::Service *service,
                                  bluetooth::RunLoop *loop,
                                  std::shared_ptr<bluetooth::SettingsHolder> settings,
                                  std::shared_ptr<bluetooth::ProfileManager> profileManager,
                                  DeviceRegistration::OnLinkKeyAddedCallback &&onLinkKeyAddedCallback)
    {
        auto driver = std::make_shared<bluetooth::Driver>(loop->getRunLoopInstance(), service);
        auto commandHandler =
            std::make_unique<bluetooth::CommandHandler>(service, settings, std::move(profileManager), driver);
        return std::make_unique<bluetooth::StatefulController>(
            std::move(driver),
            std::move(commandHandler),
            DeviceRegistration{std::move(settings), std::move(onLinkKeyAddedCallback)});
    }
} // namespace

BluetoothWorker::BluetoothWorker(sys::Service *service)
    : Worker(service, BluetoothWorkerStackDepth), service(service),
      profileManager(std::make_shared<bluetooth::ProfileManager>(service)),
      settings(static_cast<ServiceBluetooth *>(service)->settingsHolder),
      runLoop(std::make_unique<bluetooth::RunLoop>()),
      controller{createStatefulController(
          service, runLoop.get(), settings, profileManager, [this](const std::string &addr) { onLinkKeyAdded(addr); })}
{
    init({
        {queues::io, sizeof(bluetooth::Message), queues::queueLength},
        {queues::cmd, sizeof(bluetooth::Command), queues::queueLength},
        {queues::btstack, sizeof(bool), queues::triggerQueueLength},
    });
    registerQueues();
}

void BluetoothWorker::registerQueues()
{
    static_cast<ServiceBluetooth *>(service)->workerQueue = Worker::getQueueHandleByName(queues::cmd);
    runLoop->setTriggerQueue(Worker::getQueueHandleByName(queues::btstack));
    BlueKitchen::getInstance()->qHandle = queues[queueIO_handle]->GetQueueHandle();
}

void BluetoothWorker::onLinkKeyAdded(const std::string &deviceAddress)
{
    auto devices = bluetooth::GAP::getDevicesList();
    for (auto &device : devices) {
        if (bd_addr_to_str(device.address) == deviceAddress) {
            pairedDevices.emplace_back(device);
            settings->setValue(bluetooth::Settings::BondedDevices, SettingsSerializer::toString(pairedDevices));
        }
    }
}

BluetoothWorker::~BluetoothWorker()
{
    controller->shutdown();
}

auto BluetoothWorker::run() -> bool
{
    LOG_INFO("-> BluetoothWorker run request");
    if (isRunning) {
        return true;
    }

    if (const auto status = Worker::run(); !status) {
        return false;
    }
    isRunning = true;
    return true;
}

auto BluetoothWorker::handleCommand(QueueHandle_t queue) -> bool
{
    bluetooth::Command command(bluetooth::Command::Type::None);
    if (xQueueReceive(queue, static_cast<void *>(&command), 0) != pdTRUE) {
        LOG_ERROR("Queue receive failure!");
        return false;
    }

    switch (command.getType()) {
    case bluetooth::Command::PowerOn:
        initDevicesList();
        controller->turnOn();
        break;
    case bluetooth::Command::PowerOff:
        controller->turnOff();
        break;
    case bluetooth::Command::Unpair:
        controller->processCommand(command);
        removeFromBoundDevices(command.getDevice().address);
        break;
    case bluetooth::Command::None:
        break;
    default:
        controller->processCommand(command);
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
        cpp_freertos::LockGuard lock(loopMutex);
        runLoop->process();
        return true;
    }
    return false;
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

auto BluetoothWorker::deinit() -> bool
{
    controller->turnOff();
    return Worker::deinit();
}
void BluetoothWorker::initDevicesList()
{
    auto bondedDevicesStr =
        std::visit(bluetooth::StringVisitor(), settings->getValue(bluetooth::Settings::BondedDevices));
    pairedDevices = SettingsSerializer::fromString(bondedDevicesStr);
}
void BluetoothWorker::removeFromBoundDevices(uint8_t *addr)
{
    auto position = std::find_if(
        pairedDevices.begin(), pairedDevices.end(), [&](Devicei device) { return !bd_addr_cmp(addr, device.address); });
    if (position != pairedDevices.end()) {
        pairedDevices.erase(position);
        settings->setValue(bluetooth::Settings::BondedDevices, SettingsSerializer::toString(pairedDevices));
        LOG_INFO("Device removed from paired devices list");
    }
}

void BluetoothWorker::setAudioDevice(std::shared_ptr<bluetooth::BluetoothAudioDevice> device)
{
    cpp_freertos::LockGuard lock(loopMutex);
    profileManager->setAudioDevice(std::move(device));
}
