// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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

unsigned long FileValidator::readCRC(const std::filesystem::path &filePath)
{
    auto lamb = [](::FILE *stream) { ::fclose(stream); };

    std::unique_ptr<char[]> crcBuff(new char[phone_personalization::crc::size]);

    size_t readSize;
    std::filesystem::path crcFilePath(filePath);
    crcFilePath += phone_personalization::crc::extension;

    std::unique_ptr<::FILE, decltype(lamb)> fileHandle(::fopen(crcFilePath.c_str(), "r"), lamb);

    if (fileHandle.get() != nullptr) {
        if ((readSize = ::fread(crcBuff.get(), 1, phone_personalization::crc::size, fileHandle.get())) !=
            (phone_personalization::crc::size)) {
            LOG_ERROR("fread on %s returned different size then %d [%zu]",
                      crcFilePath.c_str(),
                      phone_personalization::crc::size,
                      readSize);
            return 0;
        }

        const unsigned long crc32Readed = strtoull(crcBuff.get(), nullptr, phone_personalization::crc::radix);
        LOG_DEBUG("CRC32 readed: %d", (int)crc32Readed);
        return crc32Readed;
    }

    LOG_ERROR("Reading CRC file error!");

    return 0;
}

bool FileValidator::validateFile(const std::filesystem::path &filePath)
{
    auto crcReaded = readCRC(filePath);
    if (!crcReaded) {
        LOG_ERROR("crc32 is invalid!");
        return false;
    }

    unsigned long crc32Computed;

    auto fileHandle = std::fopen(filePath.c_str(), "r");

    if (fileHandle != nullptr) {
        crc32Computed = utils::filesystem::computeFileCRC32(fileHandle);
        LOG_INFO("Computed crc32 for %s is %08" PRIX32, filePath.c_str(), static_cast<std::uint32_t>(crc32Computed));

        if (crcReaded == crc32Computed) {
            LOG_INFO("CRC is correct");
            return true;
        }

        LOG_ERROR("Wrong calculation of crc32!");
        std::fclose(fileHandle);
        return false;
    }

    LOG_ERROR("Can't open %s", filePath.c_str());
    return false;
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
