// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "audio/BluetoothAudioDevice.hpp"
#include "glucode/BluetoothRunLoop.hpp"
#include "interface/profiles/Profile.hpp"
#include "service-bluetooth/SettingsHolder.hpp"
#include "service-bluetooth/WorkerLock.hpp"
#include "Service/Worker.hpp"

#include "Device.hpp"
#include "WorkerController.hpp"

#include <bsp/bluetooth/Bluetooth.hpp>

#include <mutex.hpp>

#include <FreeRTOS.h>
#include <task.h>

#include <memory>
#include <vector>

struct HCI;

/// debug option for HCI (uart) commands debugging
// #define DO_DEBUG_HCI_COMS

namespace bluetooth
{
    using namespace bsp::bluetooth;

    inline const char *MessageCstr(Message what)
    {
        switch (what) {
        case EvtReceived:
            return "EvtReceived";
        case EvtSent:
            return "EvtSent";
        case EvtRecUnwanted:
            return "EvtRecUnwanted";
        case EvtReceivingError:
            return "EvtReceivingError";
        case EvtSendingError:
            return "EvtSendingError";
        case EvtUartError:
            return "EvtUartError";
        case EvtErrorRec:
            return "EvtErrorRec";
        default:
            return "";
        }
    }

    struct EvtWorker
    {
        enum Evt : uint8_t
        {
        };
    };
}; // namespace bluetooth

struct DeviceStore
{};

namespace bluetooth
{
    class Driver;
    class CommandHandler;
}; // namespace bluetooth

class BluetoothWorker : private sys::Worker
{
    enum WorkerEventQueues
    {
        queueService = 0,
        queueControl = 1,
        queueIO_handle, /// bsp support queue
        queueCommands,
        queueRunloopTrigger // btstack run_loop queue
    };

    sys::Service *service = nullptr;
    bool isRunning        = false;
    cpp_freertos::MutexStandard loopMutex;

    void registerQueues();
    void onLinkKeyAdded(const std::string &deviceAddress);
    void removeFromBoundDevices(uint8_t *addr);

    std::shared_ptr<Mailbox<bluetooth::Command, QueueHandle_t, WorkerLock>> workerQueue;

  public:
    enum Error
    {
        SuccessBt,
        ErrorBtGeneric,
        ErrorBtAPI,
    };

    explicit BluetoothWorker(sys::Service *service);
    ~BluetoothWorker() override;

    auto handleMessage(uint32_t queueID) -> bool override;
    auto handleCommand(QueueHandle_t queue) -> bool;
    auto handleBtStackTrigger(QueueHandle_t queue) -> bool;

    bool run() override;
    void closeWorker();

    void setAudioDevice(std::shared_ptr<bluetooth::BluetoothAudioDevice> device);

    /// bluetooth stack id in use
    unsigned long active_features;
    std::shared_ptr<bluetooth::ProfileManager> profileManager;
    std::shared_ptr<bluetooth::SettingsHolder> settings;
    std::shared_ptr<std::vector<Devicei>> pairedDevices = std::make_shared<std::vector<Devicei>>();
    std::unique_ptr<bluetooth::RunLoop> runLoop;
    std::shared_ptr<bluetooth::Driver> driver;
    std::shared_ptr<bluetooth::CommandHandler> handler;
    std::unique_ptr<bluetooth::AbstractController> controller;
};
