#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include <bsp/bluetooth/Bluetooth.hpp>
#include <memory>
#include <vector>

#include "profiles/BtProfile.hpp"
#include "profiles/GAP.hpp"
#include "Device.hpp"
#include "module-sys/Service/Worker.hpp"

struct HCI;

/// debug option for HCI (uart) commands debugging
// #define DO_DEBUG_HCI_COMS

namespace Bt {
    enum Message : uint8_t {
        /// asynchronous messages to use on event from irq
        EvtReceived,    /// trigger Bt stack received, start processing HCI in BluetoothWorker task
        EvtSent,        /// trigger Bt stack wrote, enable writting in HCI in BluetoothWorker task
        EvtRecUnwanted, /// not requested recieve - probably receive came to fast from sent...
        EvtRecError,    /// bsp error on receive
        EvtSentError,   /// bsp error on send
        EvtUartError,   /// generic uart error
    };

    inline const char* MessageCstr(Message what) {
        switch (what) {
            case EvtReceived:
                return "EvtReceived";
            case EvtSent:
                return "EvtSent";
            case EvtRecUnwanted:
                return "EvtRecUnwanted";
            case EvtRecError:
                return "EvtRecError";
            case EvtSentError:
                return "EvtSentError";
            case EvtUartError:
                return "EvtUartError";
            default:
                return "";
        }
    }

    struct EvtWorker {
        enum Evt : uint8_t {
        };
    };
};


class BluetoothWorker : private sys::Worker {
  enum WorkerEventQueues {
      queueService = 0,
      queueIO_handle,             /// bsp support queue
      queue_profiles,             /// queue for communication between profile workers,
                                  /// main bt_worker_task should dispatch these in events
  };

  TaskHandle_t bt_worker_task = nullptr;
public:
  enum Error {
    SuccessBt,
    ErrorBtGeneric,
    ErrorBtAPI,
  };

  BluetoothWorker(sys::Service* service);
  virtual ~BluetoothWorker();

  virtual bool handleMessage(uint32_t queueID);

  std::vector<Device> scan();

  Error aud_init();
  /// bluetooth stack id in use
  unsigned long active_features;
  // TODO store profiles, now dangling
  Stack stack;
};
