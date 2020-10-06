// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "i18/i18.hpp"
#include <algorithm> // std::find_if_not
#include <log/log.hpp>
#include <sstream>
#include <iomanip>

#define MAGIC_ENUM_RANGE_MAX 256
#include <magic_enum.hpp>

namespace utils
{
    static const std::string WHITESPACE = " \n\r\t\f\v";
    constexpr unsigned int secondsInMinute =
        std::chrono::duration_cast<std::chrono::seconds>(std::chrono::minutes(1)).count();

    template <typename Out> void split(const std::string &s, char delim, Out result)
    {
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delim)) {
            *(result++) = item;
        }
    }

    static inline std::vector<std::string> split(const std::string &s, char delim)
    {
        std::vector<std::string> elems;
        split(s, delim, std::back_inserter(elems));
        return elems;
    }

    static inline std::vector<std::string> split(const std::string &s,
                                                 const std::string &delimiter,
                                                 const bool skipEmptyTokens = true)
    {
        size_t pos_start = 0, pos_end, delim_len = delimiter.length();
        std::string token;
        std::vector<std::string> res;
        uint32_t tokenCount = 0;

        while (((pos_end = s.find(delimiter, pos_start)) != std::string::npos)) {
            token     = s.substr(pos_start, pos_end - pos_start);
            pos_start = pos_end + delim_len;
            if (!skipEmptyTokens || !token.empty()) {
                tokenCount++;
                res.push_back(token);
            }
        }

        token = s.substr(pos_start);
        if (!skipEmptyTokens || !token.empty()) {
            res.push_back(token);
        }
        return res;
    }

    static inline std::string removeNewLines(const std::string &s)
    {
        std::string retStr = s;
        retStr.erase(std::remove(retStr.begin(), retStr.end(), '\r'), retStr.end());
        retStr.erase(std::remove(retStr.begin(), retStr.end(), '\n'), retStr.end());

        return retStr;
    }

    static inline bool is_number(const std::string &s)
    {
        return !s.empty() && std::find_if(s.begin(), s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
    }

    static inline std::string ltrim(const std::string &s)
    {
        size_t start = s.find_first_not_of(WHITESPACE);
        return (start == std::string::npos) ? "" : s.substr(start);
    }

    static inline std::string rtrim(const std::string &s)
    {
        size_t end = s.find_last_not_of(WHITESPACE);
        return (end == std::string::npos) ? "" : s.substr(0, end + 1);
    }

    static inline std::string trim(const std::string &s)
    {
        return rtrim(ltrim(s));
    }

    template <typename T> std::string to_string(T t, size_t minStringLength = 0)
    {
        constexpr auto leadingDigit = '0';
        std::ostringstream ss;
        ss << std::setfill(leadingDigit) << std::setw(minStringLength) << t;
        return ss.str();
    }

    template <typename T>[[nodiscard]] const std::string enumToString(const T &t)
    {
        static_assert(std::is_enum_v<T>);
        return std::string(magic_enum::enum_name(t));
    }

    /// Gets value of type T from string
    ///
    /// @param value to be converted
    /// @return Value casted to type T
    template <typename T>[[nodiscard]] T getValue(const std::string &value)
    {
        static_assert(std::is_arithmetic_v<T>);
        if (value.empty()) {
            return {};
        }
        T ret;
        std::istringstream(value) >> ret;
        return ret;
    }

    static inline void findAndReplaceAll(std::string &data,
                                         const std::vector<std::pair<std::string, std::optional<std::string>>> &values,
                                         std::function<std::string(int)> getReplaceString = nullptr)
    {
        int valIdx = 0;
        for (auto &el : values) {
            std::string preStr, replaceStr, postStr;
            const std::string &toSearch = el.first;

            if (toSearch.empty()) {
                continue;
            }

            auto begin_pos = data.begin();
            auto found_pos = data.begin();
            while (found_pos != data.end()) {
                begin_pos = data.begin();
                found_pos = std::search(begin_pos, data.end(), toSearch.begin(), toSearch.end());
                if (found_pos != data.end()) {
                    preStr = std::string(begin_pos, found_pos);
                    if (replaceStr.empty() && el.second) {
                        replaceStr = *el.second;
                    }
                    else if (replaceStr.empty() && getReplaceString) {
                        replaceStr = getReplaceString(valIdx);
                    }
                    postStr.assign(found_pos + toSearch.size(), data.end());
                    data = preStr + replaceStr + postStr;
                }
            }
            valIdx++;
        }
    }

    static inline void findAndReplaceAll(std::string &data,
                                         const std::vector<std::string> &values,
                                         std::function<std::string(int)> getReplaceString)
    {
        std::vector<std::pair<std::string, std::optional<std::string>>> valuePairs;
        valuePairs.reserve(values.size());
        for (auto val : values) {
            valuePairs.emplace_back(std::make_pair(val, std::nullopt));
        }
        findAndReplaceAll(data, valuePairs, getReplaceString);
    }

    static inline void findAndReplaceAll(std::string &data, const std::string &toSearch, const std::string &replaceStr)
    {
        findAndReplaceAll(data, {{toSearch, replaceStr}});
    }

    static inline bool toNumeric(const std::string &text, uint32_t &value)
    {
        try {
            value = std::stoi(text);
            return true;
        }
        catch (const std::exception &e) {
            LOG_ERROR("toNumeric exception %s", e.what());
            return false;
        }

        return false;
    }

    static inline uint32_t fromBigEndian(const uint8_t buf[])
    {
        return (((((uint8_t)buf[0]) << 24U) | (((uint8_t)buf[1]) << 16U) | (((uint8_t)buf[2]) << 8U) |
                 (((uint8_t)buf[3]) << 0U)));
    }

    static inline void toBigEndian(const uint32_t in, uint8_t inplace[])
    {
        inplace[0] = ((((uint32_t)(in)) >> 24U) & 0xFFU);
        inplace[1] = ((((uint32_t)(in)) >> 16U) & 0xFFU);
        inplace[2] = ((((uint32_t)(in)) >> 8U) & 0xFFU);
        inplace[3] = (((uint32_t)(in)) & 0xFFU);
    }

} // namespace utils
