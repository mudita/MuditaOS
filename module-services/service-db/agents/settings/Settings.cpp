#include "Settings.hpp"
#include "messages/SettingsMessages.hpp"
#include <Service/Service.hpp>
#include <Service/Bus.hpp>

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

    sys::Message_t Settings::sendMsgAndWaitForResponse(std::shared_ptr<::Settings::Messages::SettingsMessage> &&msg)
    {
        auto ret = sys::Bus::SendUnicast(std::move(msg), dbAgentName, app.get(), dbWaitTimeMs);
        if (ret.first == sys::ReturnCodes::Success) {
            return ret.second;
        }
        return sys::Message_t{};
    }

    void Settings::registerHandlers()
    {
        handleValueChgMsg();
        handleProfileChgMsg();
        handleModeChgMsg();
    }

    void Settings::handleValueChgMsg()
    {
        LOG_DEBUG("handle handleValueChgMsg");
        app->connect(::Settings::Messages::VariableChanged(), [&](sys::DataMessage *req, sys::ResponseMessage *res) {
            LOG_DEBUG("handleSettingsValueChgMsg");
            if (auto msg = dynamic_cast<::Settings::Messages::VariableChanged *>(req)) {
                auto key = msg->getPath().variable;
                auto val = msg->getValue();
                LOG_DEBUG("handleSettingsValueChgMsg: (k=v): (%s=%s)", key.c_str(), val.value_or("").c_str());
                ValueCb::iterator it_cb = cbValues.find(key);
                if (cbValues.end() != it_cb) {
                    it_cb->second(key, std::move(val));
                    return std::make_shared<sys::ResponseMessage>();
                }
            }
            return std::make_shared<sys::ResponseMessage>();
        });
    }

    void Settings::handleProfileChgMsg()
    {
        LOG_DEBUG("handle handleProfileChgMsg");
        app->connect(::Settings::Messages::CurrentProfileChanged(),
                     [&](sys::DataMessage *req, [[maybe_unused]] sys::ResponseMessage *res) {
                         LOG_DEBUG("handleProfileChgMsg");
                         if (auto msg = dynamic_cast<::Settings::Messages::CurrentProfileChanged *>(req)) {
                             auto profile = msg->getProfileName();
                             LOG_DEBUG("handleGetProfilesMsg: %s", profile.c_str());
                             cbProfile(profile);
                         }
                         return std::make_shared<sys::ResponseMessage>();
                     });
    }

    void Settings::handleModeChgMsg()
    {
        LOG_DEBUG("handle handleModeChgMsg");
        app->connect(::Settings::Messages::CurrentModeChanged(),
                     [&](sys::DataMessage *req, [[maybe_unused]] sys::ResponseMessage *res) {
                         LOG_DEBUG("handleModeChgMsg");
                         if (auto msg = dynamic_cast<::Settings::Messages::CurrentModeChanged *>(req)) {
                             auto mode = msg->getProfileName();
                             LOG_DEBUG("handleModeChgMsg: %s", mode.c_str());
                             cbMode(mode);
                         }
                         return std::make_shared<sys::ResponseMessage>();
                     });
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

    std::optional<std::string> Settings::getValue(const std::string &variableName)
    {
        EntryPath path;
        path.variable = variableName;
        path.service  = app->GetName();
        auto msg      = std::make_shared<::Settings::Messages::GetVariable>(path);
        auto ret      = sendMsgAndWaitForResponse(msg);
        auto msgVar   = dynamic_cast<::Settings::Messages::VariableResponse *>(ret.get());
        if (nullptr != msgVar) {
            if (msgVar->getPath() == path.to_string()) {
                return msgVar->getValue();
            }
            else {
                LOG_ERROR("Settings::getValue asked for variable:%s received variable:%s",
                          variableName.c_str(),
                          msgVar->getPath().c_str());
            }
        }
        return std::string{};
    }

    void Settings::setValue(const std::string &variableName, const std::string &variableValue)
    {
        EntryPath path;
        path.variable = variableName;
        path.service  = app->GetName();
        auto msg      = std::make_shared<::Settings::Messages::SetVariable>(path, variableValue);
        sendMsg(std::move(msg));
    }

    Settings::ListOfProfiles Settings::getAllProfiles()
    {
        auto ret         = sendMsgAndWaitForResponse(std::make_shared<::Settings::Messages::ListProfiles>());
        auto msgProfiles = dynamic_cast<::Settings::Messages::ProfileListResponse *>(ret.get());
        if (nullptr != msgProfiles) {
            return msgProfiles->getValue();
        }
        return ListOfProfiles();
    }

    std::string Settings::getCurrentProfile()
    {
        auto ret     = sendMsgAndWaitForResponse(std::make_shared<::Settings::Messages::GetCurrentProfile>());
        auto msgProf = dynamic_cast<::Settings::Messages::ProfileResponse *>(ret.get());
        if (nullptr != msgProf) {
            return msgProf->getValue();
        }
        return std::string{};
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

    Settings::ListOfModes Settings::getAllModes()
    {
        auto ret      = sendMsgAndWaitForResponse(std::make_shared<::Settings::Messages::ListModes>());
        auto msgModes = dynamic_cast<::Settings::Messages::ModeListResponse *>(ret.get());
        if (nullptr != msgModes) {
            return msgModes->getValue();
        }
        return ListOfModes();
    }

    std::string Settings::getCurrentMode()
    {
        auto ret     = sendMsgAndWaitForResponse(std::make_shared<::Settings::Messages::GetCurrentMode>());
        auto msgMode = dynamic_cast<::Settings::Messages::ModeResponse *>(ret.get());
        if (nullptr != msgMode) {
            return msgMode->getValue();
        }
        return std::string{};
    }

    void Settings::setCurrentMode(const std::string &mode)
    {
        sendMsg(std::make_shared<::Settings::Messages::SetCurrentMode>(mode));
    }

    void Settings::registerModeChange(ModeChangedCallback cb)
    {
        if (nullptr == cbMode) {
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
