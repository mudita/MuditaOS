// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Result.hpp>

namespace at
{
    class ResultParser
    {
      public:
        ResultParser()
        {}

        std::shared_ptr<Result> parse();

        bool appendPartialResult(const std::vector<std::string> &part);
        void setStatusCode(at::Result::StatusCode code);

        const std::vector<std::string> &getResponse();

      private:
        enum class CmxErrorType
        {
            CmeError,
            CmsError,
        };
        static constexpr auto CME_ERROR = "+CME ERROR:";
        static constexpr auto CMS_ERROR = "+CMS ERROR:";

        static constexpr auto STATUS_ERROR = "ERROR";
        static constexpr auto STATUS_OK    = "OK";

        bool checkError(const std::vector<std::string> &partialResult);
        bool checkCmxError(CmxErrorType errorType, const std::vector<std::string> &partialResult);
        bool checkAtError(const std::vector<std::string> &partialResult);

        std::vector<std::string> response;
        std::optional<at::Result::StatusCode> statusCode;
        std::variant<at::EquipmentErrorCode, at::NetworkErrorCode> errorCode = at::EquipmentErrorCode::NoInformation;

        uint32_t cmxError = 0;
    };
} // namespace at
