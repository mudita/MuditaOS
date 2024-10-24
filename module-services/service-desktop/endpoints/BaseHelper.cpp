// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include <endpoints/BaseHelper.hpp>

namespace sdesktop::endpoints
{
    auto ret() -> BaseHelper::ProcessResult
    {
        return {Sent::No, std::nullopt};
    }

    auto BaseHelper::processPut(Context &) -> ProcessResult
    {
        return ret();
    }

    auto BaseHelper::processGet(Context &) -> ProcessResult
    {
        return ret();
    }

    auto BaseHelper::processPost(Context &) -> ProcessResult
    {
        return ret();
    }

    auto BaseHelper::processDelete(Context &) -> ProcessResult
    {
        return ret();
    }

    [[nodiscard]] auto BaseHelper::process(http::Method method, Context &context) -> ProcessResult
    {
        preProcess(method, context);
        switch (method) {
        case http::Method::Del:
            return processDelete(context);
        case http::Method::Get:
            return processGet(context);
        case http::Method::Post:
            return processPost(context);
        case http::Method::Put:
            return processPut(context);
        }
        postProcess(method, context);
        return {Sent::No, std::nullopt};
    }
} // namespace sdesktop::endpoints
