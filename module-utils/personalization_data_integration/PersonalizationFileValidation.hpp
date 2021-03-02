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
    namespace json
    {
        constexpr auto serial_number = "serial";
        constexpr auto case_color    = "case_colour";
    } // namespace json

    namespace params
    {
        namespace case_colour
        {
            constexpr auto default_value = "white";

            const std::vector<std::string> valid_values = {"white", "black"};
        } // namespace case_colour
    }     // namespace params
} // namespace phone_personalization

enum class ValidationResult
{
    valid,
    invalid,
    critical
};

class ParamValidator
{
    std::string paramKey = "";
    std::vector<std::string> validValues;
    bool isMandatory = true;

  public:
    ParamValidator(const std::string &paramKey, const bool isMandatory, const std::vector<std::string> &validValues);

    [[nodiscard]] auto getParamKey() const -> std::string
    {
        return paramKey;
    }

    auto validate(json11::Json obj) -> ValidationResult;
};

class FileValidator
{
  protected:
    [[nodiscard]] auto getJsonObject(const std::filesystem::path &filePath) const -> json11::Json;
    auto validateFile(const std::filesystem::path &filePath) -> bool;
};

class PersonalizationFileParser : public FileValidator
{
    json11::Json params = nullptr;
    std::vector<ParamValidator> paramsValidator;
    bool valid = true;

    std::string serialNumber = "";
    std::string caseColour   = "";

  public:
    explicit PersonalizationFileParser(const std::filesystem::path &filePath);

    [[nodiscard]] auto getSerialNumber() const -> std::string
    {
        return serialNumber;
    }

    [[nodiscard]] auto getCaseColour() const -> std::string
    {
        return caseColour;
    }

    auto validate(const std::string &expectedSerialNumber) -> bool;
};
