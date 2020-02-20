#include "../URC_Any.hpp"

namespace at::urc
{

    Any::Any(const std::string &str)
    {
        split(str);
    }

    void Any::split(const std::string &str)
    {
        const std::string delim = ": ";
        auto pos = str.find(delim);
        head = std::string(str, 0, pos);
        if (pos != std::string::npos)
        {
            tokens = utils::split(std::string(str.begin() + pos + delim.size(), str.end()), ',');
        } // else - everyting went to head
    }

    auto Any::is() -> bool
    {
        return head.find(what());
    }

} // namespace at::urc
