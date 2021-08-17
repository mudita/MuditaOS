// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <evtmgr/EventManager.hpp>

#include <service-evtmgr/WorkerEvent.hpp>

sys::ReturnCodes EventManager::InitHandler()
{
    EventManagerCommon::InitHandler();

    using namespace std::string_literals;
    std::list<sys::WorkerQueueInfo> list;
    list.emplace_back("qIrq"s, sizeof(uint8_t), 10);
    list.emplace_back("qHeadset"s, sizeof(uint8_t), 10);
    list.emplace_back("qBattery"s, sizeof(uint8_t), 10);
    list.emplace_back("qRTC"s, sizeof(uint8_t), 20);
    list.emplace_back("qSIM"s, sizeof(uint8_t), 5);
    list.emplace_back("qMagnetometer"s, sizeof(uint8_t), 5);
    list.emplace_back(WorkerEvent::MagnetometerNotifyQueue, sizeof(uint8_t), 1);
    list.emplace_back("qTorch"s, sizeof(uint8_t), 5);
    list.emplace_back("qLightSensor"s, sizeof(uint8_t), 5);
    list.emplace_back("qChargerDetect"s, sizeof(uint8_t), 5);

    EventWorker->init(list, settings);
    EventWorker->run();

    return sys::ReturnCodes::Success;
}
