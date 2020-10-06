#include "EventsEndpoint.hpp"

using namespace parserFSM;

auto EventsEndpoint::handle(Context &context) -> void
{
    switch (context.getMethod()) {
    case http::Method::get:
        helper->requestDataFromDB(context);
        break;
    case http::Method::post:
        helper->updateDBEntry(context);
        break;
    case http::Method::put:
        helper->createDBEntry(context);
        break;
    case http::Method::del:
        helper->deleteDBEntry(context);
        break;
    }
}
