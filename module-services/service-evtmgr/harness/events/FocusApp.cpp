#include "FocusApp.hpp"
#include <json/json11.hpp>

namespace harness
{

    using namespace json11;

    std::string FocusApp::encode()
    {
        json11::Json::object el;
        el["t"] = (int)evt;
        el["d"] = std::vector<std::string>({name});
        return Json(el).dump();
    }

} // namespace harness
