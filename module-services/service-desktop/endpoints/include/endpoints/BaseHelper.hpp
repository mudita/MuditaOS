// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/HttpEnums.hpp>
#include <endpoints/ResponseContext.hpp>

#include <optional>

namespace sys
{
    class Service;
} // namespace sys

namespace sdesktop::endpoints
{

    class Context;
    enum class sent
    {
        /// response was sent from Helper class
        /// we should avoid that
        yes,
        /// response wasn't sent from Helper class
        /// we should send it from endpoint based on processing result
        no,
        /// works on global context
        /// mechanism:
        /// - Send message which is delayed
        /// - send the actual response form ServiceDesktop on `sdesktop::developerMode::DeveloperModeRequest`
        delayed,
    };

    /// base helper class to avoid copies
    class BaseHelper
    {
      public:
        using ProcessResult = std::pair<sent, std::optional<ResponseContext>>;

      protected:
        sys::Service *owner = nullptr;
        /// by default - result = not sent
        [[nodiscard]] virtual auto processPut(Context &) -> ProcessResult;
        /// by default - result = not sent
        [[nodiscard]] virtual auto processGet(Context &) -> ProcessResult;
        /// by default - result = not sent
        [[nodiscard]] virtual auto processPost(Context &) -> ProcessResult;
        /// by default - result = not sent
        [[nodiscard]] virtual auto processDelete(Context &) -> ProcessResult;
        /// pre process action foo - in case we want to do something before processing request
        virtual void preProcess(http::Method method, Context &context){};
        /// post processing action - in case we want to do something after processing request
        virtual void postProcess(http::Method method, Context &context){};

      public:
        explicit BaseHelper(sys::Service *p) : owner(p)
        {}
        virtual ~BaseHelper() = default;

        /// generall processing function
        ///
        /// we should define processing functions, not copy switch cases
        /// as we are super ambiguous how we should really handle responses
        /// here we can either:
        /// return true - to mark that we responded on this request
        /// return fale - and optionally respond that we didn't handle the request
        /// pre and post processing is available on pre/post process method override
        [[nodiscard]] auto process(http::Method method, Context &context) -> ProcessResult;
    };

} // namespace sdesktop::endpoints
