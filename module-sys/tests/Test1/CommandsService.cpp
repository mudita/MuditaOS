//
// Created by mati on 26.04.19.
//

#include "CommandsService.hpp"

#include "../Common/SchedulerKiller.hpp"
#include "TestService1.hpp"
#include "DelayedService.hpp"
#include "ticks.hpp"
#include "Service/Bus.hpp"


CommandsService::CommandsService(const std::string &name):
        sys::Service(name) {

    busChannels.push_back(sys::BusChannels::CommandsServiceRequests);

    oneShotTimer = CreateTimer(cpp_freertos::Ticks::MsToTicks(1000),false);
    ReloadTimer(oneShotTimer);
}

sys::ReturnCodes CommandsService::InitHandler() {
    return sys::ReturnCodes::Success;
}

void CommandsService::TickHandler(uint32_t id) {

    auto msg = std::make_shared<CommandsServiceMsgRequest>();

    sys::Bus::SendUnicast(msg,GetName(),this);

}

sys::Message_t CommandsService::DataReceivedHandler(sys::DataMessage* msgl,sys::ResponseMessage* resp){
    uint32_t servCount = 10;

    if(resp != nullptr){
        return std::make_shared<sys::ResponseMessage>();
    }

    /*
     * Spawn 10 services and kill all of them
     */

    //Spawn 10 test services
    for(uint32_t i =0;i<servCount;i++){
        auto ret = sys::SystemManager::CreateService(std::make_shared<TestService1>("Test Service " + std::to_string(i)),this);
        REQUIRE( ret );
        REQUIRE( TestService1::TestServiceInstanceCount == i+1);
    }

    //Destroy each service one by one
   for(uint32_t i =servCount;i>0;i--){
        REQUIRE( TestService1::TestServiceInstanceCount == i);
        auto ret = sys::SystemManager::DestroyService("Test Service " + std::to_string(i-1),this);
        REQUIRE( ret );
    }

    REQUIRE( TestService1::TestServiceInstanceCount == 0);





   /*
    *
    * Create 10 services,suspend,resume,send data and finally destroy them
    *
    */

    for(uint32_t i =0;i<servCount;i++){
        auto ret = sys::SystemManager::CreateService(std::make_shared<TestService1>("Test Service " + std::to_string(i)),this);
        REQUIRE( ret );
        REQUIRE( TestService1::TestServiceInstanceCount == i+1);
    }

    REQUIRE( TestService1::TestServiceInstanceSuspendedCount == 0);


    // Send 1000 data messages to test service using blocking Unicast method
    uint32_t dataMsgCount = 1000;

    for(uint32_t i =0; i< dataMsgCount;i++){

        for(uint32_t j=0;j< servCount;j++){
            auto msg = std::make_shared<TestServiceDataMessage>(TestServiceDataMessageType::Plain);
            auto ret = sys::Bus::SendUnicast(msg,"Test Service " + std::to_string(j),this,5000);
            REQUIRE( ret.first == sys::ReturnCodes::Success );
        }
    }

    // Send 1000 data messages to test service using Multicast method

    for(uint32_t i =0; i< dataMsgCount;i++){
        auto msg = std::make_shared<TestServiceDataMessage>(TestServiceDataMessageType::Plain);
        sys::Bus::SendMulticast(msg,sys::BusChannels::TestServiceRequests,this);
    }
    cpp_freertos::Thread::Delay(50);

    REQUIRE( TestService1::TestServiceInstanceDataMsgCount == 2*servCount*dataMsgCount);

    // Messages count should be 1, this is because there is still one left message which invoked this DataReceivedHandler
    REQUIRE(sys::Message::unitestsMsgInstancesCount == 1);


    //Destroy each service one by one
    for(uint32_t i =servCount;i>0;i--){
        REQUIRE( TestService1::TestServiceInstanceCount == i);
        auto ret = sys::SystemManager::DestroyService("Test Service " + std::to_string(i-1),this);
        REQUIRE( ret );
    }

    REQUIRE( TestService1::TestServiceInstanceCount == 0);

    /*
     *
     * Unicast blocking API variants test 1
     * Sending data messages to service in blocked state
     *
     */


    {
        auto ret = sys::SystemManager::CreateService(std::make_shared<DelayedService>("DelayedService"),this);
        REQUIRE( ret );
        REQUIRE( DelayedService::DelayerServiceInstanceCount == 1);

        ret = sys::SystemManager::CreateService(std::make_shared<TestService1>("TS"),this);
        REQUIRE( ret );
        REQUIRE( TestService1::TestServiceInstanceCount == 1);

        REQUIRE( TestService1::TestServiceInstanceDataMsgCount == 0);

        sys::Bus::SendUnicast(std::make_shared<TestServiceDataMessage>(TestServiceDataMessageType::Delayed),"TS",this);

        for(uint32_t i=0; i< 10;i++){
            auto msg = std::make_shared<TestServiceDataMessage>(TestServiceDataMessageType::Plain);
            sys::Bus::SendUnicast(msg,"TS",this);
        }

        // Wait for TS to process incoming data
        cpp_freertos::Thread::Delay(200);

        REQUIRE( TestService1::TestServiceInstanceDataMsgCount == 10);

        // Messages count should be 12, this is because:
        // There is one response message (SendUnicast:145) waiting for processing
        // There are 10 response messages (for loop at :147) waiting for processing
        // There is still one left message which invoked this DataReceivedHandler
        REQUIRE(sys::Message::unitestsMsgInstancesCount == 12);


        ret = sys::SystemManager::DestroyService("DelayedService",this);
        REQUIRE( ret );
        REQUIRE( DelayedService::DelayerServiceInstanceCount == 0);

        ret = sys::SystemManager::DestroyService("TS",this);
        REQUIRE( ret );
        REQUIRE( TestService1::TestServiceInstanceCount == 0);


    }


    /*
     *
     * Unicast blocking API variants test 2
     * Destroying service in blocked state waiting for unicast response message
     *
     */


    {

        uint32_t currentMessagesInstances = sys::Message::unitestsMsgInstancesCount;

        auto ret = sys::SystemManager::CreateService(std::make_shared<DelayedService>("DelayedService"),this);
        REQUIRE( ret );
        REQUIRE( DelayedService::DelayerServiceInstanceCount == 1);

        ret = sys::SystemManager::CreateService(std::make_shared<TestService1>("TS"),this);
        REQUIRE( ret );
        REQUIRE( TestService1::TestServiceInstanceCount == 1);

        sys::Bus::SendUnicast(std::make_shared<TestServiceDataMessage>(TestServiceDataMessageType::Delayed),"TS",this);

        // There is one more pending response message (result of invoking SendUnicast at :195)
        REQUIRE(sys::Message::unitestsMsgInstancesCount == currentMessagesInstances+1 );


        ret = sys::SystemManager::DestroyService("TS",this);
        REQUIRE( ret );
        REQUIRE( TestService1::TestServiceInstanceCount == 0);

        ret = sys::SystemManager::DestroyService("DelayedService",this);
        REQUIRE( ret );
        REQUIRE( DelayedService::DelayerServiceInstanceCount == 0);


    }

    /*
     *
     * Unicast blocking API variants test 3
     * Testing timeout + droping staled response messages
     *
     */


    {

        uint32_t currentMessagesInstances = sys::Message::unitestsMsgInstancesCount;

        auto ret = sys::SystemManager::CreateService(std::make_shared<DelayedService>("DelayedService"),this);
        REQUIRE( ret );
        REQUIRE( DelayedService::DelayerServiceInstanceCount == 1);

        ret = sys::SystemManager::CreateService(std::make_shared<TestService1>("TS"),this);
        REQUIRE( ret );
        REQUIRE( TestService1::TestServiceInstanceCount == 1);

        {
            auto retFalse = sys::Bus::SendUnicast(
                    std::make_shared<TestServiceDataMessage>(TestServiceDataMessageType::DelayedFail), "TS", this,
                    1000);

            auto resp = std::static_pointer_cast<TestServiceResponse>(retFalse.second);

            REQUIRE(resp->ret == sys::ReturnCodes::Timeout);

        }

        cpp_freertos::Thread::Delay(200);

        REQUIRE(sys::Message::unitestsMsgInstancesCount == currentMessagesInstances );


        ret = sys::SystemManager::DestroyService("TS",this);
        REQUIRE( ret );
        REQUIRE( TestService1::TestServiceInstanceCount == 0);

        ret = sys::SystemManager::DestroyService("DelayedService",this);
        REQUIRE( ret );
        REQUIRE( DelayedService::DelayerServiceInstanceCount == 0);


    }


    /*
     *
     * Send data messages: many to many variant
     *
     */
    {
        uint32_t currentMessagesInstances = sys::Message::unitestsMsgInstancesCount;

        // Spawn 10 test services
        for(uint32_t i =0;i<servCount;i++){
            auto ret = sys::SystemManager::CreateService(std::make_shared<TestService1>(std::to_string(i)),this);
            REQUIRE( ret );
            REQUIRE( TestService1::TestServiceInstanceCount == i+1);
        }

        sys::Bus::SendMulticast(std::make_shared<TestServiceDataMessage>(TestServiceDataMessageType::SendData),sys::BusChannels::TestServiceRequests,this);


        cpp_freertos::Thread::Delay(500);

        REQUIRE( TestService1::TestServiceInstanceDataMsgCount == servCount*1000);


        //Destroy each service one by one
        for(uint32_t i =servCount;i>0;i--){
            REQUIRE( TestService1::TestServiceInstanceCount == i);
            auto ret = sys::SystemManager::DestroyService(std::to_string(i-1),this);
            REQUIRE( ret );
        }

        REQUIRE( TestService1::TestServiceInstanceCount == 0);

        REQUIRE(sys::Message::unitestsMsgInstancesCount == currentMessagesInstances );


    }





    /*
     *
     * Test system close
     *
     */



    for(uint32_t i =0;i<servCount;i++){
        auto ret = sys::SystemManager::CreateService(std::make_shared<TestService1>("Test Service " + std::to_string(i)),this);
        REQUIRE( ret );
        REQUIRE( TestService1::TestServiceInstanceCount == i+1);
    }

    auto ret = sys::SystemManager::CreateService(std::make_shared<SchedulerKiller>("Scheduler killer"),this);
    REQUIRE( ret );

    return std::make_shared<sys::ResponseMessage>();
}