// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <serial-number-parser/SerialNumberParser.hpp>

namespace
{
    namespace oldSerialNumbers
    {
        std::string grey1_FirstVersion{"0421481234567"};
        std::string grey2_FirstVersion{"0422131234567"};
        std::string black_SecondVersion{"0422421234567"};
        std::string invalidNumber{"0123456789123"};
    } // namespace oldSerialNumbers
    namespace newSerialNumbers
    {
        std::string grey_SecondVersion{"04P1G23091234"};
        std::string black_SecondVersion{"04P1B23091234"};
        std::string invalidNumber{"0123456789123"};
        std::string invalidColor{"04P1R23091234"};
    } // namespace newSerialNumbers

} // namespace

TEST_CASE("Serial number format")
{
    SECTION("Check old serial numbers")
    {
        CHECK(serial_number_parser::isOldSerialNumberFormat(oldSerialNumbers::grey1_FirstVersion));
        CHECK(serial_number_parser::isOldSerialNumberFormat(oldSerialNumbers::grey2_FirstVersion));
        CHECK(serial_number_parser::isOldSerialNumberFormat(oldSerialNumbers::black_SecondVersion));
    }
    SECTION("Check new serial numbers")
    {
        CHECK(not serial_number_parser::isOldSerialNumberFormat(newSerialNumbers::grey_SecondVersion));
        CHECK(not serial_number_parser::isOldSerialNumberFormat(newSerialNumbers::black_SecondVersion));
    }
}

TEST_CASE("Serial number metadata")
{
    SECTION("Check case color")
    {
        SECTION("Case color for valid serial numbers")
        {
            CHECK(serial_number_parser::getDeviceVersionMetadata(oldSerialNumbers::grey1_FirstVersion).has_value());
            CHECK(serial_number_parser::getDeviceVersionMetadata(oldSerialNumbers::grey1_FirstVersion).value().first ==
                  serial_number_parser::grayColor);

            CHECK(serial_number_parser::getDeviceVersionMetadata(oldSerialNumbers::grey2_FirstVersion).has_value());
            CHECK(serial_number_parser::getDeviceVersionMetadata(oldSerialNumbers::grey2_FirstVersion).value().first ==
                  serial_number_parser::grayColor);

            CHECK(serial_number_parser::getDeviceVersionMetadata(oldSerialNumbers::black_SecondVersion).has_value());
            CHECK(serial_number_parser::getDeviceVersionMetadata(oldSerialNumbers::black_SecondVersion).value().first ==
                  serial_number_parser::blackColor);

            CHECK(serial_number_parser::getDeviceVersionMetadata(newSerialNumbers::grey_SecondVersion).has_value());
            CHECK(serial_number_parser::getDeviceVersionMetadata(newSerialNumbers::grey_SecondVersion).value().first ==
                  serial_number_parser::grayColor);

            CHECK(serial_number_parser::getDeviceVersionMetadata(newSerialNumbers::black_SecondVersion).has_value());
            CHECK(serial_number_parser::getDeviceVersionMetadata(newSerialNumbers::black_SecondVersion).value().first ==
                  serial_number_parser::blackColor);
        }
        SECTION("Case color for invalid serial numbers")
        {
            CHECK(not serial_number_parser::getDeviceVersionMetadata(newSerialNumbers::invalidColor).has_value());
        }
    }
    SECTION("Check Harmony version")
    {
        CHECK(serial_number_parser::getDeviceVersionMetadata(oldSerialNumbers::grey1_FirstVersion).has_value());
        CHECK(serial_number_parser::getDeviceVersionMetadata(oldSerialNumbers::grey1_FirstVersion).value().second ==
              serial_number_parser::firstVersion);

        CHECK(serial_number_parser::getDeviceVersionMetadata(oldSerialNumbers::grey2_FirstVersion).has_value());
        CHECK(serial_number_parser::getDeviceVersionMetadata(oldSerialNumbers::grey2_FirstVersion).value().second ==
              serial_number_parser::firstVersion);

        CHECK(serial_number_parser::getDeviceVersionMetadata(oldSerialNumbers::black_SecondVersion).has_value());
        CHECK(serial_number_parser::getDeviceVersionMetadata(oldSerialNumbers::black_SecondVersion).value().second ==
              serial_number_parser::secondVersion);

        CHECK(serial_number_parser::getDeviceVersionMetadata(newSerialNumbers::grey_SecondVersion).has_value());
        CHECK(serial_number_parser::getDeviceVersionMetadata(newSerialNumbers::grey_SecondVersion).value().second ==
              serial_number_parser::secondVersion);

        CHECK(serial_number_parser::getDeviceVersionMetadata(newSerialNumbers::black_SecondVersion).has_value());
        CHECK(serial_number_parser::getDeviceVersionMetadata(newSerialNumbers::black_SecondVersion).value().second ==
              serial_number_parser::secondVersion);
    }
    SECTION("Check invalid serial numbers")
    {
        CHECK(not serial_number_parser::getDeviceVersionMetadata(oldSerialNumbers::invalidNumber).has_value());
        CHECK(not serial_number_parser::getDeviceVersionMetadata(newSerialNumbers::invalidNumber).has_value());
    }
}
