// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>           // for Section, SourceLineInfo, SECTION, SectionInfo, StringRef, TEST_CASE
#include <Service/Service.hpp>        // for Service
#include <Service/Message.hpp>        // for MessagePointer, ResponseMessage, DataMessage
#include <module-sys/Service/Bus.hpp> // for Bus
#include <functional>                 // for _Bind_helper<>::type, _Placeholder, bind, _1, _2
#include <list>                       // for list
#include <memory>                     // for make_shared, allocator, shared_ptr
#include <optional>                   // for optional
#include <string>                     // for string
#include <utility>                    // for move

#include <service-db/SettingsMessages.hpp> // for SetVariable, ListProfiles, GetVariable, AddMode, AddProfile, CurrentModeChanged, CurrentProfileChanged, EntryPath, GetCurrentMode, GetCurrentProfile, ListModes, ModeListResponse, ProfileListResponse, RegisterOnVariableChange, UnregisterOnVariableChange, VariableChanged, VariableResponse
#include "Service/Common.hpp"            // for ReturnCodes, ReturnCodes::Success, ServicePowerMode

namespace settings
{
    class Service : public sys::Service
    {
      public:
        using sys::Service::Service;

        auto getValue(settings::EntryPath path) -> std::optional<std::string>
        {
            return "";
        }

        std::string setValue(settings::EntryPath path, std::string value)
        {
            // insert into ...
            return value;
        }

        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msg, sys::ResponseMessage *resp)
        {
            return std::make_shared<sys::ResponseMessage>();
        }

        sys::ReturnCodes InitHandler()
        {
            using std::placeholders::_1;
            using std::placeholders::_2;

            connect(settings::Messages::GetVariable(), std::bind(&Service::handleGetVariable, this, _1));
            connect(settings::Messages::SetVariable(), std::bind(&Service::handleSetVariable, this, _1));

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

        sys::MessagePointer handleGetVariable(sys::Message *req)
        {
            if (auto msg = dynamic_cast<settings::Messages::GetVariable *>(req)) {

                auto path  = msg->getPath();
                auto value = getValue(path);

                return std::make_shared<settings::Messages::VariableResponse>(path, value);
            }
            return std::make_shared<sys::ResponseMessage>();
        };

        sys::MessagePointer handleSetVariable(sys::Message *req)
        {
            if (auto msg = dynamic_cast<settings::Messages::SetVariable *>(req)) {

                auto path      = msg->getPath();
                auto value     = msg->getValue().value_or("");
                auto old_value = setValue(path, msg->getValue().value_or(""));

                auto update_msg = std::make_shared<settings::Messages::VariableChanged>(path, value, old_value);
                sys::Bus::SendUnicast(std::move(update_msg), "db-worker", this);
            }
            return std::make_shared<sys::ResponseMessage>();
        };

        sys::MessagePointer handleListProfiles(sys::Message *req)
        {
            if (dynamic_cast<settings::Messages::ListProfiles *>(req) != nullptr) {
                std::list<std::string> profiles = {"silent", "loud"};
                return std::make_shared<settings::Messages::ProfileListResponse>(profiles);
            }
            return std::make_shared<sys::ResponseMessage>();
        };

        sys::MessagePointer handleListModes(sys::Message *req)
        {
            if (dynamic_cast<settings::Messages::ListProfiles *>(req) != nullptr) {
                std::list<std::string> modes = {"mode1", "mode2"};
                return std::make_shared<settings::Messages::ModeListResponse>(modes);
            }
            return std::make_shared<sys::ResponseMessage>();
        };
    };
} // namespace settings

TEST_CASE("Settings Messages")
{
    SECTION("Init handler")
    {
        settings::Service settings("settings");
        settings.InitHandler();
    }

    SECTION("Send register messages")
    {
        settings::Service settings("settings");
        settings.InitHandler();

        sys::Bus::SendUnicast(std::make_shared<settings::Messages::RegisterOnVariableChange>(settings::EntryPath(
                                  {"mode", "service", "profile", "variable", settings::SettingsScope::AppLocal})),
                              "db-worker",
                              &settings);

        sys::Bus::SendUnicast(std::make_shared<settings::Messages::UnregisterOnVariableChange>(settings::EntryPath(
                                  {"mode", "service", "profile", "variable", settings::SettingsScope::AppLocal})),
                              "db-worker",
                              &settings);
    }

    SECTION("Send profile messages")
    {
        settings::Service settings("settings");
        settings.InitHandler();

        sys::Bus::SendUnicast(std::make_shared<settings::Messages::ListProfiles>(), "settings", &settings);

        sys::Bus::SendUnicast(std::make_shared<settings::Messages::AddProfile>("new-profile"), "settings", &settings);

        sys::Bus::SendUnicast(std::make_shared<settings::Messages::GetCurrentProfile>(), "settings", &settings);

        sys::Bus::SendUnicast(
            std::make_shared<settings::Messages::CurrentProfileChanged>("profile"), "settings", &settings);
    }

    SECTION("Send mode messages")
    {
        settings::Service settings("settings");
        settings.InitHandler();

        sys::Bus::SendUnicast(std::make_shared<settings::Messages::ListModes>(), "settings", &settings);

        sys::Bus::SendUnicast(std::make_shared<settings::Messages::GetCurrentMode>(), "settings", &settings);

        sys::Bus::SendUnicast(std::make_shared<settings::Messages::AddMode>("new-mode"), "settings", &settings);

        sys::Bus::SendUnicast(std::make_shared<settings::Messages::CurrentModeChanged>("mode"), "settings", &settings);
    }
}
