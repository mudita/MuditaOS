// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <memory>
#include <service-db/Settings.hpp>
#include <service-db/SettingsMessages.hpp>
#include <service-db/SettingsCache.hpp>

#include <Service/Common.hpp>
#include <Service/Message.hpp>
//#include <Service/Service.hpp>
// #include <log/log.hpp>

#include <stdexcept>
#include <utility>
#include <vector>

#if defined(DEBUG_SETTINGS_DB) and DEBUG_SETTINGS_DB == 1
#define log_debug(...) LOG_DEBUG(__VA_ARGS__);
#else
#define log_debug(...)
#endif

namespace settings
{
    Settings::~Settings()
    {
        deinit();
    }

    void Settings::init(const service::Interface &interface)
    {
        this->interface = Interface(interface);
        if (this->interface) {
            changeHandlers(Interface::Change::Register);
        }
        else {
            throw std::runtime_error("need the interface!");
        }
    }

    void Settings::deinit()
    {
        if (interface) {
            changeHandlers(Interface::Change::Deregister);
        }
    }

    void Settings::changeHandlers(enum Interface::Change change)
    {
        using std::placeholders::_1;
        using std::placeholders::_2;
        interface.changeHandler(
            typeid(settings::Messages::VariableChanged), change, std::bind(&Settings::handleVariableChanged, this, _1));
        interface.changeHandler(typeid(settings::Messages::CurrentProfileChanged),
                                change,
                                std::bind(&Settings::handleCurrentProfileChanged, this, _1));
        interface.changeHandler(typeid(settings::Messages::CurrentModeChanged),
                                change,
                                std::bind(&Settings::handleCurrentModeChanged, this, _1));
        interface.changeHandler(typeid(settings::Messages::ProfileListResponse),
                                change,
                                std::bind(&Settings::handleProfileListResponse, this, _1));
        interface.changeHandler(typeid(settings::Messages::ModeListResponse),
                                change,
                                std::bind(&Settings::handleModeListResponse, this, _1));
    }

    auto Settings::handleVariableChanged(sys::Message *req) -> sys::MessagePointer
    {
        log_debug("handleVariableChanged");
        if (auto msg = dynamic_cast<settings::Messages::VariableChanged *>(req)) {
            auto key = msg->getPath();
            auto val = msg->getValue();
            log_debug("handleVariableChanged: (k=v): (%s=%s)", key.to_string().c_str(), val.value_or("").c_str());
            ValueCb::iterator it_cb = cbValues.find(key);
            if (cbValues.end() != it_cb) {
                auto [cb, cbWithName] = it_cb->second;
                if (nullptr != cb && nullptr != cbWithName) {
                    // in case of two callbacks there is a need to duplicate the value
                    auto value = val.value_or("");
                    cb(std::string{value});
                    cbWithName(key.variable, value);
                }
                else if (nullptr != cb) {
                    cb(val.value_or(""));
                }
                else {
                    cbWithName(key.variable, val.value_or(""));
                }
            }
        }
        return std::make_shared<sys::ResponseMessage>();
    }
    auto Settings::handleCurrentProfileChanged(sys::Message *req) -> sys::MessagePointer
    {
        log_debug("handleCurrentProfileChanged");
        if (auto msg = dynamic_cast<settings::Messages::CurrentProfileChanged *>(req)) {
            auto profile = msg->getProfileName();
            log_debug("handleCurrentProfileChanged: %s", profile.c_str());
            cbProfile(profile);
        }
        return std::make_shared<sys::ResponseMessage>();
    }
    auto Settings::handleCurrentModeChanged(sys::Message *req) -> sys::MessagePointer
    {
        log_debug("handleCurrentModeChanged");
        if (auto msg = dynamic_cast<settings::Messages::CurrentModeChanged *>(req)) {
            auto mode = msg->getProfileName();
            log_debug("handleCurrentModeChanged: %s", mode.c_str());
            cbMode(mode);
        }
        return std::make_shared<sys::ResponseMessage>();
    }
    auto Settings::handleProfileListResponse(sys::Message *req) -> sys::MessagePointer
    {
        log_debug("handleProfileListResponse");
        if (auto msg = dynamic_cast<settings::Messages::ProfileListResponse *>(req)) {
            auto profiles = msg->getValue();
            log_debug("handleProfileListResponse: %zu elements", profiles.size());
            cbAllProfiles(profiles);
        }
        return std::make_shared<sys::ResponseMessage>();
    }
    auto Settings::handleModeListResponse(sys::Message *req) -> sys::MessagePointer
    {
        log_debug("handleModeListResponse");
        if (auto msg = dynamic_cast<settings::Messages::ModeListResponse *>(req)) {
            auto modes = msg->getValue();
            log_debug("handleModeListResponse: %zu elements", modes.size());
            cbAllModes(modes);
        }
        return std::make_shared<sys::ResponseMessage>();
    }

