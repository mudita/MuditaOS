// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-db/Settings.hpp>
#include <service-db/SettingsMessages.hpp>

#include <Service/Bus.hpp>
#include <Service/Common.hpp>
#include <Service/Message.hpp>
#include <Service/Service.hpp>
#include <log/log.hpp>

#include <utility>
#include <vector>

namespace settings
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
        LOG_DEBUG("Settings::~Settings on %s", app->GetName().c_str());
        sys::Bus::Remove(std::static_pointer_cast<sys::Service>(app));
    }

    void Settings::sendMsg(std::shared_ptr<settings::Messages::SettingsMessage> &&msg)
    {
        sys::Bus::SendUnicast(std::move(msg), dbAgentName, app.get());
    }

    void Settings::registerHandlers()
    {
        using std::placeholders::_1;
        using std::placeholders::_2;
        LOG_DEBUG("Settings::registerHandlers for %s", app->GetName().c_str());
        app->connect(settings::Messages::VariableChanged(), std::bind(&Settings::handleVariableChanged, this, _1));
        app->connect(settings::Messages::CurrentProfileChanged(),
                     std::bind(&Settings::handleCurrentProfileChanged, this, _1));
        app->connect(settings::Messages::CurrentModeChanged(),
                     std::bind(&Settings::handleCurrentModeChanged, this, _1));
        app->connect(settings::Messages::ProfileListResponse(),
                     std::bind(&Settings::handleProfileListResponse, this, _1));
        app->connect(settings::Messages::ModeListResponse(), std::bind(&Settings::handleModeListResponse, this, _1));
    }
    auto Settings::handleVariableChanged(sys::Message *req) -> sys::MessagePointer
    {
        LOG_DEBUG("handleVariableChanged");
        if (auto msg = dynamic_cast<settings::Messages::VariableChanged *>(req)) {
            auto key = msg->getPath().variable;
            auto val = msg->getValue();
            LOG_DEBUG("handleVariableChanged: (k=v): (%s=%s)", key.c_str(), val.value_or("").c_str());
            ValueCb::iterator it_cb = cbValues.find(key);
            if (cbValues.end() != it_cb) {
                auto [cb, cbWithName] = it_cb->second;
                if (nullptr != cb && nullptr != cbWithName) {
                    // in case of two callbacks there is a need to duplicate the value
                    auto value = std::move(val.value_or(""));
                    cb(std::string{value});
                    cbWithName(key, std::move(value));
                }
                else if (nullptr != cb) {
                    cb(std::move(val.value_or("")));
                }
                else {
                    cbWithName(key, std::move(val.value_or("")));
                }
            }
        }
        return std::make_shared<sys::ResponseMessage>();
    }
    auto Settings::handleCurrentProfileChanged(sys::Message *req) -> sys::MessagePointer
    {
        LOG_DEBUG("handleCurrentProfileChanged");
        if (auto msg = dynamic_cast<settings::Messages::CurrentProfileChanged *>(req)) {
            auto profile = msg->getProfileName();
            LOG_DEBUG("handleCurrentProfileChanged: %s", profile.c_str());
            cbProfile(profile);
        }
        return std::make_shared<sys::ResponseMessage>();
    }
    auto Settings::handleCurrentModeChanged(sys::Message *req) -> sys::MessagePointer
    {
        LOG_DEBUG("handleCurrentModeChanged");
        if (auto msg = dynamic_cast<settings::Messages::CurrentModeChanged *>(req)) {
            auto mode = msg->getProfileName();
            LOG_DEBUG("handleCurrentModeChanged: %s", mode.c_str());
            cbMode(mode);
        }
        return std::make_shared<sys::ResponseMessage>();
    }
    auto Settings::handleProfileListResponse(sys::Message *req) -> sys::MessagePointer
    {
        LOG_DEBUG("handleProfileListResponse");
        if (auto msg = dynamic_cast<settings::Messages::ProfileListResponse *>(req)) {
            auto profiles = msg->getValue();
            LOG_DEBUG("handleProfileListResponse: %zu elements", profiles.size());
            cbAllProfiles(profiles);
        }
        return std::make_shared<sys::ResponseMessage>();
    }
    auto Settings::handleModeListResponse(sys::Message *req) -> sys::MessagePointer
    {
        LOG_DEBUG("handleModeListResponse");
        if (auto msg = dynamic_cast<settings::Messages::ModeListResponse *>(req)) {
            auto modes = msg->getValue();
            LOG_DEBUG("handleModeListResponse: %zu elements", modes.size());
            cbAllModes(modes);
        }
        return std::make_shared<sys::ResponseMessage>();
    }

    void Settings::registerValueChange(const std::string &variableName, ValueChangedCallback cb)
    {
        ValueCb::iterator it_cb = cbValues.find(variableName);
        if (cbValues.end() != it_cb && nullptr != it_cb->second.first) {
            LOG_INFO("Callback function on value change (%s) already exists, rewriting", variableName.c_str());
        }
        cbValues[variableName].first = cb;
        EntryPath path;
        path.variable = variableName;
        path.service  = app->GetName();
        auto msg      = std::make_shared<settings::Messages::RegisterOnVariableChange>(path);
        sendMsg(std::move(msg));
    }

    void Settings::registerValueChange(const std::string &variableName, ValueChangedCallbackWithName cb)
    {
        auto it_cb = cbValues.find(variableName);
        if (cbValues.end() != it_cb && nullptr != it_cb->second.second) {
            LOG_INFO("Callback function on value change (%s) already exists, rewriting", variableName.c_str());
        }
        cbValues[variableName].second = cb;
        EntryPath path;
        path.variable = variableName;
        path.service  = app->GetName();
        auto msg      = std::make_shared<settings::Messages::RegisterOnVariableChange>(path);
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
        auto msg      = std::make_shared<settings::Messages::UnregisterOnVariableChange>(path);
        sendMsg(std::move(msg));
    }

    void Settings::setValue(const std::string &variableName, const std::string &variableValue)
    {
        EntryPath path;
        path.variable = variableName;
        path.service  = app->GetName();
        auto msg      = std::make_shared<settings::Messages::SetVariable>(path, variableValue);
        sendMsg(std::move(msg));
    }

    void Settings::getAllProfiles(OnAllProfilesRetrievedCallback cb)
    {
        if (nullptr == cbAllProfiles) {
            sendMsg(std::make_shared<settings::Messages::ListProfiles>());
        }
        cbAllProfiles = cb;
    }

    void Settings::setCurrentProfile(const std::string &profile)
    {
        sendMsg(std::make_shared<settings::Messages::SetCurrentProfile>(profile));
    }

    void Settings::addProfile(const std::string &profile)
    {
        sendMsg(std::make_shared<settings::Messages::AddProfile>(profile));
    }

    void Settings::registerProfileChange(ProfileChangedCallback cb)
    {
        if (nullptr != cbProfile) {
            LOG_DEBUG("Profile change callback already exists, overwritting");
        }
        else {
            sendMsg(std::make_shared<settings::Messages::RegisterOnProfileChange>());
        }

        cbProfile = cb;
    }

    void Settings::unregisterProfileChange()
    {
        cbProfile = nullptr;
        sendMsg(std::make_shared<settings::Messages::UnregisterOnProfileChange>());
    }

    void Settings::getAllModes(OnAllModesRetrievedCallback cb)
    {
        if (nullptr == cbAllModes) {
            sendMsg(std::make_shared<settings::Messages::ListModes>());
        }
        cbAllModes = cb;
    }

    void Settings::setCurrentMode(const std::string &mode)
    {
        sendMsg(std::make_shared<settings::Messages::SetCurrentMode>(mode));
    }

    void Settings::addMode(const std::string &mode)
    {
        sendMsg(std::make_shared<settings::Messages::AddMode>(mode));
    }

    void Settings::registerModeChange(ModeChangedCallback cb)
    {
        if (nullptr != cbMode) {
            LOG_DEBUG("ModeChange callback allready set overwriting");
        }
        else {
            sendMsg(std::make_shared<settings::Messages::RegisterOnModeChange>());
        }
        cbMode = cb;
    }

    void Settings::unregisterModeChange()
    {
        cbMode = nullptr;
        sendMsg(std::make_shared<settings::Messages::UnregisterOnModeChange>());
    }
} // namespace settings
