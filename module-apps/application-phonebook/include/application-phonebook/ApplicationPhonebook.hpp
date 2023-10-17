// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Application.hpp"

#include <string>
#include <application-phonebook/models/SearchRequestModel.hpp>
#include <application-phonebook/models/PhonebookModel.hpp>

namespace gui::window::name
{
    inline constexpr auto contact                 = "Contact";
    inline constexpr auto contact_options         = "Options";
    inline constexpr auto namecard_options        = "Namecard Options";
    inline constexpr auto input_options           = "Input Options";
    inline constexpr auto new_contact             = "New";
    inline constexpr auto search                  = "Search";
    inline constexpr auto multiple_numbers_select = "SelectNumber";
    inline constexpr auto search_results          = "SearchResults";
    inline constexpr auto dialog_yes_no           = "DialogYesNo";
    inline constexpr auto dialog                  = "Dialog";
    inline constexpr auto dialog_yes_no_icon_txt  = "DialogYesNoIconTxt";
    inline constexpr auto ice_contacts            = "IceContacts";

} // namespace gui::window::name

namespace app
{
    inline constexpr auto name_phonebook         = "ApplicationPhonebook";
    constexpr std::uint16_t phonebook_stack_size = 1024 * 5;

    class ApplicationPhonebook : public app::Application
    {
        std::shared_ptr<SearchRequestModel> searchRequestModel;
        std::shared_ptr<PhonebookModel> phonebookModel;

      public:
        explicit ApplicationPhonebook(std::string name                    = name_phonebook,
                                      std::string parent                  = {},
                                      StatusIndicators statusIndicators   = StatusIndicators{},
                                      StartInBackground startInBackground = {false});
        ~ApplicationPhonebook() override = default;

        auto DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) -> sys::MessagePointer override;
        auto InitHandler() -> sys::ReturnCodes override;

        auto SwitchPowerModeHandler(const sys::ServicePowerMode mode) -> sys::ReturnCodes final
        {
            return sys::ReturnCodes::Success;
        }

        void createUserInterface() override;
        void destroyUserInterface() override;

        void onSearchRequest(const std::string &searchFilter);
    };

    template <>
    struct ManifestTraits<ApplicationPhonebook>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch,
                     manager::actions::ShowContacts,
                     manager::actions::AddContact,
                     manager::actions::EditContact,
                     manager::actions::ShowContactDetails,
                     manager::actions::ShowEmergencyContacts,
                     manager::actions::PhoneModeChanged,
                     manager::actions::BluetoothModeChanged,
                     manager::actions::AlarmClockStatusChanged}};
        }
    };
} // namespace app
