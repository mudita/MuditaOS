// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "personalization_data_integration/PersonalizationFileValidation.hpp"
#include <filesystem>

class FileValidatorTestWrapper : public FileValidator
{
  public:
    json11::Json test_getJsonObject(const std::filesystem::path &fileToLoad)
    {
        return getJsonObject(fileToLoad);
    }

    unsigned long test_ReadCRC(const std::filesystem::path &filePath)
    {
        return readCRC(filePath);
    }

    bool test_validateFile(const std::filesystem::path &filePath)
    {
        return validateFile(filePath);
    }
};

TEST_CASE("Personalization File Validation")
{
    SECTION("Check ParamValidator class")
    {
        /// Expected serial number
        auto expectedSerialNumber = "qwerty123456";

        /// Valid params:
        std::string testSerialNumber = "qwerty123456";
        std::string testCaseColour   = "black";

        auto personalizationTestObj =
            json11::Json::object{{phone_personalization::json::serial_number, testSerialNumber.c_str()},
                                 {phone_personalization::json::case_color, testCaseColour.c_str()}};

        SECTION("Mandatory Parameter valid")
        {
            auto serialNumberValidValue = std::vector<std::string>({expectedSerialNumber});
            auto paramValidator =
                ParamValidator(phone_personalization::json::serial_number, true, serialNumberValidValue);
            REQUIRE(paramValidator.validate(personalizationTestObj) == ValidationResult::valid);
        }

        SECTION("Optional Parameter valid")
        {
            std::vector<std::string> caseColourValidValues = {"white", "black"};

            auto paramValidator = ParamValidator(phone_personalization::json::case_color, false, caseColourValidValues);
            REQUIRE(paramValidator.validate(personalizationTestObj) == ValidationResult::valid);
        }
    }

    SECTION("Check ParamValidator class invalid cases")
    {
        /// Expected serial number
        auto expectedSerialNumber = "qwerty123456";

        /// Valid params:
        std::string testSerialNumber = "";
        std::string testCaseColour   = "yellow";

        auto personalizationTestObj =
            json11::Json::object{{phone_personalization::json::serial_number, testSerialNumber.c_str()},
                                 {phone_personalization::json::case_color, testCaseColour.c_str()}};

        SECTION("Mandatory parameter invalid")
        {
            auto serialNumberValidValue = std::vector<std::string>({expectedSerialNumber});
            auto paramValidator =
                ParamValidator(phone_personalization::json::serial_number, true, serialNumberValidValue);
            REQUIRE(paramValidator.validate(personalizationTestObj) == ValidationResult::critical);
        }

        SECTION("Optional parameter invalid")
        {
            std::vector<std::string> caseColourValidValues = {"white", "black"};

            auto paramValidator = ParamValidator(phone_personalization::json::case_color, false, caseColourValidValues);
            REQUIRE(paramValidator.validate(personalizationTestObj) == ValidationResult::invalid);
        }
    }

    SECTION("Check FileValidator class")
    {
        auto fileValidatorWrapper = FileValidatorTestWrapper();

        SECTION("Check readCRC method")
        {
            SECTION("File with valid crc")
            {
                auto path = "/testfiles/data/personalization_files/valid/"
                            "personalization_black.json";

                unsigned long expectedCRC = 0x66409235;
                auto crc                  = fileValidatorWrapper.test_ReadCRC(std::string(path));

                REQUIRE(crc == expectedCRC);
            }

            SECTION("Invalid File")
            {
                auto path = "/testfiles/data/personalization_files/invalid/"
                            "not_existing.json";

                unsigned long CRC_invalid = 0;
                auto crc                  = fileValidatorWrapper.test_ReadCRC(std::string(path));

                REQUIRE(crc == CRC_invalid);
            }
        }

        SECTION("Check getJsonObject method")
        {
            SECTION("Valid case")
            {
                auto path = "/testfiles/data/personalization_files/valid/"
                            "personalization_black.json";

                auto expectedSerialNumber = "TestSerialNumber";
                auto expectedCaseColour   = "black";

                auto jsonObj      = fileValidatorWrapper.test_getJsonObject(std::string(path));
                auto serialNumber = jsonObj[phone_personalization::json::serial_number].string_value();
                auto caseColour   = jsonObj[phone_personalization::json::case_color].string_value();

                REQUIRE(serialNumber == expectedSerialNumber);
                REQUIRE(caseColour == expectedCaseColour);
            }

            SECTION("Wrong json format")
            {
                auto path = "/testfiles/data/personalization_files/invalid/"
                            "invalid_json_format.json";

                auto jsonObj = fileValidatorWrapper.test_getJsonObject(std::string(path));
                REQUIRE(jsonObj == nullptr);
            }
        }

        SECTION("Check test validateFile")
        {
            SECTION("Valid case")
            {
                auto path = "/testfiles/data/personalization_files/valid/"
                            "personalization_black.json";

                auto result = fileValidatorWrapper.test_validateFile(std::string(path));

                REQUIRE(result);
            }

            SECTION("File does not exist")
            {
                auto path = "/testfiles/data/personalization_files/invalid/"
                            "not_existing_file.json";

                auto result = fileValidatorWrapper.test_validateFile(std::string(path));

                REQUIRE(!result);
            }

            SECTION("Wrong crc")
            {
                auto path = "/testfiles/data/personalization_files/invalid/"
                            "invalid_crc.json";

                auto result = fileValidatorWrapper.test_validateFile(std::string(path));

                REQUIRE(!result);
            }
        }
    }

    SECTION("Check PersonalizationFileParser class")
    {
        SECTION("Check: parse personalization file")
        {
            SECTION("Valid files")
            {
                auto pathToValidFile = "/testfiles/data/personalization_files/"
                                       "valid/personalization_black.json";
                auto expectedSerialNumber = "TestSerialNumber";
                auto expectedCaseColour   = "black";

                auto parsedData = PersonalizationFileParser(std::string(pathToValidFile));

                REQUIRE(parsedData.getSerialNumber() == expectedSerialNumber);
                REQUIRE(parsedData.getCaseColour() == expectedCaseColour);

                REQUIRE(parsedData.validate(expectedSerialNumber));

                pathToValidFile = "/testfiles/data/personalization_files/valid/"
                                  "personalization_white.json";
                expectedCaseColour = "white";

                parsedData = PersonalizationFileParser(std::string(pathToValidFile));

                REQUIRE(parsedData.getSerialNumber() == expectedSerialNumber);
                REQUIRE(parsedData.getCaseColour() == expectedCaseColour);

                REQUIRE(parsedData.validate(expectedSerialNumber));
            }

            SECTION("Invalid file")
            {

                SECTION("File does not exist")
                {
                    auto pathToValidFile = "/testfiles/data/"
                                           "personalization_files/invalid/not_existing.json";
                    auto invalidSerialNumber = "";
                    auto invalidCaseColour   = "";

                    auto parsedData = PersonalizationFileParser(std::string(pathToValidFile));

                    REQUIRE(parsedData.getSerialNumber() == invalidSerialNumber);
                    REQUIRE(parsedData.getCaseColour() == invalidCaseColour);

                    auto actualSerialNumber = parsedData.getSerialNumber();
                    REQUIRE(!parsedData.validate(actualSerialNumber));
                }

                SECTION("Invalid crc")
                {
                    auto pathToValidFile = "/testfiles/data/"
                                           "personalization_files/invalid/invalid_crc.json";
                    auto invalidSerialNumber = "";
                    auto invalidCaseColour   = "";

                    auto parsedData = PersonalizationFileParser(std::string(pathToValidFile));

                    REQUIRE(parsedData.getSerialNumber() == invalidSerialNumber);
                    REQUIRE(parsedData.getCaseColour() == invalidCaseColour);

                    auto actualSerialNumber = parsedData.getSerialNumber();
                    REQUIRE(!parsedData.validate(actualSerialNumber));
                }

                SECTION("Invalid format")
                {
                    auto pathToValidFile = "/testfiles/data/"
                                           "personalization_files/invalid/invalid_json_format.json";
                    auto invalidSerialNumber = "";
                    auto invalidCaseColour   = "";

                    auto parsedData = PersonalizationFileParser(std::string(pathToValidFile));

                    REQUIRE(parsedData.getSerialNumber() == invalidSerialNumber);
                    REQUIRE(parsedData.getCaseColour() == invalidCaseColour);

                    auto actualSerialNumber = parsedData.getSerialNumber();
                    REQUIRE(!parsedData.validate(actualSerialNumber));
                }

                SECTION("Invalid mandatory param")
                {
                    auto pathToValidFile = "/testfiles/data/"
                                           "personalization_files/valid/personalization_black.json";
                    auto expectedSerialNumber = "ProvidedSerialNumber";
                    auto caseColour           = "black";

                    auto parsedData = PersonalizationFileParser(std::string(pathToValidFile));

                    REQUIRE(parsedData.getCaseColour() == caseColour);

                    REQUIRE(!parsedData.validate(expectedSerialNumber));
                }

                SECTION("Invalid optional params")
                {
                    auto pathToValidFile = "/testfiles/data/"
                                           "personalization_files/invalid/personalization_yellow.json";
                    auto expectedSerialNumber = "TestSerialNumber";
                    auto caseColour           = "yellow";

                    auto parsedData = PersonalizationFileParser(std::string(pathToValidFile));

                    REQUIRE(parsedData.getSerialNumber() == expectedSerialNumber);
                    REQUIRE(parsedData.getCaseColour() == caseColour);

                    REQUIRE(parsedData.validate(expectedSerialNumber));

                    REQUIRE(parsedData.getCaseColour() == "white");
                }
            }
        }
    }
}
