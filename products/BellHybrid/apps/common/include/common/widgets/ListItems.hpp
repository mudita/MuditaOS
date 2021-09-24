// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <common/models/AbstractSettingsModel.hpp>
#include <common/widgets/BellSideListItemWithCallbacks.hpp>

#include <apps-common/widgets/spinners/SpinnerContents.hpp>
#include <apps-common/widgets/spinners/Spinners.hpp>
#include <i18n/i18n.hpp>

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

      private:
        UIntegerSpinner *spinner{};
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
    };

    class UTF8ListItem : public BellSideListItemWithCallbacks
    {
      public:
        explicit UTF8ListItem(AbstractSettingsModel<UTF8> &model,
                              UTF8Spinner::Range range,
                              const std::string &topDescription = "");

      private:
        UTF8Spinner *spinner{};
    };
} // namespace gui
