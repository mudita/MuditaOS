#include "AtResponse.hpp"
#include <json/json11.hpp>

namespace harness
{

    std::string AtResponse::encode()
    {
        json11::Json::object el;
        el[Type] = static_cast<int>(evt);
        el[Data] = resp;
        return json11::Json(el).dump();
    }

} // namespace harness
