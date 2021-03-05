// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "personalization_data_integration/PersonalizationFileValidation.hpp"
#include <filesystem>

namespace phone_personalization {

    class PersonalizationFileParserTestWrapper : public PersonalizationFileParser {
    public:
        explicit PersonalizationFileParserTestWrapper(const std::filesystem::path &filePath)
                : PersonalizationFileParser(filePath) {};

        std::string test_loadFileContent() {
            return loadFileContent();
        }

        bool test_parseJsonFromContent(const std::string &content) {
            return parseJsonFromContent(content);
        }
    };

    class PersonalizationFileValidatorWrapper : public PersonalizationFileValidator {
    public:
        explicit PersonalizationFileValidatorWrapper(const std::filesystem::path &filePath)
                : PersonalizationFileValidator(filePath) {};

        bool test_validateFileAndCRC() {
            return validateFileAndCRC();
        }

        bool test_validateFileAndItsContent() {
            return validateFileAndItsContent();
        }

        bool test_validateSerialNumber() {
            auto content = loadFileContent();
            parseJsonFromContent(content);
            return validateSerialNumber();
        }

        std::map<std::string, OptionalParameter> test_optionalParamsValidation() {
            auto content = loadFileContent();
            parseJsonFromContent(content);
            validateOptionalParams();
            return optionalParams;
        }

        std::map<std::string, OptionalParameter> getDefaultOptionalParams() {
            return optionalParams;
        }
    };

