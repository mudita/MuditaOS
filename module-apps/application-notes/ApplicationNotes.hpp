// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef MODULE_APPS_APPLICATION_NOTES_APPLICATIONNOTES_HPP_
#define MODULE_APPS_APPLICATION_NOTES_APPLICATIONNOTES_HPP_

#include "NotesModel.hpp"
#include "Application.hpp"

namespace app
{
    inline constexpr auto name_notes = "ApplicationNotes";

    class ApplicationNotes : public Application
    {
      public:
        ApplicationNotes(std::string name                    = name_notes,
                         std::string parent                  = {},
                         StartInBackground startInBackground = {false});
        virtual ~ApplicationNotes();
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

    template <> struct ManifestTraits<ApplicationNotes>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch}};
        }
    };
} /* namespace app */

#endif /* MODULE_APPS_APPLICATION_NOTES_APPLICATIONNOTES_HPP_ */
