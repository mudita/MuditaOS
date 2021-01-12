// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

namespace settings
{
    class TestService : public sys::Service
    {
      public:
        TestService(const std::string &name) : sys::Service(name)
        {}
        sys::ReturnCodes InitHandler() override
        {
            std::cout << "inithandler thr_id: " << std::this_thread::get_id() << std::endl << std::flush;
            return sys::ReturnCodes::Success;
        }
        sys::ReturnCodes DeinitHandler() override
        {
            std::cout << "deinithandler thr_id: " << std::this_thread::get_id() << std::endl << std::flush;
            return sys::ReturnCodes::Success;
        }
        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override
        {
            return sys::ReturnCodes::Success;
        }
        sys::MessagePointer DataReceivedHandler(sys::DataMessage *req, sys::ResponseMessage *resp) override
        {
            return std::make_shared<sys::ResponseMessage>();
        };
    };
    /* @brief AppTest handles async communication between two services (setter and getter)
        and simulates synchronous following flow:
        1. getter registers on value changes and gets the startup value of param "brightness"
        2. the setter app sets "brightness" to received in step 1 value + "1"
        3. check if the new value was received by getter
        4. unregister "brightness" on getter
        5. set "brightness" to value+"1"+"2" on setter
        6. wait 200ms and if getter does not provide value change finish
    */
    class AppTest : public TestService
    {
      public:
        enum class State : uint8_t
        {
            Unk,              // init
            Start,            // got start, send register
            Register,         // got register cnf
            RegisterStartVal, // got onchang (v), set (v+1)
            RegisterSetVal,   // got vcnf (v+1) && got onchange (v+1), send unregister
            UnregisterWait,   //
            Unregister,
            RegisterAllWait,
            RegisterAll,
            RegisterAllAdd,
            RegisterAllAddWait,
            Stop
        };
        std::shared_ptr<MyService> setter;
        std::shared_ptr<MyService> getter;
        std::shared_ptr<std::mutex> testStart;
        State state;
        std::string last_v;
        std::vector<std::string> v;