    void Settings::registerValueChange(const std::string &variableName, ValueChangedCallback cb, SettingsScope scope)
    {
        auto path = EntryPath{.service = interface.ownerName(), .variable = variableName, .scope = scope};

        auto it_cb = cbValues.find(path);
        if (cbValues.end() != it_cb && nullptr != it_cb->second.first) {
            // LOG_INFO("Callback function on value change (%s) already exists, rewriting", path.to_string().c_str());
        }
        cbValues[path].first = std::move(cb);

        auto msg = std::make_shared<settings::Messages::RegisterOnVariableChange>(path);
        interface.sendMsg(std::move(msg));
    }

    void Settings::registerValueChange(const std::string &variableName,
                                       ValueChangedCallbackWithName cb,
                                       SettingsScope scope)
    {
        auto path = EntryPath{.service = interface.ownerName(), .variable = variableName, .scope = scope};

        auto it_cb = cbValues.find(path);
        if (cbValues.end() != it_cb && nullptr != it_cb->second.second) {
            // LOG_INFO("Callback function on value change (%s) already exists, rewriting", path.to_string().c_str());
        }
        cbValues[path].second = std::move(cb);

        auto msg = std::make_shared<settings::Messages::RegisterOnVariableChange>(path);
        interface.sendMsg(std::move(msg));
    }

    void Settings::unregisterValueChange(const std::string &variableName, SettingsScope scope)
    {
        auto path = EntryPath{.service = interface.ownerName(), .variable = variableName, .scope = scope};

        auto it_cb = cbValues.find(path);
        if (cbValues.end() == it_cb) {
            // LOG_INFO("Callback function on value change (%s) does not exist", path.to_string().c_str());
        }
        else {
            log_debug("[Settings::unregisterValueChange] %s", path.to_string().c_str());
            cbValues.erase(it_cb);
        }

        auto msg = std::make_shared<settings::Messages::UnregisterOnVariableChange>(path);
        interface.sendMsg(std::move(msg));
    }

    void Settings::unregisterValueChange()
    {
        for (const auto &it_cb : cbValues) {
            log_debug("[Settings::unregisterValueChange] %s", it_cb.first.to_string().c_str());
            auto msg = std::make_shared<settings::Messages::UnregisterOnVariableChange>(it_cb.first);
            interface.sendMsg(std::move(msg));
        }
        cbValues.clear();
        // LOG_INFO("Unregistered all settings variable change on service (%s)", interface.ownerName().c_str());
    }

    void Settings::setValue(const std::string &variableName, const std::string &variableValue, SettingsScope scope)
    {
        auto path = EntryPath{.service = interface.ownerName(), .variable = variableName, .scope = scope};
        interface.sendMsg(std::make_shared<settings::Messages::SetVariable>(path, variableValue));
        interface.getCache()->setValue(path, variableValue);
    }

    std::string Settings::getValue(const std::string &variableName, SettingsScope scope)
    {
        return interface.getCache()->getValue(
            {.service = interface.ownerName(), .variable = variableName, .scope = scope});
    }

    void Settings::getAllProfiles(OnAllProfilesRetrievedCallback cb)
    {
        if (nullptr == cbAllProfiles) {
            interface.sendMsg(std::make_shared<settings::Messages::ListProfiles>());
        }
        cbAllProfiles = std::move(cb);
    }

    void Settings::setCurrentProfile(const std::string &profile)
    {
        interface.sendMsg(std::make_shared<settings::Messages::SetCurrentProfile>(profile));
    }

    void Settings::addProfile(const std::string &profile)
    {
        interface.sendMsg(std::make_shared<settings::Messages::AddProfile>(profile));
    }

    void Settings::registerProfileChange(ProfileChangedCallback cb)
    {
        if (nullptr != cbProfile) {
            log_debug("Profile change callback already exists, overwritting");
        }
        else {
            interface.sendMsg(std::make_shared<settings::Messages::RegisterOnProfileChange>());
        }

        cbProfile = std::move(cb);
    }

    void Settings::unregisterProfileChange()
    {
        cbProfile = nullptr;
        interface.sendMsg(std::make_shared<settings::Messages::UnregisterOnProfileChange>());
    }

    void Settings::getAllModes(OnAllModesRetrievedCallback cb)
    {
        if (nullptr == cbAllModes) {
            interface.sendMsg(std::make_shared<settings::Messages::ListModes>());
        }
        cbAllModes = std::move(cb);
    }

    void Settings::setCurrentMode(const std::string &mode)
    {
        interface.sendMsg(std::make_shared<settings::Messages::SetCurrentMode>(mode));
    }

    void Settings::addMode(const std::string &mode)
    {
        interface.sendMsg(std::make_shared<settings::Messages::AddMode>(mode));
    }

    void Settings::registerModeChange(ModeChangedCallback cb)
    {
        if (nullptr != cbMode) {
            log_debug("ModeChange callback allready set overwriting");
        }
        else {
            interface.sendMsg(std::make_shared<settings::Messages::RegisterOnModeChange>());
        }
        cbMode = std::move(cb);
    }

    void Settings::unregisterModeChange()
    {
        cbMode = nullptr;
        interface.sendMsg(std::make_shared<settings::Messages::UnregisterOnModeChange>());
    }
} // namespace settings
