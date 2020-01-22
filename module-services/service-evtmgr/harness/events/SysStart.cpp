#include "SysStart.hpp"
#include <json/json11.hpp>

namespace harness
{

    using namespace json11;

    std::string SysStart::encode()
    {
        json11::Json::object el;
        el["t"] = (int)evt;
        return Json(el).dump();
    }

}; // namespace harness
