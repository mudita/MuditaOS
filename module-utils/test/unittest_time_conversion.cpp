#include <iostream>
#include <time/time_conversion.hpp>
#include <algorithm>

/// crap null stream from internets
/// (https://stackoverflow.com/questions/8243743/is-there-a-null-stdostream-implementation-in-c-or-libraries)
class NullStream : public std::ostream
{
    class NullBuffer : public std::streambuf
    {
      public:
        int overflow(int c)
        {
            return c;
        }
    } m_nb;

  public:
    NullStream() : std::ostream(&m_nb)
    {}
};

class vfs vfs;

using namespace utils::time;

using namespace std;

const map<string, time_t> test_times = {
    {"", 0},
};

void foo(UTF8 val)
{
    std::cout << "foo val: " << val << std::endl;
}

/// test if month and day shown by foo are the same as from strftime (with proper locale)
bool test_time_day_month_format(std::ostream &outstream)
{
    const time_t date        = 1569503186;
    const std::string format = "%a %A %b %B %Z %a <3 %I:%M %p %a";
    tm date_tp               = *localtime(&date);

    char c_data[128];
    strftime(c_data, 128, format.c_str(), &date_tp);
    /// we dont care about UTC+x etc. for now and dont know how will it be handled via settings?
    /// and where and for what, for tests sake remove CEST so that behaviour would be the same
    string pattern = "CEST";
    string data    = c_data;

    auto el = search(data.begin(), data.end(), pattern.begin(), pattern.end());
    data.erase(el, std::next(el, 4));

    Time t(date);
    outstream << "\t"
              << "timestamp format:      " << format << std::endl;
    outstream << "\t"
              << "timestamp from format: " << t.str(format) << std::endl;
    outstream << "\t"
              << "strtof version:        " << data << std::endl;
    bool retval = true;
    if (!(t.str(format) == UTF8(data))) {
        std::cerr << "Data mismatch: " << std::endl
                  << "[" << t.str(format).length() << "]" << std::endl
                  << ">" << t.str(format) << "<" << std::endl
                  << " :vs: " << std::endl
                  << "[" << UTF8(data).length() << "]" << std::endl
                  << ">" << data << "<" << std::endl;
        retval = false;
    }

    outstream << "TEST: " << __FUNCTION__ << " result: " << retval << std::endl;
    return retval;
}

std::string fromtime(time_t time, std::string formatter)
{
    char cstr[128] = {0};
    tm ltm         = *localtime(&time);
    strftime(cstr, 128, formatter.c_str(), &ltm);
    return cstr;
}

bool test_time_date_format(std::ostream &outstream, std::string locale_format, std::string format, time_t time)
{
    bool retval = true;
    // prepare common point in time
    tm sometimetm = *localtime(&time);
    auto mytime   = SysTime();
    mytime.set_time(time);

    if (!(mytime.str(locale_format) == fromtime(time, format))) {
        std::cerr << "Format24HourMin errors: "
                  << "[" << fromtime(time, format).length() << "] " << fromtime(time, format) << " :vs: "
                  << "[" << mytime.str(locale_format).length() << "] " << mytime.str(locale_format) << std::endl;
        retval = false;
    }

    return retval;
}

bool test_time_format24hour_min(std::ostream &outstream)
{
    // this just shows usage, test is below
    // as converter uses str() method, this is equal to: constructor->str()->operator UTF8
    outstream << "\t"
              << "systime:            " << SysTime() << std::endl;

    time_t sometime = 1569503186;
    bool retval     = test_time_date_format(outstream,
                                        Locale::format(Locale::TimeFormat::Format24HourMin),
                                        Locale::format(Locale::TimeFormat::Format24HourMin),
                                        sometime);

    outstream << "TEST: " << __FUNCTION__ << " result: " << retval << std::endl;
    return retval;
}