        AppTest(std::string name,
                std::shared_ptr<MyService> setter,
                std::shared_ptr<MyService> getter,
                std::shared_ptr<std::mutex> testStart)
            : TestService(std::move(name)), setter(std::move(setter)), getter(std::move(getter)),
              testStart(std::move(testStart))
        {}
        sys::ReturnCodes InitHandler() override
        {
            state = State::Unk;
            return sys::ReturnCodes::Success;
        }
        sys::ReturnCodes DeinitHandler() override
        {
            return sys::ReturnCodes::Success;
        }
        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msg, sys::ResponseMessage *resp) override
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            if (nullptr != dynamic_cast<settings::UTMsg::UTMsgStart *>(msg)) {
                testStart->lock();
                testStart->unlock();
                if (state != State::Unk) {
                    sys::SystemManager::CloseSystem(this);
                }
                else {
                    state    = State::Start;
                    auto msg = std::make_shared<settings::UTMsg::ReqRegValChg>("brightness", "none");
                    sys::Bus::SendUnicast(std::move(msg), getter->GetName(), this);
                }
            }
            else if (nullptr != dynamic_cast<settings::UTMsg::CnfRegValChg *>(msg)) {
                if (state == State::Start) {
                    state = State::Register;
                }
            }
            else if (auto m = dynamic_cast<settings::UTMsg::CnfValChg *>(msg)) {
                if (state == State::Register) {
                    state = State::RegisterStartVal;
                    v.push_back(m->value);
                    auto msg = std::make_shared<settings::UTMsg::ReqSetVal>("brightness", v[0] + "1");
                    sys::Bus::SendUnicast(std::move(msg), setter->GetName(), this);
                }
                else if (state == State::RegisterSetVal) {
                    if (m->value == v[0] + "1") {
                        v.push_back(m->value);
                        auto msg = std::make_shared<settings::UTMsg::ReqUnRegValChg>("brightness", "empty");
                        sys::Bus::SendUnicast(std::move(msg), getter->GetName(), this);
                        state = State::UnregisterWait;
                    }
                }
            }
            else if (nullptr != dynamic_cast<settings::UTMsg::CnfUnRegValChg *>(msg)) {
                if (state == State::UnregisterWait) {
                    state    = State::Unregister;
                    auto msg = std::make_shared<settings::UTMsg::ReqSetVal>("brightness", v.back() + "2");
                    sys::Bus::SendUnicast(std::move(msg), setter->GetName(), this);
                }
            }
            else if (auto m = dynamic_cast<settings::UTMsg::CnfReqSetVal *>(msg)) {
                if (state == State::RegisterStartVal) {
                    state = State::RegisterSetVal;
                }
                else if (state == State::Unregister) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));
                    v.push_back(m->value);
                    auto msg = std::make_shared<settings::UTMsg::UTMsgStop>();
                    sys::Bus::SendUnicast(std::move(msg), GetName(), this);
                }
            }
            else if (nullptr != dynamic_cast<settings::UTMsg::UTMsgStop *>(msg)) {
                if (state == State::Unregister) {
                    sys::SystemManager::CloseSystem(this);
                }
            }

            return std::make_shared<sys::ResponseMessage>();
        }
    };

    class AppTestProfileMode : public AppTest
    {
      public:
        AppTestProfileMode(const std::string &name,
                           std::shared_ptr<MyService> setter,
                           std::shared_ptr<MyService> getter,
                           std::shared_ptr<std::mutex> testStart)
            : AppTest(name, setter, getter, testStart)
        {}
        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msg, sys::ResponseMessage *resp) override
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            if (nullptr != dynamic_cast<settings::UTMsg::UTMsgStart *>(msg)) {
                testStart->lock();
                testStart->unlock();
                if (state != State::Unk) {
                    sys::SystemManager::CloseSystem(this);
                }
                else {
                    state    = State::Start;
                    auto msg = std::make_shared<settings::UTMsg::ReqRegProfileChg>();
                    sys::Bus::SendUnicast(std::move(msg), getter->GetName(), this);
                }
            }
            else if (nullptr != dynamic_cast<settings::UTMsg::CnfRegProfileChg *>(msg)) {
                if (state == State::Start) {
                    state = State::Register;
                }
            }
            else if (auto m = dynamic_cast<settings::UTMsg::ProfileChg *>(msg)) {
                if (state == State::Register) {
                    state = State::RegisterStartVal;
                    v.push_back(m->name);
                    auto msg = std::make_shared<settings::UTMsg::ReqSetCurrentProfile>(m->name + "1");
                    sys::Bus::SendUnicast(std::move(msg), setter->GetName(), this);
                }
                else if (state == State::RegisterSetVal) {
                    if (m->name == v[0] + "1") {
                        v.push_back(m->name);
                        auto msg = std::make_shared<settings::UTMsg::ReqUnRegProfileChg>();
                        sys::Bus::SendUnicast(std::move(msg), getter->GetName(), this);
                        state = State::UnregisterWait;
                    }
                }
            }
            else if (nullptr != dynamic_cast<settings::UTMsg::CnfUnRegProfileChg *>(msg)) {
                if (state == State::UnregisterWait) {
                    state    = State::Unregister;
                    auto msg = std::make_shared<settings::UTMsg::ReqSetCurrentProfile>(v.back() + "2");
                    sys::Bus::SendUnicast(std::move(msg), setter->GetName(), this);
                }
            }
            else if (auto m = dynamic_cast<settings::UTMsg::CnfSetCurrentProfile *>(msg)) {
                if (state == State::RegisterStartVal) {
                    state = State::RegisterSetVal;
                }
                else if (state == State::Unregister) {
                    v.push_back(m->name);
                    auto msg = std::make_shared<settings::UTMsg::ReqGetAllProfiles>();
                    sys::Bus::SendUnicast(std::move(msg), getter->GetName(), this);
                }
            }
            else if (nullptr != dynamic_cast<settings::UTMsg::CnfGetAllProfiles *>(msg)) {
                if (state == State::Unregister) {
                    state = State::RegisterAllWait;
                }
            }
            else if (auto m = dynamic_cast<settings::UTMsg::ProfilesChg *>(msg)) {
                if (state == State::RegisterAllWait) {
                    state = State::RegisterAll;
                    for (auto prof : m->profiles) {
                        v.push_back(prof);
                    }
                    auto msg = std::make_shared<settings::UTMsg::ReqAddProfile>("other");
                    sys::Bus::SendUnicast(std::move(msg), setter->GetName(), this);
                }
                else if (state == State::RegisterAllAddWait) {
                    state = State::RegisterAllAdd;
                    for (auto prof : m->profiles) {
                        v.push_back(prof);
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));
                    auto msg = std::make_shared<settings::UTMsg::UTMsgStop>();
                    sys::Bus::SendUnicast(std::move(msg), GetName(), this);
                }
            }
            else if (nullptr != dynamic_cast<settings::UTMsg::CnfAddProfile *>(msg)) {
                if (state == State::RegisterAll) {
                    state = State::RegisterAllAddWait;
                }
            }
            else if (nullptr != dynamic_cast<settings::UTMsg::UTMsgStop *>(msg)) {
                if (state == State::RegisterAllAdd) {
                    sys::SystemManager::CloseSystem(this);
                }
            }

            return std::make_shared<sys::ResponseMessage>();
        }
    };
} // namespace settings
