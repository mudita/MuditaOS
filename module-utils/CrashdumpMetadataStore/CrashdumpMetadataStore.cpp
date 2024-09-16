// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "CrashdumpMetadataStore.hpp"

namespace
{
    constexpr auto separator                 = "_";
    constexpr auto defaultSerialNumberLength = 13;
    constexpr auto defaultCommitHashLength   = 8;
    constexpr auto defaultProductName        = "unknown";
    constexpr auto defaultOsVersion          = "0.0.0";
    constexpr auto defaultFillValue          = '0';
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
