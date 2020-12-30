// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "i18n/i18n.hpp"
#include <algorithm> // std::find_if_not
#include <log/log.hpp>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <chrono>
#include <random>
#include "time/time_conversion.hpp"

#include <magic_enum.hpp>

namespace utils
{
    inline constexpr auto WHITESPACE = " \n\r\t\f\v";
    constexpr unsigned int secondsInMinute =
        std::chrono::duration_cast<std::chrono::seconds>(std::chrono::minutes(1)).count();

    std::string bytesToHex(const std::vector<std::uint8_t> &bytes);
    std::vector<std::uint8_t> hexToBytes(const std::string &hex);
    // template <typename T> std::string numToHex(T c);
    template <typename T> std::string numToHex(T c)
    {
        std::stringstream s;
        s.fill('0');
        s << std::setw(sizeof(T) * 2) << std::hex << static_cast<unsigned long long>(c);
        return s.str();
    }
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

    static inline std::string addLeadingZeros(std::string base, size_t minStringLength = 0)
    {
        if (base.length() >= minStringLength) {
            return base;
        }
        constexpr auto leadingDigit = '0';
        base.insert(0, minStringLength - base.length(), leadingDigit);
        return base;
    }

    template <typename T> std::string to_string(T t)
    {
        std::ostringstream ss;
        ss << t;
        return ss.str();
    }

    template <> inline std::string to_string<long double>(long double t)
    {
        uint32_t precision = 6;
        int base           = static_cast<int>(t);
        long double frac   = (t - base) * pow(10, precision);
        auto baseAsStr     = std::to_string(base);
        if (t < 0) {
            frac *= -1;
            if (base == 0) {
                baseAsStr = "-0";
            }
        }

        auto fractionalPart = static_cast<unsigned long int>(roundl(frac));
        auto fractionalPartLength = std::to_string(fractionalPart).length();
        if (fractionalPartLength > precision) {
            base += 1;
            baseAsStr      = std::to_string(base);
            fractionalPart = 0;
        }
        if (fractionalPart == 0) {
            if (baseAsStr == "-0") {
                return "0";
            }
            return baseAsStr;
        }
        auto fractionalAsStr = std::to_string(fractionalPart);
        if (fractionalAsStr.size() < precision) {
            fractionalAsStr.insert(0, precision - fractionalAsStr.size(), '0');
        }
        if (fractionalAsStr.find_last_not_of('0') != std::string::npos) {
            fractionalAsStr.erase(fractionalAsStr.find_last_not_of('0') + 1);
        }
        return baseAsStr + "." + fractionalAsStr;
    }

    template <> inline std::string to_string<float>(float t)
    {
        return to_string((long double)(t));
    }

    template <> inline std::string to_string<double>(double t)
    {
        return to_string((long double)(t));
    }

    template <typename T>[[nodiscard]] const std::string enumToString(const T &t)
    {
        static_assert(std::is_enum_v<T>);
        return std::string(magic_enum::enum_name(t));
    }

    /// Gets arithmetic value of type T from string
    ///
    /// @param value to be converted
    /// @return Value casted to type T
    template <typename T>[[nodiscard]] T getNumericValue(const std::string &value)
    {
        static_assert(std::is_arithmetic_v<T>);
        if (value.empty()) {
            return {};
        }
        T ret;
        std::istringstream(value) >> ret;
        return ret;
    }

    static inline bool toNumeric(const std::string &text, int &value)
    {
        try {
            value = std::stoi(text);
        }
        catch (const std::exception &e) {
            LOG_ERROR("toNumeric exception %s", e.what());
            return false;
        }
        return true;
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

    static inline uint32_t swapBytes(uint32_t toSwap)
    {
#ifdef __GNUC__
        return __builtin_bswap32(toSwap);
#else
        return ((((toSwap)&0xff000000) >> 24) | (((toSwap)&0x00ff0000) >> 8) | (((toSwap)&0x0000ff00) << 8) |
                (((toSwap)&0x000000ff) << 24));
#endif
    }

    namespace filesystem
    {
        [[nodiscard]] long int filelength(std::FILE *file) noexcept;
        void computeCRC32(std::FILE *file, unsigned long *outCrc32) noexcept;
        [[nodiscard]] std::string generateRandomId(std::size_t length = 0) noexcept;
        [[nodiscard]] std::string getline(std::FILE *stream, uint32_t length = 1024) noexcept;
    } // namespace filesystem
} // namespace utils
