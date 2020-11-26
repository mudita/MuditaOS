// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-appmgr/service-appmgr/Actions.hpp>

namespace app::manager::actions
{

    class MMIParams : public ActionParams
    {
      public:
        explicit MMIParams(std::string mmiData);

        [[nodiscard]] std::string getData() const;

      private:
        std::string mmiData;
    };

    class MMIResultParams : public ActionParams
    {
      public:
        enum class MMIResult
        {
            Success,
            Failed
        };
        explicit MMIResultParams(MMIResult result);
        [[nodiscard]] MMIResult getData() const noexcept;

      private:
        MMIResult result;
    };
} // namespace app::manager::actions
