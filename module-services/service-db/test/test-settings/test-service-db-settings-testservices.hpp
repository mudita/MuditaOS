// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

namespace settings
{
    class MyService : public sys::Service
    {
      public:
        MyService(const std::string &name) : sys::Service(name)
        {
            mySettings = std::make_shared<settings::Settings>(this);
        }
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
                mySettings->registerValueChange(msg->name, ([this](std::string value) {
                                                    ValueChanged(value);
                                                    auto cnf = std::make_shared<settings::UTMsg::CnfValChg>("", value);
                                                    sys::Bus::SendUnicast(
                                                        std::move(cnf), whoRequestedNotifyOnChange, this);
                                                }));
                auto cnf = std::make_shared<settings::UTMsg::CnfRegValChg>(msg->name, msg->value);
                sys::Bus::SendUnicast(std::move(cnf), whoRequestedNotifyOnChange, this);
            }
            else if (auto msg = dynamic_cast<settings::UTMsg::ReqUnRegValChg *>(req)) {
                // unregister
                debug("ReqUnRegValChg", msg->name, msg->value);
                mySettings->unregisterValueChange(msg->name);
                auto cnf = std::make_shared<settings::UTMsg::CnfUnRegValChg>(msg->name, msg->value);
                sys::Bus::SendUnicast(std::move(cnf), msg->sender, this);
            }
            else if (auto msg = dynamic_cast<settings::UTMsg::ReqSetVal *>(req)) {
                // set value
                debug("ReqSetVal", msg->name, msg->value);
                mySettings->setValue(msg->name, msg->value);
                auto cnf = std::make_shared<settings::UTMsg::CnfReqSetVal>(msg->name, msg->value);
                sys::Bus::SendUnicast(std::move(cnf), msg->sender, this);
            }
            else if (dynamic_cast<settings::UTMsg::ReqGetVal *>(msg)) {
                debug("ReqGetValChg", msg->name, msg->value);
            }
            return std::make_shared<sys::ResponseMessage>();
        }
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
    };

    class ServiceProfile : public MyService
    {
      public:
        ServiceProfile(const std::string &name) : MyService(name)
        {}
        settings::Settings::ListOfProfiles profiles;
        std::string profile;
        sys::MessagePointer DataReceivedHandler(sys::DataMessage *req, sys::ResponseMessage *resp) override
        {
            if (auto msg = dynamic_cast<settings::UTMsg::ReqRegProfileChg *>(req)) {
                debug("ReqRegProfileChg", msg->name, msg->value);
                whoRequestedNotifyOnChange = msg->sender;
                mySettings->registerProfileChange(([this](const std::string &profile) {
                    this->profile = profile;
                    auto cnf      = std::make_shared<settings::UTMsg::ProfileChg>(profile);
                    sys::Bus::SendUnicast(std::move(cnf), whoRequestedNotifyOnChange, this);
                }));
                auto cnf = std::make_shared<settings::UTMsg::CnfRegProfileChg>();
                sys::Bus::SendUnicast(std::move(cnf), whoRequestedNotifyOnChange, this);
            }
            else if (auto msg = dynamic_cast<settings::UTMsg::ReqUnRegProfileChg *>(req)) {
                // unregister
                debug("ReqUnRegProfileChg", msg->name, msg->value);
                mySettings->unregisterProfileChange();
                auto cnf = std::make_shared<settings::UTMsg::CnfUnRegProfileChg>();
                sys::Bus::SendUnicast(std::move(cnf), msg->sender, this);
            }
            else if (auto msg = dynamic_cast<settings::UTMsg::ReqSetCurrentProfile *>(req)) {
                // set value
                debug("ReqSetCurrentProfile", msg->name, msg->value);
                mySettings->setCurrentProfile(msg->name);
                auto cnf = std::make_shared<settings::UTMsg::CnfSetCurrentProfile>(msg->name);
                sys::Bus::SendUnicast(std::move(cnf), msg->sender, this);
            }
            else if (auto msg = dynamic_cast<settings::UTMsg::ReqGetAllProfiles *>(req)) {
                debug("ReqGetAllProfiles", msg->name, msg->value);
                mySettings->getAllProfiles(([this](const settings::Settings::ListOfProfiles &profiles) {
                    this->profiles = profiles;
                    auto cnf       = std::make_shared<settings::UTMsg::ProfilesChg>(profiles);
                    sys::Bus::SendUnicast(std::move(cnf), whoRequestedNotifyOnChange, this);
                }));
                auto cnf = std::make_shared<settings::UTMsg::CnfGetAllProfiles>();
                sys::Bus::SendUnicast(std::move(cnf), msg->sender, this);
            }
            else if (auto msg = dynamic_cast<settings::UTMsg::ReqAddProfile *>(req)) {
                debug("ReqAddProfile", msg->name, msg->value);
                mySettings->addProfile(msg->name);
                auto cnf = std::make_shared<settings::UTMsg::CnfAddProfile>(msg->name);
                sys::Bus::SendUnicast(std::move(cnf), msg->sender, this);
            }

            return std::make_shared<sys::ResponseMessage>();
        }
    };

    class ServiceMode : public MyService
    {
      public:
        ServiceMode(const std::string &name) : MyService(name)
        {}
        settings::Settings::ListOfModes modes;
        std::string mode;
        sys::MessagePointer DataReceivedHandler(sys::DataMessage *req, sys::ResponseMessage *resp) override
        {
            if (auto msg = dynamic_cast<settings::UTMsg::ReqRegProfileChg *>(req)) {
                debug("ReqRegProfileChg", msg->name, msg->value);
                whoRequestedNotifyOnChange = msg->sender;
                mySettings->registerModeChange(([this](const std::string &mode) {
                    this->mode = mode;
                    auto cnf   = std::make_shared<settings::UTMsg::ProfileChg>(mode);
                    sys::Bus::SendUnicast(std::move(cnf), whoRequestedNotifyOnChange, this);
                }));
                auto cnf = std::make_shared<settings::UTMsg::CnfRegProfileChg>();
                sys::Bus::SendUnicast(std::move(cnf), whoRequestedNotifyOnChange, this);
            }
            else if (auto msg = dynamic_cast<settings::UTMsg::ReqUnRegProfileChg *>(req)) {
                // unregister
                debug("ReqUnRegProfileChg", msg->name, msg->value);
                mySettings->unregisterModeChange();
                auto cnf = std::make_shared<settings::UTMsg::CnfUnRegProfileChg>();
                sys::Bus::SendUnicast(std::move(cnf), msg->sender, this);
            }
            else if (auto msg = dynamic_cast<settings::UTMsg::ReqSetCurrentProfile *>(req)) {
                // set value
                debug("ReqSetCurrentProfile", msg->name, msg->value);
                mySettings->setCurrentMode(msg->name);
                auto cnf = std::make_shared<settings::UTMsg::CnfSetCurrentProfile>(msg->name);
                sys::Bus::SendUnicast(std::move(cnf), msg->sender, this);
            }
            else if (auto msg = dynamic_cast<settings::UTMsg::ReqGetAllProfiles *>(req)) {
                debug("ReqGetAllProfiles", msg->name, msg->value);
                mySettings->getAllModes(([this](const settings::Settings::ListOfModes &modes) {
                    this->modes = modes;
                    auto cnf    = std::make_shared<settings::UTMsg::ProfilesChg>(modes);
                    sys::Bus::SendUnicast(std::move(cnf), whoRequestedNotifyOnChange, this);
                }));
                auto cnf = std::make_shared<settings::UTMsg::CnfGetAllProfiles>();
                sys::Bus::SendUnicast(std::move(cnf), msg->sender, this);
            }
            else if (auto msg = dynamic_cast<settings::UTMsg::ReqAddProfile *>(req)) {
                debug("ReqAddProfile", msg->name, msg->value);
                mySettings->addMode(msg->name);
                auto cnf = std::make_shared<settings::UTMsg::CnfAddProfile>(msg->name);
                sys::Bus::SendUnicast(std::move(cnf), msg->sender, this);
            }

            return std::make_shared<sys::ResponseMessage>();
        }
    };
} // namespace settings
