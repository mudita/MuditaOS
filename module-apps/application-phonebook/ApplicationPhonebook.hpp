#pragma once

#include "Application.hpp"

#include <string>

namespace gui::window::name
{
    inline const std::string contact          = "Contact";
    inline const std::string contact_options  = "Options";
    inline const std::string namecard_options = "Namecard Options";
    inline const std::string new_contact      = "New";
    inline const std::string search           = "Search";
    inline const std::string search_results   = "SearchResults";
    inline const std::string dialog_yes_no    = "DialogYesNo";
    inline const std::string dialog_confirm   = "DialogConfirm";
    inline const std::string dialog           = "Dialog";
    inline const std::string dialog_yes_no_icon_txt = "DialogYesNoIconTxt";
    inline const std::string ice_contacts           = "IceContacts";

} // namespace gui::window::name

namespace app
{
    const inline std::string name_phonebook      = "ApplicationPhonebook";
    constexpr std::uint16_t phonebook_stack_size = 8192;

    class ApplicationPhonebook : public app::Application
    {
      public:
        ApplicationPhonebook(std::string name = name_phonebook, std::string parent = "", bool startBackgound = false);
        ~ApplicationPhonebook() override = default;

        auto DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) -> sys::Message_t override;
        auto InitHandler() -> sys::ReturnCodes override;
        auto DeinitHandler() -> sys::ReturnCodes override;

        auto SwitchPowerModeHandler(const sys::ServicePowerMode mode) -> sys::ReturnCodes final
        {
            return sys::ReturnCodes::Success;
        }

        void createUserInterface() override;
        void destroyUserInterface() override;

        void onSearchRequest(const std::string &searchFilter);
        bool searchEmpty(const std::string &query);
    };

} // namespace app
