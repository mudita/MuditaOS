// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-utils/log/log.hpp>
#include <module-utils/Utils.hpp>
#include <module-vfs/include/user/purefs/fs/filesystem.hpp>
#include <module-vfs/include/user/purefs/filesystem_paths.hpp>
#include "module-utils/json/json11.hpp"

namespace phone_personalization
{
    namespace json::key
    {
        constexpr inline auto serial_number = "serial";
        constexpr inline auto case_color    = "case_colour";
    } // namespace json

    namespace params
    {
        namespace case_colour
        {
            constexpr inline auto default_value = "white";

            const std::vector<std::string> valid_values = {"white", "black"};
        } // namespace case_colour

    }     // namespace params
} // namespace phone_personalization

///TODO: 1. cleanUp 2. method for checking serial number  3. UT

//class PersonalizationData
//{
//    ///TODO: Check if it can be removed!!! think
//    std::string serialNumber;
//    std::string caseColour;
//
//public:
//    void setSerialNumber(const std::string &serialNumber)
//    {
//        this->serialNumber = serialNumber;
//    }
//
//    void setCaseColour(const std::string &caseColour)
//    {
//        this->caseColour = caseColour;
//    }
//
//    [[nodiscard]] auto getSerialNumber() const -> std::string
//    {
//        return serialNumber;
//    }
//
//    [[nodiscard]] auto getCaseColour() const -> std::string
//    {
//        return caseColour;
//    }
//};

class PersonalizationFileParser
{
  protected:
    //PersonalizationData data;
    json11::Json params = nullptr;
    std::filesystem::path filePath;

    explicit PersonalizationFileParser(const std::filesystem::path &filePath) : filePath{filePath} {};

    auto loadFileAsString() -> std::string;
    auto parseJsonFromContent(const std::string &content) -> bool;
};

class PersonalizationFileValidator : public PersonalizationFileParser
{

protected:
    auto validateFileAndCRC() -> bool;
    auto validateFileAndItsContent() -> bool;
    auto validateParameters(const std::map<std::string, std::vector<std::string>> &parametersMap) -> bool;

public:
    explicit PersonalizationFileValidator(const std::filesystem::path &filePath) : PersonalizationFileParser(filePath) {}

};




