// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Message.hpp>
#include <i18n/i18n.hpp>

namespace app::manager
{
    class GetCurrentDisplayLanguageResponse : public sys::ResponseMessage
    {
      public:
        explicit GetCurrentDisplayLanguageResponse(Language language) : language(std::move(language))
        {}
        [[nodiscard]] auto getLanguage() const -> Language
        {
            return language;
        }

      private:
        Language language;
    };
} // namespace app::manager
