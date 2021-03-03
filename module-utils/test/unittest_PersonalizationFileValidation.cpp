// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "personalization_data_integration/PersonalizationFileValidation.hpp"
#include <filesystem>

class FileParserTestWrapper : public PersonalizationFileParser
{
  public:
    explicit FileParserTestWrapper(const std::filesystem::path &filePath) : PersonalizationFileParser(filePath) {};

    json11::Json test_getJsonObject()
    {
        auto content = loadFileAsString();
        return getJsonObjectFromContent(content);
    }

    bool test_validateFile()
    {
        return validateFile();
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


        SECTION("Check getJsonObject method")
        {
            SECTION("Valid case")
            {
                auto path = std::filesystem::current_path();
                path += "/testfiles/data/personalization_files/valid/personalization_black.json";

                auto fileParserTestWrapper = FileParserTestWrapper(path);

                auto expectedSerialNumber = "TestSerialNumber";
                auto expectedCaseColour   = "black";

                auto jsonObj      = fileParserTestWrapper.test_getJsonObject();
                auto serialNumber = jsonObj[phone_personalization::json::serial_number].string_value();
                auto caseColour   = jsonObj[phone_personalization::json::case_color].string_value();

                REQUIRE(serialNumber == expectedSerialNumber);
                REQUIRE(caseColour == expectedCaseColour);
            }

            SECTION("Wrong json format")
            {
                auto path = std::filesystem::current_path();
                path += "/testfiles/data/personalization_files/invalid/invalid_json_format.json";
                auto fileValidatorWrapper = FileParserTestWrapper(path);

                auto jsonObj = fileValidatorWrapper.test_getJsonObject();
                REQUIRE(jsonObj == nullptr);
            }
        }

        SECTION("Check test validateFile")
        {
            SECTION("Valid case")
            {
                auto path = std::filesystem::current_path();
                path += "/testfiles/data/personalization_files/valid/personalization_black.json";
                auto fileParserTestWrapper = FileParserTestWrapper(path);

                auto result = fileParserTestWrapper.test_validateFile();

                REQUIRE(result);
            }

            SECTION("File does not exist")
            {
                auto path = std::filesystem::current_path();
                path += "/testfiles/data/personalization_files/invalid/not_existing.json";

                auto fileParserTestWrapper = FileParserTestWrapper(path);

                auto result = fileParserTestWrapper.test_validateFile();

                REQUIRE(!result);
            }

            SECTION("Wrong crc")
            {
                auto path = std::filesystem::current_path();
                path += "/testfiles/data/personalization_files/invalid/invalid_crc.json";

                auto fileParserTestWrapper = FileParserTestWrapper(path);

                auto result = fileParserTestWrapper.test_validateFile();

                REQUIRE(!result);
            }
        }
    }

    SECTION("Check PersonalizationData class")
    {
        SECTION("Check: parse personalization file")
        {
            SECTION("Valid files")
            {
                auto path = std::filesystem::current_path();
                path += "/testfiles/data/personalization_files/valid/personalization_black.json";

                auto expectedSerialNumber = "TestSerialNumber";
                auto expectedCaseColour   = "black";

                auto parsedData = PersonalizationData(path);

                REQUIRE(parsedData.getSerialNumber() == expectedSerialNumber);
                REQUIRE(parsedData.getCaseColour() == expectedCaseColour);

                REQUIRE(parsedData.validate(expectedSerialNumber));

                path = std::filesystem::current_path();
                path += "/testfiles/data/personalization_files/valid/personalization_white.json";

                expectedCaseColour = "white";

                parsedData = PersonalizationData(path);

                REQUIRE(parsedData.getSerialNumber() == expectedSerialNumber);
                REQUIRE(parsedData.getCaseColour() == expectedCaseColour);

                REQUIRE(parsedData.validate(expectedSerialNumber));
            }

            SECTION("Invalid file")
            {

                SECTION("File does not exist")
                {
                    auto path = std::filesystem::current_path();
                    path += "/testfiles/data/personalization_files/invalid/not_existing.json";

                    auto invalidSerialNumber = "";
                    auto invalidCaseColour   = "";

                    LOG_DEBUG("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!111111111111111");
                    auto parsedData = PersonalizationData(path);

                    REQUIRE(parsedData.getSerialNumber() == invalidSerialNumber);
                    REQUIRE(parsedData.getCaseColour() == invalidCaseColour);

                    auto actualSerialNumber = parsedData.getSerialNumber();
                    REQUIRE(!parsedData.validate(actualSerialNumber));
                }

                SECTION("Invalid crc")
                {
                    auto path = std::filesystem::current_path();
                    path += "/testfiles/data/personalization_files/invalid/invalid_crc.json";

                    auto invalidSerialNumber = "";
                    auto invalidCaseColour   = "";

                    auto parsedData = PersonalizationData(path);

                    REQUIRE(parsedData.getSerialNumber() == invalidSerialNumber);
                    REQUIRE(parsedData.getCaseColour() == invalidCaseColour);

                    auto actualSerialNumber = parsedData.getSerialNumber();
                    REQUIRE(!parsedData.validate(actualSerialNumber));
                }

                SECTION("Invalid format")
                {
                    auto path = std::filesystem::current_path();
                    path += "/testfiles/data/personalization_files/invalid/invalid_json_format.json";

                    auto invalidSerialNumber = "";
                    auto invalidCaseColour   = "";

                    auto parsedData = PersonalizationData(path);

                    REQUIRE(parsedData.getSerialNumber() == invalidSerialNumber);
                    REQUIRE(parsedData.getCaseColour() == invalidCaseColour);

                    auto actualSerialNumber = parsedData.getSerialNumber();
                    REQUIRE(!parsedData.validate(actualSerialNumber));
                }

                SECTION("Invalid mandatory param")
                {
                    auto path = std::filesystem::current_path();
                    path += "/testfiles/data/personalization_files/valid/personalization_black.json";

                    auto expectedSerialNumber = "ProvidedSerialNumber";
                    auto caseColour           = "black";

                    auto parsedData = PersonalizationData(path);

                    REQUIRE(parsedData.getCaseColour() == caseColour);

                    REQUIRE(!parsedData.validate(expectedSerialNumber));
                }

                SECTION("Invalid optional params")
                {
                    auto path = std::filesystem::current_path();
                    path += "/testfiles/data/personalization_files/invalid/personalization_yellow.json";

                    auto expectedSerialNumber = "TestSerialNumber";
                    auto caseColour           = "yellow";

                    auto parsedData = PersonalizationData(path);

                    REQUIRE(parsedData.getSerialNumber() == expectedSerialNumber);
                    REQUIRE(parsedData.getCaseColour() == caseColour);

                    REQUIRE(parsedData.validate(expectedSerialNumber));

                    REQUIRE(parsedData.getCaseColour() == "white");
                }
            }
        }
    }
}
