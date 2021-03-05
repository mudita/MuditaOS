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
    struct OptionalParameter
    {
        std::string defaultValue;
        std::vector<std::string> validValues;
        bool isValid = false;
    };

    namespace param
    {
        namespace serial_number
        {
            constexpr inline auto key = "serial";
            constexpr inline auto prefix = "SN";  ///Need access to req witch describes SN format
            constexpr inline auto length = 14;
        } // namespace serial_number

        namespace case_colour
        {
            constexpr inline auto key = "case_colour";
            constexpr inline auto default_value = "white";
            const std::vector<std::string> valid_values = {"white", "black"};
        } // namespace case_colour

    } // namespace params

    class PersonalizationFileParser
    {
        void setDefaultOptionalParamsMap()
        {
            optionalParams = {
                    {param::case_colour::key, OptionalParameter{param::case_colour::default_value, param::case_colour::valid_values}}
            };
        }
    protected:
        std::map<std::string, OptionalParameter> optionalParams;
        json11::Json jsonObj = nullptr;
        std::filesystem::path filePath;

        explicit PersonalizationFileParser(const std::filesystem::path &filePath)
        : filePath{filePath}
        {
            setDefaultOptionalParamsMap();
        };

        auto loadFileContent() -> std::string;
        auto parseJsonFromContent(const std::string &content) -> bool;

    public:
        [[nodiscard]] auto getJsonObject() const -> json11::Json
        {
            return jsonObj;
        }

        [[nodiscard]] auto getDefaultOptionalParamsMap() const -> std::map<std::string, OptionalParameter>
        {
            return optionalParams;
        }
    };

    class PersonalizationFileValidator : public PersonalizationFileParser {

    protected:
        auto validateFileAndCRC() -> bool;
        auto validateFileAndItsContent() -> bool;
        auto validateSerialNumber() -> bool;
        void validateOptionalParams();

    public:
        explicit PersonalizationFileValidator(const std::filesystem::path &filePath)
        : PersonalizationFileParser(filePath) {}

        auto validate() -> bool;
    };

    class PersonalizationParamsGetter
    {
        PersonalizationFileParser personalizationParser;
        std::map<std::string, std::string> parameters;
    public:
        explicit PersonalizationParamsGetter(const PersonalizationFileParser &parser) : personalizationParser{parser}
        {}
        ///Set invalid optional params as default -> get parameters map
        std::map<std::string, std::string> getParams();
    };
}



