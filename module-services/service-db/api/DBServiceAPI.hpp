
/*
 * @file DBServiceAPI.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 06.06.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#ifndef PUREPHONE_DBSERVICEAPI_HPP
#define PUREPHONE_DBSERVICEAPI_HPP

#include "Service/Bus.hpp"
#include "Interface/SettingsRecord.hpp"
#include "Interface/SMSRecord.hpp"
#include "Interface/ThreadRecord.hpp"
#include "Interface/ContactRecord.hpp"
#include "Interface/AlarmsRecord.hpp"
#include "Interface/NotesRecord.hpp"

class DBServiceAPI {

public:
    static SettingsRecord SettingsGet(sys::Service* serv);
    static bool SettingsUpdate(sys::Service* serv,const SettingsRecord& rec);

    static bool SMSAdd(sys::Service* serv,const SMSRecord& rec);
    static bool SMSRemove(sys::Service* serv,uint32_t id);
    static bool SMSUpdate(sys::Service* serv,const SMSRecord& rec);
    static std::unique_ptr<std::vector<SMSRecord>> SMSGetLimitOffset(sys::Service *serv,uint32_t offset,uint32_t limit);
    static std::unique_ptr<std::vector<SMSRecord>> SMSGetLimitOffsetByThreadID(sys::Service *serv,uint32_t offset,uint32_t limit,uint32_t id);

    static ThreadRecord ThreadGet(sys::Service* serv,uint32_t id);
    static bool ThreadRemove(sys::Service* serv,uint32_t id);
    static std::unique_ptr<std::vector<ThreadRecord>> ThreadGetLimitOffset(sys::Service *serv,uint32_t offset,uint32_t limit);

    static ContactRecordInterface::VerifyResult ContactAdd(sys::Service* serv,const ContactRecord& rec);
    static bool ContactRemove(sys::Service* serv,uint32_t id);
    static bool ContactUpdate(sys::Service* serv,const ContactRecord& rec);
    static uint32_t ContactGetCount(sys::Service* serv);
    static std::unique_ptr<std::vector<ContactRecord>> ContactGetLimitOffset(sys::Service *serv,uint32_t offset,uint32_t limit);
    static std::unique_ptr<std::vector<ContactRecord>> ContactGetLimitOffsetByFavourites(sys::Service *serv, uint32_t offset, uint32_t limit);

    static bool AlarmAdd(sys::Service* serv,const AlarmsRecord& rec);
    static bool AlarmRemove(sys::Service* serv,uint32_t id);
    static bool AlarmUpdate(sys::Service* serv,const AlarmsRecord& rec);
    static uint32_t AlarmGetCount(sys::Service* serv);
    static std::unique_ptr<std::vector<AlarmsRecord>> AlarmGetLimitOffset(sys::Service *serv,uint32_t offset,uint32_t limit);
    static AlarmsRecord AlarmGetNext(sys::Service *serv, time_t time);

    static bool NotesAdd(sys::Service* serv,const NotesRecord& rec);
	static bool NotesRemove(sys::Service* serv,uint32_t id);
	static bool NotesUpdate(sys::Service* serv,const NotesRecord& rec);
	static uint32_t NotesGetCount(sys::Service* serv);
	static bool NotesGetLimitOffset(sys::Service *serv,uint32_t offset,uint32_t limit);

};


#endif //PUREPHONE_DBSERVICEAPI_HPP
