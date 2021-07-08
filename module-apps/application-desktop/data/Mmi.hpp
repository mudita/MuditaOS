// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <i18n/i18n.hpp>
#include <service-appmgr/data/MmiActionsParams.hpp>
#include <utf8/UTF8.hpp>

#include <map>

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
                                      "app_desktop_info_mmi_common_no_message");
            messageDictionary.emplace(mmiactions::IMMICustomResultParams::MMIResultMessage::CommonMMINotSupported,
                                      "app_desktop_info_mmi_common_mmi_not_supported");
            messageDictionary.emplace(mmiactions::IMMICustomResultParams::MMIResultMessage::CommonEnabled,
                                      "app_desktop_info_mmi_common_enabled");
            messageDictionary.emplace(mmiactions::IMMICustomResultParams::MMIResultMessage::CommonDisabled,
                                      "app_desktop_info_mmi_common_disabled");
            messageDictionary.emplace(mmiactions::IMMICustomResultParams::MMIResultMessage::CommonVoice,
                                      "app_desktop_info_mmi_common_voice");
            messageDictionary.emplace(mmiactions::IMMICustomResultParams::MMIResultMessage::CommonData,
                                      "app_desktop_info_mmi_common_data");
            messageDictionary.emplace(mmiactions::IMMICustomResultParams::MMIResultMessage::CommonFax,
                                      "app_desktop_info_mmi_common_fax");
            messageDictionary.emplace(mmiactions::IMMICustomResultParams::MMIResultMessage::CommonSync,
                                      "app_desktop_info_mmi_common_sync");
            messageDictionary.emplace(mmiactions::IMMICustomResultParams::MMIResultMessage::CommonAsync,
                                      "app_desktop_info_mmi_common_async");
            messageDictionary.emplace(mmiactions::IMMICustomResultParams::MMIResultMessage::CommonAllDisabled,
                                      "app_desktop_info_mmi_common_all_disabled");
            messageDictionary.emplace(mmiactions::IMMICustomResultParams::MMIResultMessage::CommonDeactivated,
                                      "app_desktop_info_mmi_common_deactivated");
            messageDictionary.emplace(mmiactions::IMMICustomResultParams::MMIResultMessage::CommonActivated,
                                      "app_desktop_info_mmi_common_activated");
            messageDictionary.emplace(mmiactions::IMMICustomResultParams::MMIResultMessage::CommonQuery,
                                      "app_desktop_info_mmi_common_query");
            messageDictionary.emplace(mmiactions::IMMICustomResultParams::MMIResultMessage::ClirAccordingToSubscription,
                                      "app_desktop_info_mmi_clir_according_to_subscription");
            messageDictionary.emplace(mmiactions::IMMICustomResultParams::MMIResultMessage::ClirEnabled,
                                      "app_desktop_info_mmi_clir_enabled");
            messageDictionary.emplace(mmiactions::IMMICustomResultParams::MMIResultMessage::ClirDisabled,
                                      "app_desktop_info_mmi_clir_disabled");
            messageDictionary.emplace(mmiactions::IMMICustomResultParams::MMIResultMessage::ClirNotProvisioned,
                                      "app_desktop_info_mmi_clir_not_provisioned");
            messageDictionary.emplace(mmiactions::IMMICustomResultParams::MMIResultMessage::ClirPermanentProvisioned,
                                      "app_desktop_info_mmi_clir_permanent_provisioned");
            messageDictionary.emplace(mmiactions::IMMICustomResultParams::MMIResultMessage::ClirUnknown,
                                      "app_desktop_info_mmi_clir_unknown");
            messageDictionary.emplace(mmiactions::IMMICustomResultParams::MMIResultMessage::ClirTemporaryRestricted,
                                      "app_desktop_info_mmi_clir_temporary_restricted");
            messageDictionary.emplace(mmiactions::IMMICustomResultParams::MMIResultMessage::ClirTemporaryAllowed,
                                      "app_desktop_info_mmi_clir_temporary_allowed");
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
            messageDictionary.emplace(mmiactions::IMMICustomResultParams::MMIResultMessage::CallForwardingDisabled,
                                      "app_desktop_info_mmi_call_forwarding_disabled");
            messageDictionary.emplace(mmiactions::IMMICustomResultParams::MMIResultMessage::CallBarringActivated,
                                      "app_desktop_info_mmi_call_barring_activated");
            messageDictionary.emplace(mmiactions::IMMICustomResultParams::MMIResultMessage::CallBarringDeactivated,
                                      "app_desktop_info_mmi_call_barring_deactivated");
            messageDictionary.emplace(mmiactions::IMMICustomResultParams::MMIResultMessage::ClipActivted,
                                      "app_desktop_info_mmi_clip_activated");
            messageDictionary.emplace(mmiactions::IMMICustomResultParams::MMIResultMessage::ClipDeactivated,
                                      "app_desktop_info_mmi_clip_deactivated");
            messageDictionary.emplace(mmiactions::IMMICustomResultParams::MMIResultMessage::ClipNotProvisioned,
                                      "app_desktop_info_mmi_clip_not_provisioned");
            messageDictionary.emplace(mmiactions::IMMICustomResultParams::MMIResultMessage::ClipProvisioned,
                                      "app_desktop_info_mmi_clip_provisioned");
            messageDictionary.emplace(mmiactions::IMMICustomResultParams::MMIResultMessage::ClipUnknown,
                                      "app_desktop_info_mmi_clip_unknown");
            messageDictionary.emplace(mmiactions::IMMICustomResultParams::MMIResultMessage::CallWaitingActivated,
                                      "app_desktop_info_mmi_call_waiting_activated");
            messageDictionary.emplace(mmiactions::IMMICustomResultParams::MMIResultMessage::CallWaitingDeactivated,
                                      "app_desktop_info_mmi_call_waiting_deactivated");
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
            displayMessage += utils::translate("app_desktop_info_mmi_call_forwarding") + "\n";
            if (customResult.getMessageType() == mmiactions::IMMICustomResultParams::MMIType::CallForwardingData) {
                auto [voice, fax, sync, async] = customResult.getData();
                displayMessage += utils::translate("app_desktop_info_mmi_common_voice") + ": " + voice + "\n" +
                                  utils::translate("app_desktop_info_mmi_common_fax") + ": " + fax + "\n" +
                                  utils::translate("app_desktop_info_mmi_common_sync") + ": " + sync + "\n" +
                                  utils::translate("app_desktop_info_mmi_common_async") + ": " + async + "\n";
            }
            else if (customResult.getMessageType() ==
                     mmiactions::IMMICustomResultParams::MMIType::CallForwardingNotification) {
                displayMessage += getSelectedMessagesFromDictionary(customResult.getMessage());
            }
        }

        virtual void visit(mmiactions::MMICallBarringResult &customResult, std::string &displayMessage)
        {
            displayMessage += utils::translate("app_desktop_info_mmi_call_barring") + "\n";
            if (customResult.getMessageType() == mmiactions::IMMICustomResultParams::MMIType::CallBarringData) {
                displayMessage += getCustomMessagesFromDictionary(customResult.getMessages());
            }
            else if (customResult.getMessageType() ==
                     mmiactions::IMMICustomResultParams::MMIType::CallBarringNotification) {
                displayMessage += getSelectedMessagesFromDictionary(customResult.getMessage());
            }
        }

        virtual void visit(mmiactions::MMICallWaitingResult &customResult, std::string &displayMessage)
        {
            displayMessage += utils::translate("app_desktop_info_mmi_call_waiting") + "\n";
            if (customResult.getMessageType() == mmiactions::IMMICustomResultParams::MMIType::CallWaitingData) {
                displayMessage += getCustomMessagesFromDictionary(customResult.getMessages());
            }
            else if (customResult.getMessageType() ==
                     mmiactions::IMMICustomResultParams::MMIType::CallWaitingNotification) {
                displayMessage += getSelectedMessagesFromDictionary(customResult.getMessage());
            }
        }

        virtual void visit(mmiactions::MMIClipResult &customResult, std::string &displayMessage)
        {
            displayMessage += utils::translate("app_desktop_info_mmi_clip") + "\n";
            displayMessage += getSelectedMessagesFromDictionary(customResult.getMessage());
        }

        virtual void visit(mmiactions::MMIClirResult &customResult, std::string &displayMessage)
        {
            displayMessage += utils::translate("app_desktop_info_mmi_clir") + "\n";
            displayMessage += getSelectedMessagesFromDictionary(customResult.getMessage());
        }

        virtual void visit(mmiactions::MMIImeiResult &customResult, std::string &displayMessage)
        {
            displayMessage += utils::translate("app_desktop_info_mmi_imei") + "\n";
            displayMessage += customResult.getImei() + "\n";
            displayMessage += getSelectedMessagesFromDictionary(customResult.getMessage());
        }

        std::string getCustomMessagesFromDictionary(
            std::vector<std::pair<mmiactions::IMMICustomResultParams::MMIResultMessage,
                                  mmiactions::IMMICustomResultParams::MMIResultMessage>> msgData)
        {
            std::string queryStr;
            for (auto msg : msgData) {
                auto serviceClass = msg.first;
                auto serviceState = msg.second;

                auto it = messageDictionary.find(serviceState);
                if (messageDictionary.end() != it) {
                    if (queryStr.empty()) {
                        queryStr += utils::translate(it->second) + "\n";
                    }
                }
                it = messageDictionary.find(serviceClass);
                if (messageDictionary.end() != it) {
                    queryStr += utils::translate(it->second) + "\n";
                }
            }
            return queryStr;
        }

        UTF8 getSelectedMessagesFromDictionary(
            const std::vector<mmiactions::IMMICustomResultParams::MMIResultMessage> &msgList)
        {
            UTF8 selectedMessages = "";
            for (const auto &msg : msgList) {
                auto it = messageDictionary.find(msg);
                if (messageDictionary.end() != it) {
                    selectedMessages += utils::translate(it->second) + "\n";
                }
            }
            return selectedMessages;
        }

      private:
        std::map<mmiactions::IMMICustomResultParams::MMIResultMessage, std::string> messageDictionary;
    };

} // namespace mmi
