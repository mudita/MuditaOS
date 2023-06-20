// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CrashdumpMetadataStore.hpp"

namespace
{
    inline constexpr auto separator                 = "_";
    inline constexpr auto defaultSerialNumberLength = 13;
    inline constexpr auto defaultCommitHashLength   = 8;
    inline constexpr auto defaultProductName        = "unknown";
    inline constexpr auto defaultOsVersion          = "0.0.0";
    inline constexpr auto defaultFillValue          = '0';
} // namespace

namespace Store
{
    CrashdumpMetadata::CrashdumpMetadata() : productName(defaultProductName), osVersion(defaultOsVersion)
    {
        serialNumber.insert(0, defaultSerialNumberLength, defaultFillValue);
        commitHash.insert(0, defaultCommitHashLength, defaultFillValue);
    }

    CrashdumpMetadata &CrashdumpMetadata::getInstance()
    {
        static CrashdumpMetadata instance;
        return instance;
    }

    void CrashdumpMetadata::setSerialNumber(const std::string &sn)
    {
        serialNumber = sn;
    }

    const std::string &CrashdumpMetadata::getSerialNumber()
    {
        return serialNumber;
    }

    void CrashdumpMetadata::setProductName(const std::string &name)
    {
        productName = name;
    }

    const std::string &CrashdumpMetadata::getProductName()
    {
        return productName;
    }

    void CrashdumpMetadata::setOsVersion(const std::string &version)
    {
        osVersion = version;
    }

    const std::string &CrashdumpMetadata::getOsVersion()
    {
        return osVersion;
    }

    void CrashdumpMetadata::setCommitHash(const std::string &hash)
    {
        commitHash = hash;
    }

    const std::string &CrashdumpMetadata::getCommitHash()
    {
        return commitHash;
    }

    std::string CrashdumpMetadata::getMetadataString()
    {
        return productName + separator + osVersion + separator + commitHash + separator + serialNumber;
    }
} // namespace Store
