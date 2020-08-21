#include "ContactsEndpoint.hpp"
#include "queries/phonebook/QueryContactGet.hpp"
#include <memory>
#include <string>

using namespace ParserStateMachine;

auto ContactsEndpoint::handle(Context &context) -> std::string
{

    switch (context.getMethod()) {
    case http::Method::get:
        helper->requestDataFromDB(context);
        break;
    case http::Method::post: // update entry
        helper->updateDBEntry(context);
        break;
    case http::Method::put:
        helper->createDBEntry(context);
        break;
    case http::Method::del:
        helper->deleteDBEntry(context);
        break;
    }

    return std::string();
}
