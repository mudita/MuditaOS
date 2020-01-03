#pragma once
/// beware time_t size might vary, as i.e. on linux it's long int (64b -> 8B != 4B)
/// this calss uses strftime to convert timestamp to text, but for UTF8 elements (mon,day) it uses our locale
/// as stdlib builtin locale doesn't provide way to substitute C-LOCALE

#include "time/time_locale.hpp"
#include <bsp/rtc/rtc.hpp>
#include <vector>
#include <string>
#include <log/log.hpp>

namespace utils {
namespace time {

// helper class to not put everything in time
struct Localer {
    const unsigned int abbrev_len = 3;
    /// order matters, it's used in replace_locale with enum Replacements
    const std::vector<std::string> specifiers_replacement = {"%a",  // day abbrew
                                                             "%A",  // day long
                                                             "%b",  // month abbrew
                                                             "%B",  // month long
                                                             "%Z"}; // timezone
    /// see specifiers_replacements description above
    enum Replacements {
        DayAbbrev,
        DayLong,
        MonthAbbrev,
        MonthLong,
        Timezone,
    };

    UTF8 get_replacement(Replacements val, const struct tm &timeinfo);
};

class Time : protected Localer {
    protected:
    time_t time=0;
    struct tm timeinfo;
    /// only reformat on: new format
    std::string format = "";

    explicit operator UTF8 *() const { return nullptr; }

    static const uint32_t datasize = 128;
    char data[datasize];

    /// replace day mon specifiers (first 2 characters)
    /// cant use std::replace -> due to fact that it doesn't support multiple element replace (or i cant find it)
    /// cant use string::replace -> expcetion out_of_range on size when replacing with bigger (our case)
    /// please be vary when using begin_pos/found_pos (as format in next loops might be in totally different place)
    void replace_specifiers();

  public:
    Time() {
        auto err = bsp::rtc_GetCurrentTimestamp(&time);
        if(err) {
            LOG_ERROR("rtc_GetCurrentTimestamp failure!");
        }
        timeinfo =*localtime(&time);
    }
    Time(time_t newtime) : time(newtime) { timeinfo = *localtime(&time); }

    /// set Time time_t value held (set timestamp)
    void set_time(time_t newtime);
    void set_format(std::string format) { this->format = format;}

    operator UTF8() { return str(); }
    friend std::ostream& operator<<(std::ostream &os, Time t) {
        os << t.str();
        return os;
    }

    /// get Time in any format possible via strftime
    virtual UTF8 str(std::string format="");

    /// get day UTF8 value
    UTF8 day(bool abbrev = false);

    /// get month UTF8 value
    UTF8 month(bool abbrev = false);

    // get timestamp value
    uint32_t getTime(void) { return time; };
};

/// helper class to operate on time now
/// takes timestamp and can show time in past
class SysTime : public Time {
    time_t ref_time = 0;
    public:

    bool show_textual_past = true;
    bool date_format_long = true;
    std::string today_format = "%H:%M";
    std::string long_ago_format = "%d.%m.%y";

    /// shows time in past: time_now - val in seconds
    SysTime(time_t val=0, bool date_format_long=true) : date_format_long(date_format_long) {
        before_n_sec(val);
    }

    friend std::ostream& operator<<(std::ostream &os, SysTime t) {
        os << t.str();
        return os;
    }

    /// converter -> returns time in past: (val) and stores localtime in ref_time
    void before_n_sec(time_t val);

    /// Time have str(std::string ) this one uses presets
    virtual UTF8 str(std::string format="");
};

};
};
