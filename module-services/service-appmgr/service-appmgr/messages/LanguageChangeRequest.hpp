// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BaseMessage.hpp"

#include <i18/i18.hpp>

namespace app::manager
{
    class LanguageChangeRequest : public BaseMessage
    {
      public:
        [[nodiscard]] auto getLanguage() const noexcept -> utils::Lang;

      protected:
        LanguageChangeRequest(const ApplicationName &senderName, utils::Lang language);

      private:
        utils::Lang language;
    };

    /// Requests to change the display language.
    class DisplayLanguageChangeRequest : public LanguageChangeRequest
    {
      public:
        DisplayLanguageChangeRequest(const ApplicationName &senderName, utils::Lang language);
    };

    /// Requests to change the input language.
    class InputLanguageChangeRequest : public LanguageChangeRequest
    {
      public:
        InputLanguageChangeRequest(const ApplicationName &senderName, utils::Lang language);
    };
} // namespace app::manager
