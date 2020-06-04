#pragma once

#include "Application.hpp"

#include <string>

namespace gui
{
    namespace window::name
    {
        inline const std::string contact            = "Contact";
        inline const std::string contact_options    = "Options";
        inline const std::string duplicated_contact = "DuplicatedContactWindow";
        inline const std::string no_results         = "NoResults";
        inline const std::string contact_blocked    = "ContactBlocked";
        inline const std::string namecard_options   = "Namecard Options";
        inline const std::string new_contact        = "New";
        inline const std::string search             = "Search";
        inline const std::string search_results     = "SearchResults";
        inline const std::string dialog_yes_no      = "DialogYesNo";
    } // namespace window::name
} // namespace gui

namespace app
{

    /* this is is a bad idea but no other exists as of 06.02.2020 */
    static std::string defaultCountryCode = "+48";

    const inline std::string name_phonebook = "ApplicationPhonebook";

    class ApplicationPhonebook : public app::Application
    {
      public:
        ApplicationPhonebook(std::string name = name_phonebook, std::string parent = "", bool startBackgound = false);
        virtual ~ApplicationPhonebook();

        sys::Message_t DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;
        sys::ReturnCodes InitHandler() override;
        sys::ReturnCodes DeinitHandler() override;

        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final
        {
            return sys::ReturnCodes::Success;
        }

        void createUserInterface() override;
        void destroyUserInterface() override;
    };

} // namespace app
