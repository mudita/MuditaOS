// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <common/models/AbstractSettingsModel.hpp>
#include <common/widgets/BellSideListItemWithCallbacks.hpp>

#include <apps-common/widgets/spinners/SpinnerContents.hpp>
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
        const UTF8 onStr;
        const UTF8 offStr;
        UTF8Spinner *spinner{};
    };

    class NumListItem : public BellSideListItemWithCallbacks
    {
      public:
        explicit NumListItem(AbstractSettingsModel<std::uint8_t> &model,
                             UIntegerSpinner::Range range,
                             const std::string &topDescription    = "",
                             const std::string &bottomDescription = "");

        void setOnValueChanged(std::function<void(const UIntegerSpinner::Type &)> &&cb);
        UIntegerSpinner::Type getCurrentValue();

      private:
        UIntegerSpinner *spinner{};

        void setArrowsVisibility(UIntegerSpinner::Range range);
    };

    class NumWithStringListItem : public BellSideListItemWithCallbacks
    {
      public:
        using Value                = NumWithString<std::uint32_t, UTF8>;
        using NumWithStringSpinner = GenericSpinner<StringPolicy<Value>>;

        explicit NumWithStringListItem(AbstractSettingsModel<std::uint8_t> &model,
                                       NumWithStringSpinner::Range range,
                                       const std::string &topDescription    = "",
                                       const std::string &bottomDescription = "");

        bool isOff() const;

      private:
        NumWithStringSpinner *spinner{};
        TextFixedSize *bottomText{};
        const UTF8 offStr;
        const UTF8 minStr;

        void setArrowsVisibility(const NumWithStringSpinner::Range &range);
    };

    class UTF8ListItem : public BellSideListItemWithCallbacks
    {
      public:
        explicit UTF8ListItem(AbstractSettingsModel<UTF8> &model,
                              UTF8Spinner::Range range,
                              const std::string &topDescription = "");

        void setOnValueChanged(std::function<void(const UTF8 &)> &&cb);
        UTF8Spinner::Type getCurrentValue();

      private:
        UTF8Spinner *spinner{};
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
