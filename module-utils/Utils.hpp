#pragma once
#include "i18/i18.hpp"
#include <algorithm> // std::find_if_not
#include <log/log.hpp>
#include <sstream>
#include <iomanip>

namespace utils
{
    static const std::string WHITESPACE = " \n\r\t\f\v";

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

    template <typename T> std::string to_string(T t, size_t stringLength = 0)
    {
        std::ostringstream ss;
        ss << std::setfill('0') << std::setw(stringLength) << t;
        return ss.str();
    }

    static inline void findAndReplaceAll(std::string &data, std::string toSearch, std::string replaceStr)
    {
        size_t pos = data.find(toSearch);
        while (pos != std::string::npos) {
            data.replace(pos, toSearch.size(), replaceStr);
            pos = data.find(toSearch, pos + replaceStr.size());
        }
    }
} // namespace utils
