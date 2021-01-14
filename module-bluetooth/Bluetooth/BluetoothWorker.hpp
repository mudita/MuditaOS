// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Device.hpp"
#include "Service/Worker.hpp"
#include "interface/profiles/Profile.hpp"
#include <FreeRTOS.h>
#include <bsp/bluetooth/Bluetooth.hpp>
#include <memory>
#include <task.h>
#include <vector>
#include "service-bluetooth/SettingsHolder.hpp"

struct HCI;

/// debug option for HCI (uart) commands debugging
// #define DO_DEBUG_HCI_COMS

namespace Bt
{
    enum Message : std::uint8_t
    {
        /// asynchronous messages to use on event from irq
        EvtSending,        /// Bt stack ordered a write transaction and it is pending
        EvtSent,           /// trigger Bt stack wrote, enable writting in HCI in BluetoothWorker task
        EvtSendingError,   /// bsp error on send
        EvtReceiving,      /// Bt stack requested a receive transaction and it is pending
        EvtReceived,       /// trigger Bt stack received, start processing HCI in BluetoothWorker task
        EvtRecUnwanted,    /// not requested recieve - probably receive came to fast from sent...
        EvtReceivingError, /// bsp error on receive
        EvtUartError,      /// generic uart error
        EvtErrorRec,       /// there was error o queue receive
    };

    enum Command : std::uint8_t
    {
        StartScan,
        StopScan,
        VisibilityOn,
        VisibilityOff,
        ConnectAudio,
        DisconnectAudio,
        PowerOn,
        PowerOff,
    };

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
}; // namespace Bt

class BluetoothWorker : private sys::Worker
{
    enum WorkerEventQueues
    {
        queueService = 0,
        queueControl = 1,
        queueIO_handle, /// bsp support queue
                        //        queue_profiles, /// queue for communication between profile workers,
                        //                        /// main bt_worker_task should dispatch these in events
        queueCommands,
    };

    TaskHandle_t bt_worker_task = nullptr;
    int is_running              = false;
    sys::Service *service       = nullptr;

  public:
    enum Error
    {
        SuccessBt,
        ErrorBtGeneric,
        ErrorBtAPI,
    };

    BluetoothWorker(sys::Service *service);
    virtual ~BluetoothWorker();

    virtual bool handleMessage(uint32_t queueID);

    bool handleCommand(QueueHandle_t queue);

    bool run();

    bool scan();

    void setVisibility(bool visibility);

    bool start_pan();

    bool establishAudioConnection();

    bool disconnectAudioConnection();

    Error aud_init();
    /// bluetooth stack id in use
    unsigned long active_features;
    void stopScan();
    void setDeviceAddress(bd_addr_t addr);
    void initAudioBT();

    std::shared_ptr<Bt::Profile> currentProfile;
    std::shared_ptr<Bluetooth::SettingsHolder> settings;
    std::vector<Devicei> pairedDevices;
};
