/*
 * Service.hpp
 *
 *  Created on: Mar 7, 2019
 *      Author: mati
 */

#ifndef SYSTEMMANAGER_SYSTEMMANAGER_HPP_
#define SYSTEMMANAGER_SYSTEMMANAGER_HPP_

#include <functional>
#include "thread.hpp"
#include "timer.hpp"
#include "condition_variable.hpp"
#include "mutex.hpp"
#include "Mailbox.hpp"
#include "Service/Bus.hpp"
#include "Service/Service.hpp"
#include "Service/Message.hpp"

namespace sys
{

    enum class SystemManagerMsgType{
        CloseSystem,
    };

    class SystemManagerMsg : public DataMessage{
    public:

        SystemManagerMsg(SystemManagerMsgType type):
                DataMessage(BusChannels::SystemManagerRequests),
                type(type){}


        SystemManagerMsgType type;
    };


class SystemManager : public Service
{
public:
	SystemManager(TickType_t pingInterval);

	~SystemManager();

	void StartSystem();

	// Invoke system close procedure
	static bool CloseSystem(Service* s);

	// Create new service
	static bool CreateService(std::shared_ptr<Service> service,Service* s,TickType_t timeout=5000);

	// Destroy existing service
	static bool DestroyService(const std::string& name,Service* s,TickType_t timeout=5000);

	// Suspend specified serivice
	static bool SuspendService(const std::string& name,Service* s,TickType_t timeout=5000);

	// Resume specified service
	static bool ResumeService(const std::string& name,Service* s,TickType_t timeout=5000);

	// Register init procedure which will be invoked upon system start
	void RegisterInitFunction(std::function<int(SystemManager* sysmgr)> init);

private:

	void TickHandler(uint32_t id) override;

    Message_t DataReceivedHandler(DataMessage* msg) override;

    ReturnCodes InitHandler() override;

    ReturnCodes WakeUpHandler() override{return ReturnCodes::Success;}

    ReturnCodes SleepHandler() override{return ReturnCodes::Success;}

    ReturnCodes DeinitHandler() override{return ReturnCodes::Success;}




    void Run() override;

    void CloseSystemHandler();


    TickType_t pingInterval;
	uint32_t    pingPongTimerID;

    std::function<int(SystemManager* sysmgr)> userInit;

	static std::vector<std::shared_ptr<Service>> servicesList;


};




}

#endif /* SYSTEMMANAGER_SYSTEMMANAGER_HPP_ */
