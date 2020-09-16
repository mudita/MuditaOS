#pragma once
#include "i18/i18.hpp"
#include <algorithm> // std::find_if_not
#include <log/log.hpp>
#include <sstream>
#include <iomanip>

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
} // namespace utils
