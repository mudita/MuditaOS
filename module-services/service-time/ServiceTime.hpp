/*
 *  @file ServiceTime.hpp
 *  @author
 *  @date 08.05.20
 *  @brief
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

#ifndef PUREPHONE_SERVICETIME_HPP
#define PUREPHONE_SERVICETIME_HPP

#include "Service/Service.hpp"
#include "ServiceTime.hpp"
#include <functional>
#include "MessageType.hpp"

#include <module-db/queries/calendar/QueryEventsGet.hpp>
#include <module-db/queries/calendar/QueryEventsEdit.hpp>
#include <module-sys/Service/Timer.hpp>

namespace service::name
{
    const inline std::string service_time = "ServiceTime";
};

class ServiceTime;

class TimeEvents
{
  protected:
    uint32_t timerId = 0;
    uint32_t lastDT  = 0;
    bool timerActive = false;

    void StartTimer();
    // auto id    = CreateTimer(interval, isPeriodic, name);

  public:
    TimeEvents();
    ~TimeEvents();

    virtual sys::Message_t OnDataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp = nullptr);
    virtual void OnTickHandler(uint32_t id);

    virtual bool Reload();
};

//************************************************************************************************

class CalendarTimeEvents : public TimeEvents
{
  private:
    uint32_t todayEventsNum = 0;
    EventsRecord eventRecord;

  protected:
    void DeleteTimer();
    void FireReminder();

  public:
    CalendarTimeEvents();
    ~CalendarTimeEvents();

    sys::Message_t OnDataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp = nullptr);
    void OnTickHandler(uint32_t id);

    bool Reload() override;
};

//************************************************************************************************
//************************************************************************************************
//************************************************************************************************

class ServiceTime : public sys::Service
{
  private:
    CalendarTimeEvents calendarEvents;

    uint32_t todayEventsNum = 0;
    EventsRecord eventRecord;
    TimePoint startTP = TIME_POINT_INVALID;
    uint32_t timerId  = 0;

    std::unique_ptr<sys::Timer> calendarReminderTimer = nullptr;

    bool timersProcessingStarted = false;

  protected:
    void SendReloadQuery();
    void ReceiveReloadQuery(std::unique_ptr<std::vector<EventsRecord>> records);

    void DestroyTimer();
    void RecreateTimer();
    void InvokeReminder();
    void SendReminderFiredQuery();

    // mlucki
    void SendFilterQuery();
    void ReceiveFilterQuery();

  public:
    ServiceTime();

    ~ServiceTime();

    sys::Message_t DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp = nullptr) override;

    // Invoked when timer ticked
    // void TickHandler(uint32_t id) override final;
    void reminderTimerCallback();

    // Invoked during initialization
    sys::ReturnCodes InitHandler() override;

    sys::ReturnCodes DeinitHandler() override;

    sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final;

    static bool messageReloadTimers(sys::Service *sender);
    static bool messageTimersProcessingStart(sys::Service *sender);
    static bool messageTimersProcessingStop(sys::Service *sender);

    static const char *serviceName;
};

#endif // PUREPHONE_SERVICETIME_HPP
