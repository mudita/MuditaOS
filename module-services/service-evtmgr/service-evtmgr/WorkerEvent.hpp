﻿// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "EventManager.hpp"

#include <Service/Message.hpp>
#include <Service/Service.hpp>
#include <Service/Worker.hpp>
#include <bsp/common.hpp>
#include <bsp/keyboard/key_codes.hpp>
#include <bsp/keyboard/key_codes.hpp>

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
    queueHeadsetIRQ,
    queueBattery,
    queueRTC,
    queueCellular,
    queueMagnetometerIRQ,
    queueTorch,
    queueLightSensor
};

class WorkerEvent : public sys::Worker
{
  private:
    /**
     * @brief This method is responsible for catch and process keyboard event.
     * @param event key event
     * @param code key code
     * @note It sends message to service if event is processed successfully.
     */
    void processKeyEvent(bsp::KeyEvents event, bsp::KeyCodes code);
    /**
     * @brief list of keys with long press enabled. First item is key code, second is long press time.
     */
    std::map<uint32_t, uint32_t> longPressParamsList;
    bool longPressTaskEnabled = false;
    bsp::KeyEvents lastState  = bsp::KeyEvents::Released;
    bsp::KeyCodes lastPressed = static_cast<bsp::KeyCodes>(0);
    sys::Service *service     = nullptr;

  public:
    WorkerEvent(sys::Service *service);
    /**
     * This function is responsible for creating all queues provided in the constructor.
     * When all queues are created this method creates set of queues.
     */
    virtual bool init(std::list<sys::WorkerQueueInfo> queuesList) override;
    virtual bool deinit() override;

    /**
     * This method is called from thread when new message arrives in queue.
     * @param queueID Index of the queue in the queues vector.
     */
    bool handleMessage(uint32_t queueID) override final;
};
