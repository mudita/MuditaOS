// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SettingsAgent.hpp"

#include <module-sys/Service/Bus.hpp> // for Bus
#include <module-vfs/vfs.hpp>         // for USER_PATH
#include <memory>                     // for make_shared, allocator, shared_ptr, make_unique, unique_ptr
#include <functional>                 // for _Bind_helper<>::type, _Placeholder, bind, _1, _2
#include <utility>                    // for move

#include "Database/Database.hpp"         // for Database
#include "Service/Service.hpp"           // for Service
#include "messages/SettingsMessages.hpp" // for SetVariable, GetVariable, VariableChanged, VariableResponse, EntryPath

SettingsAgent::SettingsAgent(sys::Service *parentService) : DatabaseAgent(parentService)
{
    database = std::make_unique<Database>(getDbFilePath().c_str());
}

void SettingsAgent::initDb()
{
    // LOG_DEBUG("sql:\n--\n%s\n--", getDbInitString().c_str());
    database->execute(getDbInitString().c_str());
}

void SettingsAgent::deinitDb()
{
    database->deinitialize();
}

void SettingsAgent::registerMessages()
{
    // connect handler & message in parent service
    using std::placeholders::_1;
    using std::placeholders::_2;

    parentService->connect(Settings::Messages::GetVariable(),
                           std::bind(&SettingsAgent::handleGetVariable, this, _1, _2));
    parentService->connect(Settings::Messages::SetVariable(),
                           std::bind(&SettingsAgent::handleSetVariable, this, _1, _2));
}

auto SettingsAgent::getDbInitString() -> const std::string
{
    auto x          = 0;
    const char *sql = (
#include "settings.sql"
    );
    return sql;
}

auto SettingsAgent::getDbFilePath() -> const std::string
{

    return USER_PATH("settings_v2.db");
}
auto SettingsAgent::getAgentName() -> const std::string
{
    return std::string("settingsAgent");
}

auto SettingsAgent::getValue(Settings::EntryPath path) -> std::optional<std::string>
{
    // get value from db from specified path
    return "";
}

auto SettingsAgent::setValue(Settings::EntryPath path, std::string value) -> std::string
{
    // set value into db
    // return old value
    return "";
}

auto SettingsAgent::handleGetVariable(sys::DataMessage *req, sys::ResponseMessage * /*response*/) -> sys::Message_t
{
    if (auto msg = dynamic_cast<Settings::Messages::GetVariable *>(req)) {

        auto path  = msg->getPath();
        auto value = getValue(path);

        return std::make_shared<Settings::Messages::VariableResponse>(path, value);
    }
    return std::make_shared<sys::ResponseMessage>();
};

auto SettingsAgent::handleSetVariable(sys::DataMessage *req, sys::ResponseMessage * /*response*/) -> sys::Message_t
{
    if (auto msg = dynamic_cast<Settings::Messages::SetVariable *>(req)) {

        auto path     = msg->getPath();
        auto value    = msg->getValue().value_or("");
        auto oldValue = setValue(path, msg->getValue().value_or(""));

        auto updateMsg = std::make_shared<Settings::Messages::VariableChanged>(path, value, oldValue);
        sys::Bus::SendUnicast(std::move(updateMsg), "db-worker", parentService);
    }
    return std::make_shared<sys::ResponseMessage>();
};
