// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Settings.hpp"

#include <Service/Service.hpp> // for Service
#include <Service/Bus.hpp>     // for Bus
#include <utility>             // for move, pair
#include <vector>              // for vector

#include "messages/SettingsMessages.hpp" // for EntryPath, VariableChanged, CurrentModeChanged, CurrentProfileChanged, AddProfile, GetCurrentMode, GetCurrentProfile, GetVariable, ListModes, ListProfiles, RegisterOnModeChange, RegisterOnProfileChange, RegisterOnVariableChange, SetCurrentMode, SetCurrentProfile, SetVariable, UnregisterOnModeChange, UnregisterOnProfileChange, UnregisterOnVariableChange, VariableResponse, ModeListResponse, ModeResponse, ProfileListResponse, ProfileResponse, SettingsMessage (ptr only)
#include "Service/Common.hpp" // for BusChannels, BusChannels::ServiceDBNotifications, ReturnCodes, ReturnCodes::Success
#include "Service/Message.hpp" // for ResponseMessage, DataMessage, Message_t
#include "log/log.hpp"         // for LOG_DEBUG, LOG_INFO, LOG_ERROR

namespace Settings
{
    Settings::Settings(sys::Service *app, const std::string &dbAgentName) : dbAgentName(dbAgentName)
    {
        this->app = std::shared_ptr<sys::Service>(app, [](sys::Service *) {}); /// with deleter that doesn't delete.
        this->app->busChannels.push_back(sys::BusChannels::ServiceDBNotifications);
        sys::Bus::Add(std::static_pointer_cast<sys::Service>(this->app));
        registerHandlers();
    }

    Settings::~Settings()
    {
        sys::Bus::Remove(std::static_pointer_cast<sys::Service>(app));
    }

    void Settings::sendMsg(std::shared_ptr<::Settings::Messages::SettingsMessage> &&msg)
    {
        sys::Bus::SendUnicast(std::move(msg), dbAgentName, app.get());
    }

    void Settings::registerHandlers()
    {
        using std::placeholders::_1;
        using std::placeholders::_2;
        app->connect(::Settings::Messages::VariableChanged(),
                     std::bind(&Settings::handleVariableChanged, this, _1, _2));
        app->connect(::Settings::Messages::CurrentProfileChanged(),
                     std::bind(&Settings::handleCurrentProfileChanged, this, _1, _2));
        app->connect(::Settings::Messages::CurrentModeChanged(),
                     std::bind(&Settings::handleCurrentModeChanged, this, _1, _2));
        app->connect(::Settings::Messages::ProfileListResponse(),
                     std::bind(&Settings::handleProfileListResponse, this, _1, _2));
        app->connect(::Settings::Messages::ModeListResponse(),
                     std::bind(&Settings::handleModeListResponse, this, _1, _2));
    }
    auto Settings::handleVariableChanged(sys::DataMessage *req, sys::ResponseMessage *) -> sys::Message_t
    {
        LOG_DEBUG("handleVariableChanged");
        if (auto msg = dynamic_cast<::Settings::Messages::VariableChanged *>(req)) {
            auto key = msg->getPath().variable;
            auto val = msg->getValue();
            LOG_DEBUG("handleVariableChanged: (k=v): (%s=%s)", key.c_str(), val.value_or("").c_str());
            ValueCb::iterator it_cb = cbValues.find(key);
            if (cbValues.end() != it_cb) {
                it_cb->second(key, std::move(val));
                return std::make_shared<sys::ResponseMessage>();
            }
        }
        return std::make_shared<sys::ResponseMessage>();
    }
    auto Settings::handleCurrentProfileChanged(sys::DataMessage *req, sys::ResponseMessage *) -> sys::Message_t
    {
        LOG_DEBUG("handleCurrentProfileChanged");
        if (auto msg = dynamic_cast<::Settings::Messages::CurrentProfileChanged *>(req)) {
            auto profile = msg->getProfileName();
            LOG_DEBUG("handleCurrentProfileChanged: %s", profile.c_str());
            cbProfile(profile);
        }
        return std::make_shared<sys::ResponseMessage>();
    }
    auto Settings::handleCurrentModeChanged(sys::DataMessage *req, sys::ResponseMessage *) -> sys::Message_t
    {
        LOG_DEBUG("handleCurrentModeChanged");
        if (auto msg = dynamic_cast<::Settings::Messages::CurrentModeChanged *>(req)) {
            auto mode = msg->getProfileName();
            LOG_DEBUG("handleCurrentModeChanged: %s", mode.c_str());
            cbMode(mode);
        }
        return std::make_shared<sys::ResponseMessage>();
    }
    auto Settings::handleProfileListResponse(sys::DataMessage *req, sys::ResponseMessage *) -> sys::Message_t
    {
        LOG_DEBUG("handleProfileListResponse");
        if (auto msg = dynamic_cast<::Settings::Messages::ProfileListResponse *>(req)) {
            auto profiles = msg->getValue();
            LOG_DEBUG("handleProfileListResponse: %zu elements", profiles.size());
            cbAllProfiles(profiles);
        }
        return std::make_shared<sys::ResponseMessage>();
    }
    auto Settings::handleModeListResponse(sys::DataMessage *req, sys::ResponseMessage *) -> sys::Message_t
    {
        LOG_DEBUG("handleModeListResponse");
        if (auto msg = dynamic_cast<::Settings::Messages::ModeListResponse *>(req)) {
            auto modes = msg->getValue();
            LOG_DEBUG("handleModeListResponse: %zu elements", modes.size());
            cbAllModes(modes);
        }
        return std::make_shared<sys::ResponseMessage>();
    }

