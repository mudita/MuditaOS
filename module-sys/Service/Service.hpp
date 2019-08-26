#pragma once

#include "Common.hpp"
#include "Message.hpp"
#include "Mailbox.hpp"
#include "Bus.hpp"
#include <memory>
#include <queue>
#include <vector>
#include <string>
#include <algorithm>
#include <stdbool.h>
#include "thread.hpp"
#include "timer.hpp"


namespace sys
{

class ServiceTimer : public Timer
{
public:
    ServiceTimer(const std::string& name,TickType_t tick,bool isPeriodic,uint32_t  idx, Service* service);

	uint32_t GetId(){
	    return m_id;
	}

	void Run() override;


private:
    bool m_isPeriodic;
    TickType_t m_interval;
	uint32_t  m_id;
    Service* m_service;
};

class Service : public cpp_freertos::Thread,public std::enable_shared_from_this<Service>
{
public:
	Service(std::string name,uint32_t stackDepth=4096,ServicePriority priority=ServicePriority::Idle,std::string parent="");

	virtual ~Service();

	void StartService();

	//Create service timer
    uint32_t CreateTimer(TickType_t interval,bool isPeriodic,const std::string& name="Default");
	// Reload service timer
	void ReloadTimer(uint32_t id);
	// Delete timer
	void DeleteTimer(uint32_t id);
	void setTimerPeriod(uint32_t id,uint32_t period);
	/**
	 * @brief Stops a timer with specified ID
	 * @param id ID of the timer;
	 */
	void stopTimer(uint32_t id);

	// Invoked when service received data message
	virtual Message_t DataReceivedHandler(DataMessage* msg) = 0;

	// Invoked when timer ticked
	virtual void TickHandler(uint32_t id){};

	// Invoked during initialization
	virtual ReturnCodes InitHandler()= 0;

    // Invoked upon receiving close request
    virtual ReturnCodes DeinitHandler()= 0;

    // Invoked upon receiving wake up request
	virtual ReturnCodes WakeUpHandler() = 0;

	// Invoked upon receiving go sleep request
	virtual ReturnCodes SleepHandler() = 0;

	void CloseHandler();

	std::string parent;

	std::vector<BusChannels> busChannels;

	Mailbox<std::shared_ptr<Message>> mailbox;

	uint32_t pingTimestamp;

    bool isReady;

    std::vector<std::pair<uint64_t,uint32_t>> staleUniqueMsg;

protected:


    bool enableRunLoop;


//TODO R.B. I've removed this private tag to be able to create application class
//private:

	virtual void Run();

    uint32_t  m_timers_unique_idx;
	std::vector<std::unique_ptr<ServiceTimer>> timersList;

	friend class ServiceTimer;

};



}
