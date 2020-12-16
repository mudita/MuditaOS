// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BaseMessage.hpp"

#include <i18n/i18n.hpp>

namespace app::manager
{
    class LanguageChangeRequest : public BaseMessage
    {
      public:
        [[nodiscard]] auto getLanguage() const -> Language;

      protected:
        LanguageChangeRequest(const ApplicationName &senderName, Language &&language);

      private:
        Language language;
    };

    /// Requests to change the display language.
    class DisplayLanguageChangeRequest : public LanguageChangeRequest
    {
      public:
        DisplayLanguageChangeRequest(const ApplicationName &senderName, Language language);
    };

    /// Requests to change the input language.
    class InputLanguageChangeRequest : public LanguageChangeRequest
    {
      public:
        InputLanguageChangeRequest(const ApplicationName &senderName, Language language);
    };
} // namespace app::manager
