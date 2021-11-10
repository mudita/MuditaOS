// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <ApplicationCallLog.hpp>
#include <Interface/CalllogRecord.hpp>
#include <OptionWindow.hpp>

namespace gui
{

    class CalllogWindowOptions : public OptionWindow
    {
      public:
        explicit CalllogWindowOptions(app::ApplicationCommon *app, std::string windowName);
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

      private:
        enum class ValidCodes
        {
            Uninitialized,
            Invalid,
            Valid
        };

        CalllogRecord record;
        ValidCodes recordValidCode{ValidCodes::Uninitialized};

        std::list<Option> calllogWindowOptions(app::ApplicationCallLog *app, const CalllogRecord &record);
        ValidCodes getRecordValid();
    };

} // namespace gui
