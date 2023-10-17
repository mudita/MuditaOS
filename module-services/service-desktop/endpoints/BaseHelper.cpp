// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <endpoints/BaseHelper.hpp>

namespace sdesktop::endpoints
{

    auto ret() -> BaseHelper::ProcessResult
    {
        return {sent::no, std::nullopt};
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
        case http::Method::del:
            return processDelete(context);
        case http::Method::get:
            return processGet(context);
        case http::Method::post:
            return processPost(context);
        case http::Method::put:
            return processPut(context);
        }
        postProcess(method, context);
        return {sent::no, std::nullopt};
    }

} // namespace sdesktop::endpoints
