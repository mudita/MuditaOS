// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-appmgr/messages/LanguageChangeRequest.hpp>

namespace app::manager
{
    LanguageChangeRequest::LanguageChangeRequest(const ApplicationName &senderName, utils::Lang language)
        : BaseMessage(MessageType::APMChangeLanguage, senderName), language{language}
    {}

    [[nodiscard]] auto LanguageChangeRequest::getLanguage() const noexcept -> utils::Lang
    {
        return language;
    }

    DisplayLanguageChangeRequest::DisplayLanguageChangeRequest(const ApplicationName &senderName, utils::Lang language)
        : LanguageChangeRequest(senderName, language)
    {}

    InputLanguageChangeRequest::InputLanguageChangeRequest(const ApplicationName &senderName, utils::Lang language)
        : LanguageChangeRequest(senderName, language)
    {}
} // namespace app::manager
