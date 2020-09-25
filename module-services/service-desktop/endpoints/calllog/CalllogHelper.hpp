#pragma once

#include "Common/Query.hpp"
#include "Context.hpp"
#include "Service/Service.hpp"
#include "Service/Common.hpp"
#include "DBHelper.hpp"
#include <application-phonebook/models/PhonebookModel.hpp>
#include <service-db/api/DBServiceAPI.hpp>
#include "CalllogRecord.hpp"

namespace parserFSM
{

    class CalllogHelper : public DBHelper
    {
      public:
        CalllogHelper(sys::Service *_ownerServicePtr) : DBHelper(_ownerServicePtr){};

        auto createDBEntry(Context &context) -> sys::ReturnCodes override;
        auto requestDataFromDB(Context &context) -> sys::ReturnCodes override;
        auto updateDBEntry(Context &context) -> sys::ReturnCodes override;
        auto deleteDBEntry(Context &context) -> sys::ReturnCodes override;

        auto requestCount(Context &context) -> sys::ReturnCodes;
        static auto to_json(CalllogRecord record) -> json11::Json;
        auto getCalllogCount(Context &context) -> sys::ReturnCodes;
        auto getCalllogByContactID(Context &context) -> sys::ReturnCodes;
    };

    namespace json::calllog
    {
        const inline std::string count        = "count";
        const inline std::string limit        = "limit";
        const inline std::string offset       = "offset";
        const inline std::string presentation = "presentation";
        const inline std::string date         = "date";
        const inline std::string duration     = "duration";
        const inline std::string id           = "id";
        const inline std::string type         = "type";
        const inline std::string name         = "name";
        const inline std::string contactId    = "contactId";
        const inline std::string phoneNumber  = "phoneNumber";
        const inline std::string isRead       = "isRead";

    } // namespace json::calllog
} // namespace parserFSM
