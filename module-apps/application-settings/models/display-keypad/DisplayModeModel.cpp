// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DisplayModeModel.hpp"

#include <AsyncTask.hpp>

#include <service-eink/Common.hpp>
#include <service-eink/api/ServiceEinkApi.hpp>
#include <service-eink/messages/EinkModeMessage.hpp>

namespace display_mode
{
    namespace
    {
        service::eink::EinkModeMessage::Mode toEinkMode(Mode mode) noexcept
        {
            using service::eink::EinkModeMessage;
            if (mode == Mode::Light) {
                return EinkModeMessage::Mode::Normal;
            }
            return EinkModeMessage::Mode::Invert;
        }
    } // namespace

    DisplayModeModel::DisplayModeModel(app::ApplicationCommon *application)
        : app::AsyncCallbackReceiver{application}, application{application}
    {}

    void DisplayModeModel::setMode(Mode mode, std::function<void(Mode)> &&onSuccess)
    {
        using service::eink::EinkModeMessage;
        auto request = app::AsyncRequest::createFromMessage(std::make_unique<EinkModeMessage>(toEinkMode(mode)),
                                                            service::name::eink);
        request->execute(application, this, [mode, onSuccess](auto response) {
            using service::eink::EinkModeResponse;
            if (auto einkModeResponse = static_cast<EinkModeResponse *>(response);
                einkModeResponse->retCode != sys::ReturnCodes::Success) {
                return false;
            }

            if (onSuccess) {
                onSuccess(mode);
            }
            return true;
        });
    }

    auto DisplayModeModel::isDarkModeEnabled() const noexcept -> bool
    {
        return service::eink::api::isInvertedModeEnabled();
    }
} // namespace display_mode