    TEST_CASE("Personalization File Validation") {

        SECTION("Check PersonalizationFileParser class") {

            SECTION("Valid case") {
                auto path = std::filesystem::current_path();
                path += "/testfiles/data/personalization_files/valid/personalization_black.json";

                auto fileParserTestWrapper = PersonalizationFileParserTestWrapper(path);

                auto expectedSerialNumber = "SN123456789098";
                auto expectedCaseColour = "black";

                auto content = fileParserTestWrapper.test_loadFileContent();
                REQUIRE(!content.empty());

                REQUIRE(fileParserTestWrapper.test_parseJsonFromContent(content));
                auto jsonObj = fileParserTestWrapper.getJsonObject();
                REQUIRE(jsonObj != nullptr);

                auto serialNumber = jsonObj[param::serial_number::key].string_value();
                auto caseColour = jsonObj[param::case_colour::key].string_value();

                REQUIRE(serialNumber == expectedSerialNumber);
                REQUIRE(caseColour == expectedCaseColour);
            }

            SECTION("Wrong json format") {
                auto path = std::filesystem::current_path();
                path += "/testfiles/data/personalization_files/invalid/invalid_json_format.json";

                auto fileParserTestWrapper = PersonalizationFileParserTestWrapper(path);

                auto content = fileParserTestWrapper.test_loadFileContent();
                REQUIRE(!content.empty());

                REQUIRE(!fileParserTestWrapper.test_parseJsonFromContent(content));
                REQUIRE(fileParserTestWrapper.getJsonObject() == nullptr);
            }

            SECTION("File doesn't exist") {
                auto path = std::filesystem::current_path();
                path += "/testfiles/data/personalization_files/invalid/not_existing.json";

                auto fileParserTestWrapper = PersonalizationFileParserTestWrapper(path);

                auto content = fileParserTestWrapper.test_loadFileContent();
                REQUIRE(content.empty());
            }

        }

        SECTION("Check PersonalizationFileParser class") {

            SECTION("Check test validateFileAndCRC") {
                SECTION("Valid case") {
                    auto path = std::filesystem::current_path();
                    path += "/testfiles/data/personalization_files/valid/personalization_black.json";

                    auto fileParserTestWrapper = PersonalizationFileValidatorWrapper(path);
                    auto result = fileParserTestWrapper.test_validateFileAndCRC();

                    REQUIRE(result);
                }

                SECTION("File does not exist") {
                    auto path = std::filesystem::current_path();
                    path += "/testfiles/data/personalization_files/invalid/not_existing.json";

                    auto fileParserTestWrapper = PersonalizationFileValidatorWrapper(path);
                    auto result = fileParserTestWrapper.test_validateFileAndCRC();

                    REQUIRE(!result);
                }

                SECTION("Wrong crc") {
                    auto path = std::filesystem::current_path();
                    path += "/testfiles/data/personalization_files/invalid/invalid_crc.json";

                    auto fileParserTestWrapper = PersonalizationFileValidatorWrapper(path);
                    auto result = fileParserTestWrapper.test_validateFileAndCRC();

                    REQUIRE(!result);
                }
            }

            SECTION("Check test validateFileAndItsContent") {
                SECTION("Valid case") {
                    auto path = std::filesystem::current_path();
                    path += "/testfiles/data/personalization_files/valid/personalization_black.json";

                    auto fileValidatorTestWrapper = PersonalizationFileValidatorWrapper(path);
                    auto result = fileValidatorTestWrapper.test_validateFileAndItsContent();

                    REQUIRE(result);
                }

                SECTION("File does not exist") {
                    auto path = std::filesystem::current_path();
                    path += "/testfiles/data/personalization_files/invalid/not_existing.json";

                    auto fileValidatorTestWrapper = PersonalizationFileValidatorWrapper(path);
                    auto result = fileValidatorTestWrapper.test_validateFileAndItsContent();

                    REQUIRE(!result);
                }

                SECTION("Wrong json format") {
                    auto path = std::filesystem::current_path();
                    path += "/testfiles/data/personalization_files/invalid/invalid_json_format.json";

                    auto fileValidatorTestWrapper = PersonalizationFileValidatorWrapper(path);
                    auto result = fileValidatorTestWrapper.test_validateFileAndItsContent();

                    REQUIRE(!result);
                }
            }

            SECTION("Check test validateSerialNumber") {
                SECTION("Valid cases") {

                    auto path = std::filesystem::current_path();
                    path += "/testfiles/data/personalization_files/valid/personalization_black.json";

                    auto fileValidatorTestWrapper = PersonalizationFileValidatorWrapper(path);
                    auto result = fileValidatorTestWrapper.test_validateSerialNumber();

                    REQUIRE(result);

                    path = std::filesystem::current_path();
                    path += "/testfiles/data/personalization_files/valid/personalization_white.json";

                    auto fileValidatorTestWrapper2 = PersonalizationFileValidatorWrapper(path);
                    result = fileValidatorTestWrapper2.test_validateSerialNumber();

                    REQUIRE(result);
                }

                SECTION("Empty serial field") {
                    auto path = std::filesystem::current_path();
                    path += "/testfiles/data/personalization_files/invalid/personalization_empty_serial.json";

                    auto fileValidatorTestWrapper = PersonalizationFileValidatorWrapper(path);
                    auto result = fileValidatorTestWrapper.test_validateSerialNumber();

                    REQUIRE(!result);
                }

                SECTION("Wrong serial number format") {
                    auto path = std::filesystem::current_path();
                    path += "/testfiles/data/personalization_files/invalid/invalid_serial_number.json";

                    auto fileValidatorTestWrapper = PersonalizationFileValidatorWrapper(path);
                    auto result = fileValidatorTestWrapper.test_validateSerialNumber();

                    REQUIRE(!result);
                }
            }

            SECTION("Check test validateOptionalParams") {
                SECTION("Valid cases") {
                    SECTION("case colour black") {
                        auto path = std::filesystem::current_path();
                        path += "/testfiles/data/personalization_files/valid/personalization_black.json";

                        auto fileValidatorTestWrapper = PersonalizationFileValidatorWrapper(path);
                        auto validatedParams = fileValidatorTestWrapper.test_optionalParamsValidation();

                        REQUIRE(validatedParams[param::case_colour::key].isValid);
                    }
                }

                SECTION("case colour white") {
                    auto path = std::filesystem::current_path();
                    path += "/testfiles/data/personalization_files/valid/personalization_white.json";

                    auto fileValidatorTestWrapper = PersonalizationFileValidatorWrapper(path);
                    auto validatedParams = fileValidatorTestWrapper.test_optionalParamsValidation();

                    REQUIRE(validatedParams[param::case_colour::key].isValid);
                }
            }

            SECTION("Invalid case") {
                auto path = std::filesystem::current_path();
                path += "/testfiles/data/personalization_files/invalid/personalization_yellow.json";

                auto fileValidatorTestWrapper = PersonalizationFileValidatorWrapper(path);
                auto expectedParameters = fileValidatorTestWrapper.getDefaultOptionalParams();
                auto validatedParams = fileValidatorTestWrapper.test_optionalParamsValidation();

                REQUIRE(!validatedParams[param::case_colour::key].isValid);
            }
        }


        SECTION("Check PersonalizationFileValidator class") {
            SECTION("Check: parse personalization file") {
                SECTION("Valid files") {
                    auto path = std::filesystem::current_path();
                    path += "/testfiles/data/personalization_files/valid/personalization_black.json";

                    auto expectedSerialNumber = "SN123456789098";
                    auto expectedCaseColour = "black";

                    auto personalizationValidator = PersonalizationFileValidator(path);
                    REQUIRE(personalizationValidator.validate());
                    auto paramsGetter = PersonalizationParamsGetter(personalizationValidator);

                    auto params = paramsGetter.getParams();
                    auto serialNumber = params[param::serial_number::key];
                    auto caseColour = params[param::case_colour::key];

                    REQUIRE(serialNumber == expectedSerialNumber);
                    REQUIRE(caseColour == expectedCaseColour);


                    path = std::filesystem::current_path();
                    path += "/testfiles/data/personalization_files/valid/personalization_white.json";

                    expectedCaseColour = "white";

                    auto personalizationValidator2 = PersonalizationFileValidator(path);
                    REQUIRE(personalizationValidator2.validate());
                    auto paramsGetter2 = PersonalizationParamsGetter(personalizationValidator2);

                    auto params2 = paramsGetter2.getParams();
                    serialNumber = params2[param::serial_number::key];
                    caseColour = params2[param::case_colour::key];


                    REQUIRE(serialNumber == expectedSerialNumber);
                    REQUIRE(caseColour == expectedCaseColour);
                }

                SECTION("Invalid file") {

                    SECTION("File does not exist") {
                        auto path = std::filesystem::current_path();
                        path += "/testfiles/data/personalization_files/invalid/not_existing.json";

                        auto personalizationValidator = PersonalizationFileValidator(path);
                        REQUIRE(!personalizationValidator.validate());
                    }

                    SECTION("Invalid crc") {
                        auto path = std::filesystem::current_path();
                        path += "/testfiles/data/personalization_files/invalid/invalid_crc.json";

                        auto personalizationValidator = PersonalizationFileValidator(path);
                        REQUIRE(!personalizationValidator.validate());
                    }

                    SECTION("Invalid format") {
                        auto path = std::filesystem::current_path();
                        path += "/testfiles/data/personalization_files/invalid/invalid_json_format.json";

                        auto personalizationValidator = PersonalizationFileValidator(path);
                        REQUIRE(!personalizationValidator.validate());
                    }

                    SECTION("Empty serial number") {
                        auto path = std::filesystem::current_path();
                        path += "/testfiles/data/personalization_files/invalid/personalization_empty_serial.json";

                        auto personalizationValidator = PersonalizationFileValidator(path);
                        REQUIRE(!personalizationValidator.validate());
                    }

                    SECTION("Invalid serial number") {
                        auto path = std::filesystem::current_path();
                        path += "/testfiles/data/personalization_files/invalid/invalid_serial_number.json";

                        auto personalizationValidator = PersonalizationFileValidator(path);
                        REQUIRE(!personalizationValidator.validate());
                    }

                    SECTION("Invalid optional params") {
                        auto path = std::filesystem::current_path();
                        path += "/testfiles/data/personalization_files/invalid/personalization_yellow.json";

                        auto expectedSerialNumber = "SN123456789098";
                        auto expectedCaseColour = param::case_colour::default_value;

                        auto personalizationValidator = PersonalizationFileValidator(path);
                        REQUIRE(personalizationValidator.validate());
                        auto paramsGetter = PersonalizationParamsGetter(personalizationValidator);

                        auto params = paramsGetter.getParams();
                        auto serialNumber = params[param::serial_number::key];
                        auto caseColour = params[param::case_colour::key];

                        REQUIRE(serialNumber == expectedSerialNumber);
                        REQUIRE(caseColour == expectedCaseColour);
                    }
                }
            }
        }

    }
}