bool test_time_day(std::ostream &outstream)
{
    bool retval     = true;
    time_t sometime = 1569503186;
    tm sometimetm   = *localtime(&sometime);
    auto mytime     = SysTime();
    mytime.set_time(sometime);

    if (!(mytime.day() == fromtime(sometime, "%A"))) {
        std::cerr << "Day mismatch: " << std::endl
                  << mytime.day() << std::endl
                  << " :vs: " << std::endl
                  << fromtime(sometime, "%A") << std::endl;
        retval = false;
    }
    outstream << "TEST: " << __FUNCTION__ << " result: " << retval << std::endl;
    return retval;
}

bool test_time_day_abbrew(std::ostream &outstream)
{
    bool retval     = true;
    time_t sometime = 1569503186;
    tm sometimetm   = *localtime(&sometime);
    auto mytime     = SysTime();
    mytime.set_time(sometime);

    if (!(mytime.day(true) == fromtime(sometime, "%a"))) {
        std::cerr << "Day mismatch: " << std::endl
                  << mytime.day() << std::endl
                  << " :vs: " << std::endl
                  << fromtime(sometime, "%a") << std::endl;
        retval = false;
    }
    outstream << "TEST: " << __FUNCTION__ << " result: " << retval << std::endl;
    return retval;
}

//    outstream << "\t" << "systime, month:     " << mytime.month() << std::endl;
//    outstream << "\t" << "systime, month a:   " << mytime.month(true) << std::endl;

bool test_time_from_before(std::ostream &outstream, time_t before, std::string format_expected)
{
    auto retval   = true;
    auto mytime   = SysTime(before);
    tm todaytime  = *localtime(&before);
    char buf[128] = {0};

    strftime(buf, 128, format_expected.c_str(), &todaytime);

    if (!(mytime.str() == buf)) {
        std::cerr << "Error: "
                  << "\n\t" << mytime.str() << "\n\t"
                  << " :vs:"
                  << "\n\t" << buf << "\n\t"
                  << "format_expected: " << format_expected << "\n\t" << std::endl;
    }

    outstream << "TEST: " << __FUNCTION__ << " result: " << retval << std::endl;
    return retval;
}

bool test_time_from_today_200s(std::ostream &outstream)
{
    outstream << "TEST: " << __FUNCTION__ << std::endl;
    time_t timenow;
    bsp::rtc_GetCurrentTimestamp(&timenow);

    return test_time_from_before(outstream, timenow - 200, Locale::format(Locale::Format12HourMin));
}

bool test_time_from_today_49h(std::ostream &outstream)
{
    outstream << "TEST: " << __FUNCTION__ << std::endl;
    time_t timenow;
    bsp::rtc_GetCurrentTimestamp(&timenow);

    return test_time_from_before(outstream, timenow - 3600 * 49, Locale::format(Locale::FormatLocaleDateFull));
}

bool test_time_from_today_24h(std::ostream &outstream)
{
    outstream << "TEST: " << __FUNCTION__ << std::endl;
    time_t timenow;
    bsp::rtc_GetCurrentTimestamp(&timenow);

    return test_time_from_before(outstream, timenow - 3600 * 24, Locale::yesterday().c_str());
}

int main(int argc, char *argv[])
{
    time_t time_today = 0;

    // get reference Today time
    if (bsp::rtc_GetCurrentTimestamp(&time_today)) {
        std::cerr << "Error on gettime" << std::endl;
        return -1;
    }

    // set locale to PL
    utils::localize.Switch(utils::Lang::Pl);
    std::setlocale(LC_TIME, "pl_PL.UTF-8");

    // set default output to nullstream
    // change nullst to std::cout to debug
    // auto &nullst = std::cout;
    NullStream nullst;

    auto tests = {
        test_time_day_month_format,
        test_time_format24hour_min,
        test_time_day,
        test_time_day_abbrew,
        test_time_from_today_200s,
        test_time_from_today_24h,
        test_time_from_today_49h,
    };

    int retval = 0;
    for (auto test : tests) {
        if (!test(nullst)) {
            retval = 1;
        }
    }

    return retval;
}
