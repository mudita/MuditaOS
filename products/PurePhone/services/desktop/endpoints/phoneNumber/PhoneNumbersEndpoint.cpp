// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <endpoints/phoneNumber/PhoneNumbersEndpoint.hpp>

namespace sdesktop::endpoints
{

    auto PhoneNumbersEndpoint::handle(Context &context) -> void
    {
        switch (context.getMethod()) {
        case http::Method::get:
            helper->requestDataFromDB(context);
            break;
        default:
            context.setResponseStatus(http::Code::BadRequest);
            break;
        }
    }

} // namespace sdesktop::endpoints
