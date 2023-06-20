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

        static CrashdumpMetadata &getInstance();

        void setSerialNumber(const std::string &serialNumber);
        const std::string &getSerialNumber();

        void setProductName(const std::string &product);
        const std::string &getProductName();

        void setOsVersion(const std::string &osVersion);
        const std::string &getOsVersion();

        void setCommitHash(const std::string &hash);
        const std::string &getCommitHash();

        std::string getMetadataString();

      private:
        CrashdumpMetadata();

        std::string serialNumber;
        std::string productName;
        std::string osVersion;
        std::string commitHash;
    };
} // namespace Store
