// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ApplicationMessages.hpp"

#include <OptionWindow.hpp>
#include <Interface/ThreadRecord.hpp>

namespace gui
{

    class ThreadWindowOptions : public OptionWindow
    {
      public:
        explicit ThreadWindowOptions(app::ApplicationCommon *app, std::string windowName);
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

      private:
        enum class ValidCodes
        {
            Uninitialized,
            Invalid,
            Valid
        };
        std::shared_ptr<ThreadRecord> thread;
        ValidCodes threadValidCode{ValidCodes::Uninitialized};

        std::list<Option> threadWindowOptions(app::ApplicationMessages *app, const ThreadRecord *record);
        ValidCodes getThreadValid();
    };

} // namespace gui
