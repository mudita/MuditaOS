// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include "messages/SettingsMessages.hpp"

#include <Service/Service.hpp>
#include <Service/Message.hpp>

#include <functional>
#include <module-sys/Service/Bus.hpp>

namespace Settings
{
    class Service : public sys::Service
    {
      public:
        using sys::Service::Service;

        auto getValue(Settings::EntryPath path) -> std::optional<std::string>
        {
            return "";
        }

        std::string setValue(Settings::EntryPath path, std::string value)
        {
            // insert into ...
            return value;
        }

        sys::Message_t DataReceivedHandler(sys::DataMessage *msg, sys::ResponseMessage *resp)
        {
            return std::make_shared<sys::ResponseMessage>();
        }

        sys::ReturnCodes InitHandler()
        {
            using std::placeholders::_1;
            using std::placeholders::_2;

            connect(Settings::Messages::GetVariable(), std::bind(&Service::handleGetVariable, this, _1, _2));
            connect(Settings::Messages::SetVariable(), std::bind(&Service::handleSetVariable, this, _1, _2));

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

        sys::Message_t handleGetVariable(sys::DataMessage *req, sys::ResponseMessage * /*response*/)
        {
            if (auto msg = dynamic_cast<Settings::Messages::GetVariable *>(req)) {

                auto path  = msg->getPath();
                auto value = getValue(path);

                return std::make_shared<Settings::Messages::VariableResponse>(path, value);
            }
            return std::make_shared<sys::ResponseMessage>();
        };

        sys::Message_t handleSetVariable(sys::DataMessage *req, sys::ResponseMessage * /*response*/)
        {
            if (auto msg = dynamic_cast<Settings::Messages::SetVariable *>(req)) {

                auto path      = msg->getPath();
                auto value     = msg->getValue().value_or("");
                auto old_value = setValue(path, msg->getValue().value_or(""));

                auto update_msg = std::make_shared<Settings::Messages::VariableChanged>(path, value, old_value);
                sys::Bus::SendUnicast(std::move(update_msg), "db-worker", this);
            }
            return std::make_shared<sys::ResponseMessage>();
        };

        sys::Message_t handleListProfiles(sys::DataMessage *req, sys::ResponseMessage * /*response*/)
        {
            if (dynamic_cast<Settings::Messages::ListProfiles *>(req) != nullptr) {
                std::list<std::string> profiles = {"silent", "loud"};
                return std::make_shared<Settings::Messages::ProfileListResponse>(profiles);
            }
            return std::make_shared<sys::ResponseMessage>();
        };

        sys::Message_t handleListModes(sys::DataMessage *req, sys::ResponseMessage * /*response*/)
        {
            if (dynamic_cast<Settings::Messages::ListProfiles *>(req) != nullptr) {
                std::list<std::string> modes = {"mode1", "mode2"};
                return std::make_shared<Settings::Messages::ModeListResponse>(modes);
            }
            return std::make_shared<sys::ResponseMessage>();
        };
    };
} // namespace Settings

TEST_CASE("Settings Messages")
{
    SECTION("Init handler")
    {
        Settings::Service settings("settings");
        settings.InitHandler();
    }

    SECTION("Send register messages")
    {
        Settings::Service settings("settings");
        settings.InitHandler();

        sys::Bus::SendUnicast(std::make_shared<Settings::Messages::RegisterOnVariableChange>(
                                  Settings::EntryPath({"mode", "service", "profile", "variable"})),
                              "db-worker",
                              &settings);

        sys::Bus::SendUnicast(std::make_shared<Settings::Messages::UnregisterOnVariableChange>(
                                  Settings::EntryPath({"mode", "service", "profile", "variable"})),
                              "db-worker",
                              &settings);
    }

    SECTION("Send profile messages")
    {
        Settings::Service settings("settings");
        settings.InitHandler();

        sys::Bus::SendUnicast(std::make_shared<Settings::Messages::ListProfiles>(), "settings", &settings);

        sys::Bus::SendUnicast(std::make_shared<Settings::Messages::AddProfile>("new-profile"), "settings", &settings);

        sys::Bus::SendUnicast(std::make_shared<Settings::Messages::GetCurrentProfile>(), "settings", &settings);

        sys::Bus::SendUnicast(
            std::make_shared<Settings::Messages::CurrentProfileChanged>("profile"), "settings", &settings);
    }

    SECTION("Send mode messages")
    {
        Settings::Service settings("settings");
        settings.InitHandler();

        sys::Bus::SendUnicast(std::make_shared<Settings::Messages::ListModes>(), "settings", &settings);

        sys::Bus::SendUnicast(std::make_shared<Settings::Messages::GetCurrentMode>(), "settings", &settings);

        sys::Bus::SendUnicast(std::make_shared<Settings::Messages::AddMode>("new-mode"), "settings", &settings);

        sys::Bus::SendUnicast(std::make_shared<Settings::Messages::CurrentModeChanged>("mode"), "settings", &settings);
    }
}
