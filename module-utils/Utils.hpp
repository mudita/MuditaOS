#pragma once
#include "i18/i18.hpp"
#include <algorithm> // std::find_if_not
#include <log/log.hpp>
#include <sstream>

namespace utils
{

static const std::string WHITESPACE = " \n\r\t\f\v";

template <typename Out> void split(const std::string &s, char delim, Out result)
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim))
    {
        *(result++) = item;
    }
}

static inline std::vector<std::string> split(const std::string &s, char delim)
{
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
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

static inline std::string replaceString(std::string subject, const std::string &search, const std::string &replace)
{
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos)
    {
        subject.replace(pos, search.length(), replace);
        pos += replace.length();
    }
    return subject;
}

static inline std::string formatContactName(std::shared_ptr<ContactRecord> contact)
{
    return (contact->primaryName + " " + contact->alternativeName);
}

static inline std::string fillContactData(const std::string &baseString, std::shared_ptr<ContactRecord> contact)
{
    std::string t = replaceString(baseString, "%CONTACT_NAME%", formatContactName(contact));
    t = replaceString(t, "%CONTACT_NUMBER1%", (contact.get()->numbers.size() >= 1) ? contact.get()->numbers[0].numberE164.c_str() : "");
    return (t);
}

template <typename... Args> static inline std::string stringFormatted(const std::string &format, Args... args)
{
    size_t size = snprintf(nullptr, 0, format.c_str(), args...) + 1; // Extra space for '\0'
    std::unique_ptr<char[]> buf(new char[size]);
    snprintf(buf.get(), size, format.c_str(), args...);
    return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}
} // namespace utils
