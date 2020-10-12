#include <catch2/catch.hpp>
#include "agents/settings/Settings.hpp"
#include "messages/SettingsMessages.hpp"
#include <Service/Service.hpp>
#include <functional>

namespace Settings
{
    class MyService : public sys::Service
    {
      public:
        MyService(const std::string &name) : sys::Service(name)
        {}
        using MapStr = std::map<std::string, std::string>;
        MapStr vars;
        sys::Message_t handGetVar(sys::DataMessage *req, sys::ResponseMessage * /*response*/)
        {
            if (auto msg = dynamic_cast<::Settings::Messages::GetVariable *>(req)) {

                auto path = msg->getPath();
                if (vars.end() != vars.find(path.variable)) {
                    return std::make_shared<::Settings::Messages::VariableResponse>(path, vars[path.variable]);
                }

                return std::make_shared<::Settings::Messages::VariableResponse>(path, "");
            }
            return std::make_shared<sys::ResponseMessage>();
        };

        sys::Message_t handSetVar(sys::DataMessage *req, sys::ResponseMessage * /*response*/)
        {
            if (auto msg = dynamic_cast<::Settings::Messages::SetVariable *>(req)) {

                auto path      = msg->getPath();
                auto value     = msg->getValue().value_or("");
                auto old_value = setVal(path, msg->getValue().value_or(""));

                auto update_msg = std::make_shared<::Settings::Messages::VariableChanged>(path, value, old_value);
            }
            return std::make_shared<sys::ResponseMessage>();
        };
        std::string setVal(::Settings::EntryPath path, std::string value)
        {
            // insert into ...
            vars[path.variable] = value;
            return value;
        }

        sys::Message_t DataReceivedHandler(sys::DataMessage *msg, sys::ResponseMessage *resp)
        {
            return sys::Message_t();
        }
        sys::ReturnCodes InitHandler()
        {
            using std::placeholders::_1;
            using std::placeholders::_2;

            connect(::Settings::Messages::GetVariable(), std::bind(&MyService::handGetVar, this, _1, _2));
            connect(::Settings::Messages::SetVariable(), std::bind(&MyService::handSetVar, this, _1, _2));
            return sys::ReturnCodes::Success;
        }
        sys::ReturnCodes DeinitHandler()
        {
            return sys::ReturnCodes::Success;
        }
        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode)
        {
            return sys::ReturnCodes::Success;
        }
        void ProcessMsgs()
        {
            isReady = true;
            while (1) {
                auto msg = mailbox.pop(0);
                if (msg == nullptr) {
                    break;
                }
                auto ret = msg->Execute(this);
            }
        }
    };
}; // namespace Settings
TEST_CASE("SettingsApi")
{
    SECTION("Create APP")
    {
        Settings::MyService theApp("theApp");
        Settings::Settings setapi(&theApp);
    }

    SECTION("set variable")
    {
        Settings::MyService theApp("theApp");
        theApp.InitHandler();
        Settings::Settings setapi(&theApp, "theApp");
        setapi.setValue("glosnosc", "5");
        theApp.ProcessMsgs();
        REQUIRE(1 == theApp.vars.size());
        REQUIRE(theApp.vars.end() != theApp.vars.find("glosnosc"));
        REQUIRE("5" == theApp.vars["glosnosc"]);
    }
}
