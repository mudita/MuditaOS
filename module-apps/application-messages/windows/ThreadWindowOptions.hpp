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
        std::shared_ptr<ThreadRecord> thread;
        gui::option::OptionRecordValidity threadValidCode{gui::option::OptionRecordValidity::Uninitialized};

        std::list<Option> threadWindowOptions(app::ApplicationMessages *app, const ThreadRecord *record);
        gui::option::OptionRecordValidity getThreadValid();
    };

} // namespace gui
