// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-bluetooth/ServiceBluetooth.hpp>
#include "BluetoothWorker.hpp"
#include "BtCommand.hpp"
#include <log/log.hpp>
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
    constexpr auto BluetoothWorkerStackDepth = 4096U;

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
                constexpr auto name = "Mudita Pure";
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
                                  std::shared_ptr<bluetooth::Driver> driver,
                                  std::shared_ptr<bluetooth::CommandHandler> commandHandler,
                                  const std::shared_ptr<bluetooth::SettingsHolder> &settings,
                                  std::shared_ptr<bluetooth::BaseProfileManager> profileManager,
                                  DeviceRegistration::OnLinkKeyAddedCallback &&onLinkKeyAddedCallback,
                                  std::shared_ptr<std::vector<Devicei>> pairedDevices)
    {
        ;

        return std::make_unique<bluetooth::StatefulController>(
            driver,
            commandHandler,
            DeviceRegistration{settings, std::move(onLinkKeyAddedCallback)},
            settings,
            pairedDevices,
            profileManager);
    }
} // namespace

BluetoothWorker::BluetoothWorker(sys::Service *service)
    : Worker(service, BluetoothWorkerStackDepth), service(service),
      profileManager(std::make_shared<bluetooth::ProfileManager>(service)),
      settings(dynamic_cast<ServiceBluetooth *>(service)->settingsHolder),
      runLoop(std::make_unique<bluetooth::RunLoop>()),
      driver(std::make_shared<bluetooth::Driver>(runLoop->getRunLoopInstance(), service)),
      handler(std::make_shared<bluetooth::CommandHandler>(service, settings, profileManager, driver)),
      controller{createStatefulController(
          service,
          driver,
          handler,
          settings,
          profileManager,
          [this](const std::string &addr) { onLinkKeyAdded(addr); },
          pairedDevices)}
{
    init({
        {queues::io, sizeof(bluetooth::Message), queues::queueLength},
        {queues::cmd, 0, queues::queueLength},
        {queues::btstack, sizeof(bool), queues::triggerQueueLength},
    });
    registerQueues();
}

void BluetoothWorker::registerQueues()
{
    workerQueue = std::make_shared<Mailbox<bluetooth::Command, QueueHandle_t, WorkerLock>>(
        Worker::getQueueHandleByName(queues::cmd));

    dynamic_cast<ServiceBluetooth *>(service)->workerQueue = workerQueue;
    runLoop->setTriggerQueue(Worker::getQueueHandleByName(queues::btstack));
    bsp::BlueKitchen::getInstance()->qHandle = queues[queueIO_handle]->GetQueueHandle();
}

void BluetoothWorker::onLinkKeyAdded(const std::string &deviceAddress)
{
    auto devices = bluetooth::GAP::getDevicesList();
    for (auto &device : devices) {
        if (bd_addr_to_str(device.address) == deviceAddress) {
            pairedDevices->emplace_back(device);
            settings->setValue(bluetooth::Settings::BondedDevices, SettingsSerializer::toString(*pairedDevices));
        }
    }
}

BluetoothWorker::~BluetoothWorker()
{
    controller->handle(bluetooth::event::ShutDown{});
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
    LOG_INFO("handle bluetooth command(s)");
    xQueueReceive(queue, nullptr, 0);
    while (not workerQueue->empty()) {
        auto command = workerQueue->peek();
        if (command == std::nullopt) {
            LOG_ERROR("There was no data even with notification");
            break;
        }
        if (command->event != nullptr) {
            controller->handle(*command->event);
        }
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
    auto bt = bsp::BlueKitchen::getInstance();
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

void BluetoothWorker::closeWorker()
{
    controller->handle(bluetooth::event::PowerOff{});
    this->close();
}

void BluetoothWorker::removeFromBoundDevices(uint8_t *addr)
{
}

void BluetoothWorker::setAudioDevice(std::shared_ptr<bluetooth::BluetoothAudioDevice> device)
{
    cpp_freertos::LockGuard lock(loopMutex);
    profileManager->setAudioDevice(std::move(device));
}
