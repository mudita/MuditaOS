// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <module-utils/gsl/gsl>
#include "module-utils/bootconfig/src/bootconfig.cpp"
#include "PersonalizationFileValidation.hpp"


ParamValidator::ParamValidator(const std::string &paramKey,
                               const bool isMandatory,
                               const std::vector<std::string> &validValues)
        : paramKey{paramKey}, validValues{validValues}, isMandatory{isMandatory} {}

auto ParamValidator::validate(json11::Json obj) -> ValidationResult
{
    if (!paramKey.empty()) {
        for (auto valid : validValues) {
            if (obj[paramKey].string_value() == valid) {
                return ValidationResult::valid;
            }
        }
    }

    if (!isMandatory) {
        return ValidationResult::invalid;
    }

    LOG_FATAL("Mandatory data is invalid!");
    return ValidationResult::critical;
}

auto PersonalizationFileParser::getJsonObject(const std::filesystem::path &filePath) const -> json11::Json
{
    auto fileHandle = fopen(filePath.c_str(), "r");
    auto readBuf = std::make_unique<char[]>(purefs::buffer::tar_buf);

    std::string contents;
    size_t readSize;

    if (fileHandle) {
        while (!feof(fileHandle)) {
            readSize = fread(readBuf.get(), 1, purefs::buffer::tar_buf, fileHandle);
            contents.append(static_cast<const char *>(readBuf.get()), readSize);
        }
    }

    auto fpCloseAct = gsl::finally([fileHandle] { fclose(fileHandle); });

    LOG_INFO("CONTENTS: %s", contents.c_str());
    std::string err;
    json11::Json jsonObject = json11::Json::parse(contents.c_str(), err);

    if (err.length() != 0) {
        LOG_FATAL("JSON format error: %s", err.c_str());
        return nullptr;
    }

    return jsonObject;
}

auto PersonalizationFileParser::validateFile(const std::filesystem::path &filePath) -> bool
{
    if (!boot::readAndVerifyCRC(filePath)) {
        LOG_ERROR("Invalid crc calculation!");
        return false;
    }

    LOG_INFO("CRC is correct");
    return true;
};

auto PersonalizationFileParser::parse(const std::filesystem::path &filePath) -> json11::Json
{
    if (validateFile(filePath)) {
        return getJsonObject(filePath);
    }
    LOG_ERROR("Personalization file is invalid!");
    return nullptr;
}

PersonalizationData::PersonalizationData(const std::filesystem::path &filePath)
{
    params = parse(filePath);
    if (params != nullptr) {
        serialNumber = params[phone_personalization::json::serial_number].string_value();
        caseColour   = params[phone_personalization::json::case_color].string_value();
        return;
    }

    LOG_ERROR("Personalization file is invalid!");
    valid = false;
}

auto PersonalizationData::validate(const std::string &expectedSerialNumber) -> bool
{
    if (!valid) {
        return false;
    }

    auto validSerialNumber = std::vector<std::string>({expectedSerialNumber});

    paramsValidator.emplace_back(ParamValidator(phone_personalization::json::serial_number, true, validSerialNumber));
    paramsValidator.emplace_back(ParamValidator(
        phone_personalization::json::case_color, false, phone_personalization::params::case_colour::valid_values));

    for (auto &validator : paramsValidator) {
        switch (validator.validate(params)) {
        case ValidationResult::valid: {
            break;
        }
        case ValidationResult::invalid: {
            if (validator.getParamKey() == phone_personalization::json::case_color) {
                caseColour = phone_personalization::params::case_colour::default_value;
            }
            break;
        }
        case ValidationResult::critical: {
            LOG_ERROR("Personalization parameter is invalid!");
            return false;
        }
        }
    }

    LOG_INFO("Personalization file is valid");
    return true;
}
