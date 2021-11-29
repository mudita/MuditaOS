// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "EventManagerCommon.hpp"

#include <Service/Message.hpp>
#include <Service/Service.hpp>
#include <Service/Worker.hpp>
#include <bsp/common.hpp>
#include <SystemManager/CpuSentinel.hpp>
#include <hal/battery_charger/AbstractBatteryCharger.hpp>
#include <hal/key_input/AbstractKeyInput.hpp>
#include <battery/BatteryController.hpp>

#include <cstdint>
#include <list>
#include <map>

namespace sys
{
    class Service;
} // namespace sys

struct KeyState
{
    uint8_t event;
    uint8_t code;
};

enum class WorkerEventQueues
{
    queueService = 0,
    queueControl = 1,
    queueKeyboardIRQ,
    queueBattery,
    queueChargerDetect,
    queueRTC,
};

class WorkerEventCommon : public sys::Worker
{
  protected:
    virtual void addProductQueues(std::list<sys::WorkerQueueInfo> &queueList);
    virtual void initProductHardware();
    virtual void deinitProductHardware();

    virtual void processKeyEvent(bsp::KeyEvents event, bsp::KeyCodes code);

    sys::Service *service = nullptr;

  private:
    static constexpr auto stackDepthBytes = 3072;

    static constexpr auto keyboardQueueName = "qIrq";
    static constexpr auto batteryQueueName  = "qBattery";
    static constexpr auto chargerQueueName  = "qCharger";
    static constexpr auto rtcQueueName      = "qRTC";

    static constexpr auto keyboardQueueSize = 10;
    static constexpr auto batteryQueueSize  = 10;
    static constexpr auto chargerQueueSize  = 5;
    static constexpr auto rtcQueueSize      = 20;

    /**
     * @brief This method is responsible for catch and process keyboard event.
     * @param event key event
     * @param code key code
     * @note It sends message to service if event is processed successfully.
     */
    void updateResourcesAfterCpuFrequencyChange(bsp::CpuFrequencyMHz newFrequency);
    bool initEventQueues();
    bool initCommonHardwareComponents();

    /**
     * @brief list of keys with long press enabled. First item is key code, second is long press time.
     */
    std::map<uint32_t, uint32_t> longPressParamsList;
    bool longPressTaskEnabled = false;
    bsp::KeyEvents lastState  = bsp::KeyEvents::Released;
    bsp::KeyCodes lastPressed = static_cast<bsp::KeyCodes>(0);
    std::shared_ptr<sys::CpuSentinel> cpuSentinel;
    std::shared_ptr<hal::key_input::AbstractKeyInput> keyInput;
    std::shared_ptr<sevm::battery::BatteryController> batteryController;

  public:
    explicit WorkerEventCommon(sys::Service *service);

    void init(std::shared_ptr<settings::Settings> settings);
    virtual bool deinit() override;

    /**
     * This method is called from thread when new message arrives in queue.
     * @param queueID Index of the queue in the queues vector.
     */
    bool handleMessage(uint32_t queueID) override;
};
