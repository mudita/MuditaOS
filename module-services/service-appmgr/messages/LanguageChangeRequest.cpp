// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "LanguageChangeRequest.hpp"

#include <utility>

namespace app::manager
{
    LanguageChangeRequest::LanguageChangeRequest(const ApplicationName &senderName, Language &&language)
        : BaseMessage(MessageType::APMChangeLanguage, senderName), language{std::move(language)}
    {}

    [[nodiscard]] auto LanguageChangeRequest::getLanguage() const -> Language
    {
        return language;
    }

    DisplayLanguageChangeRequest::DisplayLanguageChangeRequest(const ApplicationName &senderName, Language language)
        : LanguageChangeRequest(senderName, std::move(language))
    {}

    InputLanguageChangeRequest::InputLanguageChangeRequest(const ApplicationName &senderName, Language language)
        : LanguageChangeRequest(senderName, std::move(language))
    {}
} // namespace app::manager
