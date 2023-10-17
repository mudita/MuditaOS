// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <common/models/AbstractSettingsModel.hpp>
#include <common/widgets/BellSideListItemWithCallbacks.hpp>

#include <apps-common/widgets/spinners/Spinners.hpp>
#include <apps-common/widgets/TimeSetFmtSpinner.hpp>

#include <i18n/i18n.hpp>
#include <time/dateCommon.hpp>

namespace gui
{

    class OnOffListItem : public BellSideListItemWithCallbacks
    {
      public:
        explicit OnOffListItem(AbstractSettingsModel<bool> &model, const std::string &topDescription = "");

        bool isActive() const;

      private:
        void setArrowsVisibility();

        const std::string onStr;
        const std::string offStr;
        StringSpinner *spinner;
    };

    class TimeListItem : public BellSideListItemWithCallbacks
    {
      public:
        explicit TimeListItem(AbstractSettingsModel<time_t> &model,
                              utils::time::Locale::TimeFormat fmt,
                              const std::string &focusFont,
                              const std::string &noFocusFont,
                              const std::string &topDescription = "");

      private:
        TimeSetFmtSpinner *spinner{};
    };
} // namespace gui
