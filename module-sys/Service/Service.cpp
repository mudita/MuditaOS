#include <algorithm>
#include "Service.hpp"
#include "thread.hpp"
#include "ticks.hpp"
#include "timer.hpp"
#include "Bus.hpp"
#include "LogOutput.hpp"


namespace sys
{

using namespace cpp_freertos;
using namespace std;

Service::Service(std::string name,uint32_t stackDepth,ServicePriority priority)
		:	cpp_freertos::Thread(name, stackDepth/4 /* Stack depth in bytes */, static_cast<UBaseType_t >(priority)),
             mailbox(this),
             pingTimestamp(UINT32_MAX),
             isReady(false),
             enableRunLoop(false),
             m_timers_unique_idx(0)



{
    // System channel is mandatory for each service
    busChannels = {BusChannels::System};

}

Service::~Service()
{
    enableRunLoop = false;
    LogOutput::Output(GetName() + ":Service base destructor");
}

void Service::StartService()
{
    Bus::Add(shared_from_this());
    enableRunLoop = true;
	Start();
}


void Service::Run() {

    while (enableRunLoop) {

        auto msg = mailbox.pop();
        
        uint32_t timestamp = cpp_freertos::Ticks::GetTicks();

        // Remove all staled messages
        staleUniqueMsg.erase(std::remove_if(
                staleUniqueMsg.begin(), staleUniqueMsg.end(),
                [&](const auto& id) {
                    return ((id.first == msg->uniID) || ((timestamp-id.second) >=15000));
                }), staleUniqueMsg.end());


    	auto ret =  msg->Execute(this);

        // Unicast messages always need response with the same ID as received message
        // Don't send responses to themselves,
        // Don't send responses to responses
        if((msg->transType == Message::TransmissionType ::Unicast)
                && (msg->type != Message::Type::Response)
                && (GetName() != msg->sender))
        {
            Bus::SendResponse(ret,msg,this);
        }

    }

    Bus::Remove(shared_from_this());
};




//Create service timer
uint32_t Service::CreateTimer(uint32_t interval,bool isPeriodic,const std::string& name)
{
	timersList.push_back(std::make_unique<ServiceTimer>(name,Ticks::MsToTicks(interval),isPeriodic,m_timers_unique_idx,this));
	return m_timers_unique_idx++;
}
// Reload service timer
void Service::ReloadTimer(uint32_t id)
{
	auto it = std::find_if(timersList.begin(),timersList.end(),[&] (std::unique_ptr<ServiceTimer>const &s) { return s->GetId() == id; });
	if(it == timersList.end()){
		// not found, error
	}
	else{
		(*it)->Start(0);
	}

}
// Delete timer
void Service::DeleteTimer(uint32_t id)
{
	auto it = std::find_if(timersList.begin(),timersList.end(),[&] (std::unique_ptr<ServiceTimer>const &s) { return s->GetId() == id; });
	if(it == timersList.end()){
		// not found, error
	}
	else{
        (*it)->Stop(0);
		timersList.erase(it);
	}

}

void Service::CloseHandler(){

    // Stop currently active timers
    for(auto &w : timersList){
        w->Stop(0);
    }

    enableRunLoop = false;

};



ServiceTimer::ServiceTimer(const std::string& name,TickType_t tick,bool isPeriodic,uint32_t  idx, Service* service)
	: 	Timer(name.c_str(),tick,isPeriodic),
            m_isPeriodic(isPeriodic),
		    m_interval(tick),
            m_id(idx),
		    m_service(service)


{

}

void ServiceTimer::Run()
{
	m_service->TickHandler(m_id);
}











}
