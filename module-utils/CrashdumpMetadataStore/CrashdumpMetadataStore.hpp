// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>

namespace Store
{
    struct CrashdumpMetadata
    {
      public:
        CrashdumpMetadata(const CrashdumpMetadata &) = delete;
        CrashdumpMetadata &operator=(const CrashdumpMetadata &) = delete;

        [[nodiscard]] static CrashdumpMetadata &getInstance();

        void setSerialNumber(const std::string &serialNumber);
        [[nodiscard]] const std::string &getSerialNumber();

        void setProductName(const std::string &product);
        [[nodiscard]] const std::string &getProductName();

        void setOsVersion(const std::string &osVersion);
        [[nodiscard]] const std::string &getOsVersion();

        void setCommitHash(const std::string &hash);
        [[nodiscard]] const std::string &getCommitHash();

        [[nodiscard]] std::string getMetadataString();

      private:
        CrashdumpMetadata();

        std::string serialNumber;
        std::string productName;
        std::string osVersion;
        std::string commitHash;
    };
} // namespace Store
