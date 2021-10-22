// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DarkModeModel.hpp"

#include <service-eink/Common.hpp>
#include <service-eink/api/ServiceEinkApi.hpp>
#include <service-eink/messages/EinkModeMessage.hpp>

namespace dark_mode
{
    DarkModeModel::DarkModeModel(app::ApplicationCommon *application)
        : app::AsyncCallbackReceiver{application}, application{application}
    {}

    void DarkModeModel::enable(std::function<void()> &&onSuccess)
    {
        switchMode(true, std::move(onSuccess));
    }

    void DarkModeModel::disable(std::function<void()> &&onSuccess)
    {
        switchMode(false, std::move(onSuccess));
    }

    void DarkModeModel::switchMode(bool enableDarkMode, std::function<void()> &&onSuccess)
    {
        using service::eink::EinkModeMessage;
        auto request = app::AsyncRequest::createFromMessage(
            std::make_unique<EinkModeMessage>(enableDarkMode ? EinkModeMessage::Mode::Invert
                                                             : EinkModeMessage::Mode::Normal),
            service::name::eink);
        request->execute(application, this, [this, enableDarkMode, onSuccess](auto response) {
            using service::eink::EinkModeResponse;
            if (auto einkModeResponse = static_cast<EinkModeResponse *>(response);
                einkModeResponse->retCode != sys::ReturnCodes::Success) {
                return false;
            }

            enabled = enableDarkMode;
            if (onSuccess) {
                onSuccess();
            }
            return true;
        });
    }

    bool DarkModeModel::isEnabled() const noexcept
    {
        return service::eink::api::isDarkModeEnabled();
    }
} // namespace dark_mode
