// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-appmgr/data/MmiActionsParams.hpp>

namespace mmi
{

    static inline std::string removePhrase(std::string str, std::string phrase)
    {
        auto find_pos = str.find(phrase);
        while (find_pos != std::string::npos) {
            str.replace(find_pos, phrase.size(), "");
            find_pos = str.find(phrase, find_pos);
        }
        return str;
    }

    class MMIMessageVisitor : public app::manager::actions::Visitor
    {
      public:
        MMIMessageVisitor()
        {
            messageDictionary.clear();
            messageDictionary.emplace(mmiactions::IMMICustomResultParams::MMIResultMessage::NoneSpecifiedFailed,
                                      "app_desktop_info_mmi_none_specified_failed");
            messageDictionary.emplace(mmiactions::IMMICustomResultParams::MMIResultMessage::NoneSpecifiedSuccess,
                                      "app_desktop_info_mmi_none_specified_success");
            messageDictionary.emplace(mmiactions::IMMICustomResultParams::MMIResultMessage::CommonFailure,
                                      "app_desktop_info_mmi_common_failed");
            messageDictionary.emplace(mmiactions::IMMICustomResultParams::MMIResultMessage::CommonNoMessage,
                                      "app_desktop_info_mmi_common_no");
            messageDictionary.emplace(mmiactions::IMMICustomResultParams::MMIResultMessage::RegistrationSuccessful,
                                      "app_desktop_info_mmi_registration_success");
            messageDictionary.emplace(mmiactions::IMMICustomResultParams::MMIResultMessage::RegistrationFailed,
                                      "app_desktop_info_mmi_registration_failed");
            messageDictionary.emplace(mmiactions::IMMICustomResultParams::MMIResultMessage::ErasureSuccessful,
                                      "app_desktop_info_mmi_erasure_success");
            messageDictionary.emplace(mmiactions::IMMICustomResultParams::MMIResultMessage::ErasureFailed,
                                      "app_desktop_info_mmi_erasure_failed");
            messageDictionary.emplace(mmiactions::IMMICustomResultParams::MMIResultMessage::DisablingSuccessful,
                                      "app_desktop_info_mmi_disabling_success");
            messageDictionary.emplace(mmiactions::IMMICustomResultParams::MMIResultMessage::DisablingFailed,
                                      "app_desktop_info_mmi_disabling_failed");
            messageDictionary.emplace(mmiactions::IMMICustomResultParams::MMIResultMessage::EnablingSuccessful,
                                      "app_desktop_info_mmi_enabling_success");
            messageDictionary.emplace(mmiactions::IMMICustomResultParams::MMIResultMessage::EnablingFailed,
                                      "app_desktop_info_mmi_enabling_failed");
        };

        virtual void visit(mmiactions::MMINoneSpecifiedResult &customResult, std::string &displayMessage)
        {
            displayMessage += getSelectedMessagesFromDictionary(customResult.getMessage());
        }

        virtual void visit(mmiactions::MMICustomResultParams &customResult, std::string &displayMessage)
        {
            displayMessage += getSelectedMessagesFromDictionary(customResult.getMessage());
        }

        virtual void visit(mmiactions::MMICallForwardingResult &customResult, std::string &displayMessage)
        {
            displayMessage += utils::localize.get("app_desktop_info_mmi_call_forwarding") + "\n";
            if (customResult.getMessageType() == mmiactions::IMMICustomResultParams::MMIType::CallForwardingData) {
                auto [voice, fax, sync, async] = customResult.getData();
                displayMessage += utils::localize.get("app_desktop_info_mmi_voice") + ": " + voice + "\n" +
                                  utils::localize.get("app_desktop_info_mmi_fax") + ": " + fax + "\n" +
                                  utils::localize.get("app_desktop_info_mmi_sync") + ": " + sync + "\n" +
                                  utils::localize.get("app_desktop_info_mmi_async") + ": " + async + "\n";
            }
            else if (customResult.getMessageType() ==
                     mmiactions::IMMICustomResultParams::MMIType::CallForwardingNotification) {
                displayMessage += getSelectedMessagesFromDictionary(customResult.getMessage());
            }
        }

        UTF8 getSelectedMessagesFromDictionary(
            const std::vector<mmiactions::IMMICustomResultParams::MMIResultMessage> &msgList)
        {
            UTF8 selectedMessages = "";
            for (const auto &msg : msgList) {
                auto it = messageDictionary.find(msg);
                if (messageDictionary.end() != it) {
                    selectedMessages += utils::localize.get(it->second) + "\n";
                }
            }
            return selectedMessages;
        }

      private:
        std::map<mmiactions::IMMICustomResultParams::MMIResultMessage, std::string> messageDictionary;
    };

} // namespace mmi