    void Settings::registerValueChange(const std::string &variableName, ValueChangedCallback cb)
    {
        ValueCb::iterator it_cb = cbValues.find(variableName);
        if (cbValues.end() != it_cb) {
            LOG_INFO("Callback function on value change (%s) already exists, rewriting", variableName.c_str());
        }
        cbValues[variableName] = cb;
        EntryPath path;
        path.variable = variableName;
        path.service  = app->GetName();
        auto msg      = std::make_shared<::Settings::Messages::RegisterOnVariableChange>(path);
        sendMsg(std::move(msg));
    }

    void Settings::unregisterValueChange(const std::string &variableName)
    {
        ValueCb::iterator it_cb = cbValues.find(variableName);
        if (cbValues.end() == it_cb) {
            LOG_INFO("Callback function on value change (%s) does not exist", variableName.c_str());
        }
        else {
            cbValues.erase(it_cb);
        }

        EntryPath path;
        path.variable = variableName;
        path.service  = app->GetName();
        auto msg      = std::make_shared<::Settings::Messages::UnregisterOnVariableChange>(path);
        sendMsg(std::move(msg));
    }

    void Settings::setValue(const std::string &variableName, const std::string &variableValue)
    {
        EntryPath path;
        path.variable = variableName;
        path.service  = app->GetName();
        auto msg      = std::make_shared<::Settings::Messages::SetVariable>(path, variableValue);
        sendMsg(std::move(msg));
    }

    void Settings::getAllProfiles(OnAllProfilesRetrievedCallback cb)
    {
        if (nullptr == cbAllProfiles) {
            sendMsg(std::make_shared<::Settings::Messages::ListProfiles>());
        }
        cbAllProfiles = cb;
    }

    void Settings::setCurrentProfile(const std::string &profile)
    {
        sendMsg(std::make_shared<::Settings::Messages::SetCurrentProfile>(profile));
    }

    void Settings::addProfile(const std::string &profile)
    {
        sendMsg(std::make_shared<::Settings::Messages::AddProfile>(profile));
    }

    void Settings::registerProfileChange(ProfileChangedCallback cb)
    {
        if (nullptr != cbProfile) {
            LOG_DEBUG("Profile change callback already exists, overwritting");
        }
        else {
            sendMsg(std::make_shared<::Settings::Messages::RegisterOnProfileChange>());
        }

        cbProfile = cb;
    }

    void Settings::unregisterProfileChange()
    {
        cbProfile = nullptr;
        sendMsg(std::make_shared<::Settings::Messages::UnregisterOnProfileChange>());
    }

    void Settings::getAllModes(OnAllModesRetrievedCallback cb)
    {
        if (nullptr == cbAllModes) {
            sendMsg(std::make_shared<::Settings::Messages::ListModes>());
        }
        cbAllModes = cb;
    }

    void Settings::setCurrentMode(const std::string &mode)
    {
        sendMsg(std::make_shared<::Settings::Messages::SetCurrentMode>(mode));
    }

    void Settings::addMode(const std::string &mode)
    {
        sendMsg(std::make_shared<::Settings::Messages::AddMode>(mode));
    }

    void Settings::registerModeChange(ModeChangedCallback cb)
    {
        if (nullptr != cbMode) {
            LOG_DEBUG("ModeChange callback allready set overwriting");
        }
        else {
            sendMsg(std::make_shared<::Settings::Messages::RegisterOnModeChange>());
        }

        cbMode = cb;
    }

    void Settings::unregisterModeChange()
    {
        cbMode = nullptr;
        sendMsg(std::make_shared<::Settings::Messages::UnregisterOnModeChange>());
    }
} // namespace Settings
