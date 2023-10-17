// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <CrashdumpMetadataStore.hpp>

using namespace Store;

/* This has to be the first test case! */
TEST_CASE("Crashdump metadata store defaults")
{
    static constexpr auto defaultSerialNumber = "0000000000000";
    static constexpr auto defaultOsVersion    = "0.0.0";
    static constexpr auto defaultCommitHash   = "00000000";
    static constexpr auto defaultProductName  = "unknown";

    REQUIRE(CrashdumpMetadata::getInstance().getSerialNumber() == defaultSerialNumber);
    REQUIRE(CrashdumpMetadata::getInstance().getOsVersion() == defaultOsVersion);
    REQUIRE(CrashdumpMetadata::getInstance().getCommitHash() == defaultCommitHash);
    REQUIRE(CrashdumpMetadata::getInstance().getProductName() == defaultProductName);
}

TEST_CASE("Crashdump metadata store operations")
{
    static constexpr auto serialNumber1 = "1234567892137";
    static constexpr auto serialNumber2 = "2010307060900";

    static constexpr auto osVersion1 = "6.6.6";
    static constexpr auto osVersion2 = "9.9.9";

    static constexpr auto commitHash1 = "baadcafe";
    static constexpr auto commitHash2 = "c00lbabe";

    static constexpr auto productName1 = "pure";
    static constexpr auto productName2 = "bell";

    static constexpr auto separator = "_";

    SECTION("Setting values")
    {
        CrashdumpMetadata::getInstance().setSerialNumber(serialNumber1);
        REQUIRE(CrashdumpMetadata::getInstance().getSerialNumber() == serialNumber1);

        CrashdumpMetadata::getInstance().setOsVersion(osVersion1);
        REQUIRE(CrashdumpMetadata::getInstance().getOsVersion() == osVersion1);

        CrashdumpMetadata::getInstance().setCommitHash(commitHash1);
        REQUIRE(CrashdumpMetadata::getInstance().getCommitHash() == commitHash1);

        CrashdumpMetadata::getInstance().setProductName(productName1);
        REQUIRE(CrashdumpMetadata::getInstance().getProductName() == productName1);
    }

    SECTION("Updating values")
    {
        CrashdumpMetadata::getInstance().setSerialNumber(serialNumber2);
        REQUIRE(CrashdumpMetadata::getInstance().getSerialNumber() == serialNumber2);

        CrashdumpMetadata::getInstance().setOsVersion(osVersion2);
        REQUIRE(CrashdumpMetadata::getInstance().getOsVersion() == osVersion2);

        CrashdumpMetadata::getInstance().setCommitHash(commitHash2);
        REQUIRE(CrashdumpMetadata::getInstance().getCommitHash() == commitHash2);

        CrashdumpMetadata::getInstance().setProductName(productName2);
        REQUIRE(CrashdumpMetadata::getInstance().getProductName() == productName2);
    }

    SECTION("Stringifying values")
    {
        const auto realMetadataStringified = CrashdumpMetadata::getInstance().getMetadataString();
        const auto referenceMetadataStringified =
            std::string(productName2) + separator + osVersion2 + separator + commitHash2 + separator + serialNumber2;
        REQUIRE(realMetadataStringified == referenceMetadataStringified);
    }
}
