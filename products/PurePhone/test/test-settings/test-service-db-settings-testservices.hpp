// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <memory>
#include <iostream>

namespace settings
{
    class MyService : public sys::Service
    {
      public:
        MyService(const std::string &name) : sys::Service(name)
        {}
        std::shared_ptr<settings::Settings> mySettings;
        std::vector<std::string> valChanged;
        std::string whoRequestedNotifyOnChange;
        void ValueChanged(std::string value)
        {
            valChanged.emplace_back(value);
        }
        void debug(const std::string &cmd, const std::string &k, const std::string &v)
        {
            std::cout << "[thr_id:" << std::this_thread::get_id() << ", thr_name:" << GetName() << "] " << cmd
                      << " [key, val] = (" << k << ", " << v << ")" << std::endl
                      << std::flush;
        }
        sys::MessagePointer DataReceivedHandler(sys::DataMessage *req, sys::ResponseMessage *resp) override
        {
            if (auto msg = dynamic_cast<settings::UTMsg::ReqRegValChg *>(req)) {
                debug("ReqRegValChg", msg->name, msg->value);
                whoRequestedNotifyOnChange = msg->sender;
                mySettings->registerValueChange(msg->name,
                                                ([this](std::string value) {
                                                    ValueChanged(value);
                                                    auto cnf = std::make_shared<settings::UTMsg::CnfValChg>("", value);
                                                    bus.sendUnicast(std::move(cnf), whoRequestedNotifyOnChange);
                                                }),
                                                settings::SettingsScope::Global);
                auto cnf = std::make_shared<settings::UTMsg::CnfRegValChg>(msg->name, msg->value);
                bus.sendUnicast(std::move(cnf), whoRequestedNotifyOnChange);
            }
            else if (auto msg = dynamic_cast<settings::UTMsg::ReqUnRegValChg *>(req)) {
                // unregister
                debug("ReqUnRegValChg", msg->name, msg->value);
                mySettings->unregisterValueChange(msg->name, settings::SettingsScope::Global);
                auto cnf = std::make_shared<settings::UTMsg::CnfUnRegValChg>(msg->name, msg->value);
                bus.sendUnicast(std::move(cnf), msg->sender);
            }
            else if (auto msg = dynamic_cast<settings::UTMsg::ReqSetVal *>(req)) {
                // set value
                debug("ReqSetVal", msg->name, msg->value);
                mySettings->setValue(msg->name, msg->value, settings::SettingsScope::Global);
                auto cnf = std::make_shared<settings::UTMsg::CnfReqSetVal>(msg->name, msg->value);
                bus.sendUnicast(std::move(cnf), msg->sender);
            }
            else if (dynamic_cast<settings::UTMsg::ReqGetVal *>(msg)) {
                debug("ReqGetValChg", msg->name, msg->value);
            }
            return std::make_shared<sys::ResponseMessage>();
        }
        sys::ReturnCodes InitHandler() override
        {
            std::cout << "InitHandler thr_id: " << std::this_thread::get_id() << "name: " << GetName() << std::endl
                      << std::flush;
            mySettings = std::make_shared<settings::Settings>();
            mySettings->init(service::ServiceProxy(shared_from_this()));
            return sys::ReturnCodes::Success;
        }
        sys::ReturnCodes DeinitHandler() override
        {
            mySettings->deinit();
            std::cout << "deinithandler thr_id: " << std::this_thread::get_id() << std::endl << std::flush;
            return sys::ReturnCodes::Success;
        }
        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override
        {
            return sys::ReturnCodes::Success;
        }

        std::shared_ptr<settings::Settings> getSettings()
        {
            return mySettings;
        }
    };

} // namespace settings
