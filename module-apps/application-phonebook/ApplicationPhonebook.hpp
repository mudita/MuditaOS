#pragma once

#include "Application.hpp"
#include <Interface/ContactRecord.hpp>
#include <OptionWindow.hpp>


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

        gui::OptionWindow *windowOptions = nullptr;

        sys::Message_t DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;
        sys::ReturnCodes InitHandler() override;
        sys::ReturnCodes DeinitHandler() override;

        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final
        {
            return sys::ReturnCodes::Success;
        }

        void createUserInterface() override;
        void destroyUserInterface() override;

        bool blockContact(const uint32_t contactId);
        bool removeContact(const uint32_t contactId);
    };

} // namespace app
