// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Service.hpp>
#include "startupIndexer.hpp"

namespace service
{

    class ServiceFileIndexer final : public sys::Service
    {
      public:
        ServiceFileIndexer(const std::string_view name);
        virtual ~ServiceFileIndexer()                  = default;
        ServiceFileIndexer(const ServiceFileIndexer &) = delete;
        ServiceFileIndexer &operator=(const ServiceFileIndexer &) = delete;
        sys::Message_t DataReceivedHandler(sys::DataMessage *msg, sys::ResponseMessage *resp) override;
        sys::ReturnCodes InitHandler() override;
        sys::ReturnCodes DeinitHandler() override;
        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override;

      private:
        auto onDeleteFile(std::string_view path) -> void;
        auto onRenameFile(std::string_view oldPath, std::string_view newPath) -> void;
        auto onAudioContentChanged(std::string_view path) -> void;
        auto onTextContentChanged(std::string_view path) -> void;
        detail::startupIndexer mStartupIndexer;
    };

}; // namespace service
