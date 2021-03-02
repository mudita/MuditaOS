// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <module-utils/gsl/gsl>
#include "module-utils/bootconfig/src/bootconfig.cpp"
#include "PersonalizationFileValidation.hpp"

json11::Json FileValidator::getJsonObject(const std::filesystem::path &filePath)
{
    auto lamb = [](::FILE *stream) { ::fclose(stream); };
    std::unique_ptr<char[]> readBuf(new char[purefs::buffer::tar_buf]);
    std::unique_ptr<::FILE, decltype(lamb)> fp(fopen(filePath.c_str(), "r"), lamb);
    std::string contents;
    size_t readSize;

    if (fp.get() != nullptr) {
        while (!feof(fp.get())) {
            readSize = fread(readBuf.get(), 1, purefs::buffer::tar_buf, fp.get());
            contents.append(static_cast<const char *>(readBuf.get()), readSize);
        }
    }

    LOG_INFO("CONTENTS: %s", contents.c_str());
    std::string err;
    json11::Json jsonObject = json11::Json::parse(contents.c_str(), err);

    if (err.length() != 0) {
        LOG_FATAL("JSON format error: %s", err.c_str());
        return nullptr;
    }

    return jsonObject;
}

bool FileValidator::validateFile(const std::filesystem::path &filePath)
{
    if (!boot::readAndVerifyCRC(filePath)) {
        LOG_ERROR("Invalid crc calculation!");
        return false;
    }

    LOG_INFO("CRC is correct");
    return true;
};

ParamValidator::ParamValidator(const std::string &paramKey,
                               const bool isMandatory,
                               const std::vector<std::string> &validValues)
{
    this->paramKey    = paramKey;
    this->isMandatory = isMandatory;
    this->validValues = std::make_unique<std::vector<std::string>>(validValues);
}

auto ParamValidator::validate(json11::Json obj) -> ValidationResult
{
    if (!paramKey.empty()) {
        for (auto valid : *validValues) {
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

PersonalizationFileParser::PersonalizationFileParser(const std::filesystem::path &filePath)
{
    if (validateFile(filePath)) {
        params = getJsonObject(filePath);
        if (params != nullptr) {
            serialNumber = params[phone_personalization::json::serial_number].string_value();
            caseColour   = params[phone_personalization::json::case_color].string_value();
            return;
        }
    }
    LOG_ERROR("Personalization file is invalid!");
    valid = false;
}

auto PersonalizationFileParser::validate(const std::string &expectedSerialNumber) -> bool
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
