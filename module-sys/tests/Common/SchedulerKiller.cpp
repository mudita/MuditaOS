//
// Created by mati on 25.04.19.
//

#include "SchedulerKiller.hpp"
#include "thread.hpp"
#include "timer.hpp"
#include "ticks.hpp"
#include "SystemManager/SystemManager.hpp"

SchedulerKiller::SchedulerKiller(const std::string &name) : sys::Service(name)
{
    timer_id = CreateTimer(cpp_freertos::Ticks::MsToTicks(100), true);
    ReloadTimer(timer_id);
}

void SchedulerKiller::TickHandler(uint32_t id)
{
    sys::SystemManager::CloseSystem(this);
}

sys::Message_t SchedulerKiller::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
{
    return std::make_shared<sys::ResponseMessage>();
}