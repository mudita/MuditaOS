// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/Context.hpp>

#include <Service/Service.hpp>

namespace sdesktop::endpoints
{

    class DBHelper
    {
      public:
        virtual auto createDBEntry(Context &context) -> sys::ReturnCodes     = 0;
        virtual auto requestDataFromDB(Context &context) -> sys::ReturnCodes = 0;
        virtual auto updateDBEntry(Context &context) -> sys::ReturnCodes     = 0;
        virtual auto deleteDBEntry(Context &context) -> sys::ReturnCodes     = 0;

        explicit DBHelper(sys::Service *_ownerServicePtr) : ownerServicePtr(_ownerServicePtr){};
        virtual ~DBHelper() = default;

      protected:
        sys::Service *ownerServicePtr = nullptr;
    };

} // namespace sdesktop::endpoints
