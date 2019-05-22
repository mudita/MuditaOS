//
// Created by mati on 11.03.19.
//

#ifndef DBUS_TESTSERVICE1_H
#define DBUS_TESTSERVICE1_H

#include "catch.hpp"
#include "Service/Service.hpp"
#include "Service/Message.hpp"
#include "Service/Common.hpp"
#include "Service/LogOutput.hpp"
#include "../../SystemManager/SystemManager.hpp"


enum class TestServiceDataMessageType{
    Plain,
    Delayed,
    DelayedFail,
    SendData
};


class TestServiceResponse : public sys::ResponseMessage{
public:
    TestServiceResponse(sys::ReturnCodes ret):
        ResponseMessage(),
        ret(ret){

    }
    sys::ReturnCodes ret;
};


class TestServiceDataMessage : public sys::DataMessage{
public:

    TestServiceDataMessage(TestServiceDataMessageType type):
        DataMessage(),
        type(type){

    }


    TestServiceDataMessageType type;
};

class TestService1 : public sys::Service {

public:
    TestService1(const std::string& name)
        : sys::Service(name)
    {
        busChannels.push_back(sys::BusChannels::TestServiceRequests);
    }

    ~TestService1(){
    }

    // Invoked when service received data message
    sys::Message_t DataReceivedHandler(sys::DataMessage* msgl) override{

        TestServiceDataMessage* msg = static_cast<TestServiceDataMessage*>(msgl);

        sys::ReturnCodes ret = sys::ReturnCodes ::Success;

        switch (msg->type){

            case TestServiceDataMessageType ::Delayed:
            {
                auto msg = std::make_shared<sys::DataMessage>();
                auto retSend = sys::Bus::SendUnicast(msg,"DelayedService",this,5000);
                REQUIRE( retSend.first == sys::ReturnCodes::Success );
                ret = retSend.first;
            }
                break;

            case TestServiceDataMessageType ::DelayedFail:
            {
                auto msg = std::make_shared<sys::DataMessage>();
                auto retSend = sys::Bus::SendUnicast(msg,"DelayedService",this,50);
                REQUIRE( retSend.first == sys::ReturnCodes::Timeout );
                ret = retSend.first;
            }
                break;

            case TestServiceDataMessageType ::Plain:
                TestServiceInstanceDataMsgCount++;
                break;

            case TestServiceDataMessageType ::SendData:
            {
                if(std::stoi(GetName()) == 9 ){

                    for(uint32_t i=0;i<1000;i++){
                        auto msg = std::make_shared<TestServiceDataMessage>(TestServiceDataMessageType::Plain);
                        sys::Bus::SendUnicast(msg,"0",this);
                    }
                }
                else{
                    for(uint32_t i=0;i<1000;i++){
                        auto msg = std::make_shared<TestServiceDataMessage>(TestServiceDataMessageType::Plain);
                        sys::Bus::SendUnicast(msg,std::to_string(std::stoi(GetName())+1),this);
                    }
                }
            }

                break;



        }

        return std::make_shared<TestServiceResponse>(ret);
    }

    // Invoked when timer ticked
    void TickHandler(uint32_t id) override{
    }

    // Invoked during initialization
    sys::ReturnCodes InitHandler() override{
        TestServiceInstanceCount++;
        return sys::ReturnCodes::Success;

    }

    sys::ReturnCodes DeinitHandler() override{
        TestServiceInstanceCount--;
        if(TestServiceInstanceCount == 0){
            TestServiceInstanceDataMsgCount=0;
        }
        return sys::ReturnCodes::Success;
    }

    sys::ReturnCodes WakeUpHandler() override{
        TestServiceInstanceSuspendedCount--;
        return sys::ReturnCodes::Success;
    }


    sys::ReturnCodes SleepHandler() override{
        TestServiceInstanceSuspendedCount++;
        return sys::ReturnCodes::Success;
    }


    static uint32_t TestServiceInstanceCount;
    static uint32_t TestServiceInstanceSuspendedCount;
    static uint32_t TestServiceInstanceDataMsgCount;

};


#endif //DBUS_TESTSERVICE1_H
