#include "DeveloperModeEndpoint.hpp"

using namespace parserFSM;

auto DeveloperModeEndpoint::handle(Context &context) -> void
{
    switch (context.getMethod()) {
    case http::Method::get:
        helper->processGetRequest(context);
        break;
    case http::Method::post:

        break;
    case http::Method::put:

        break;
    case http::Method::del:

        break;
    }
}
