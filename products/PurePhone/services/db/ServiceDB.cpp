// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <db/ServiceDB.hpp>

#include <module-db/Databases/CountryCodesDB.hpp>
#include <module-db/Databases/EventsDB.hpp>
#include <module-db/Databases/MultimediaFilesDB.hpp>
#include <module-db/Databases/NotificationsDB.hpp>
#include <module-db/Interface/AlarmEventRecord.hpp>
#include <module-db/Interface/CalllogRecord.hpp>
#include <module-db/Interface/CountryCodeRecord.hpp>
#include <module-db/Interface/MultimediaFilesRecord.hpp>
#include <module-db/Interface/NotesRecord.hpp>
#include <module-db/Interface/NotificationsRecord.hpp>
#include <module-db/Interface/SMSRecord.hpp>
#include <module-db/Interface/SMSTemplateRecord.hpp>
#include <purefs/filesystem_paths.hpp>
#include <service-db/agents/quotes/QuotesAgent.hpp>
#include <service-db/agents/settings/SettingsAgent.hpp>
#include <service-db/DBCalllogMessage.hpp>
#include <service-db/DBContactMessage.hpp>
#include <service-db/DBServiceMessage.hpp>
#include <service-db/QueryMessage.hpp>
#include <time/ScopedTime.hpp>

ServiceDB::~ServiceDB()
{
    eventsDB.reset();
    contactsDB.reset();
    smsDB.reset();
    notesDB.reset();
    countryCodesDB.reset();
    notificationsDB.reset();
    quotesDB.reset();
    multimediaFilesDB.reset();

    Database::deinitialize();
    LOG_INFO("[ServiceDB] Cleaning resources");
}

db::Interface *ServiceDB::getInterface(db::Interface::Name interface)
{
    switch (interface) {
    case db::Interface::Name::AlarmEvents:
        return alarmEventRecordInterface.get();
    case db::Interface::Name::SMS:
        return smsRecordInterface.get();
    case db::Interface::Name::SMSThread:
        return threadRecordInterface.get();
    case db::Interface::Name::SMSTemplate:
        return smsTemplateRecordInterface.get();
    case db::Interface::Name::Contact:
        return contactRecordInterface.get();
    case db::Interface::Name::Notes:
        return notesRecordInterface.get();
    case db::Interface::Name::Calllog:
        return calllogRecordInterface.get();
    case db::Interface::Name::CountryCodes:
        return countryCodeRecordInterface.get();
    case db::Interface::Name::Notifications:
        return notificationsRecordInterface.get();
    case db::Interface::Name::Quotes:
        return quotesRecordInterface.get();
    case db::Interface::Name::MultimediaFiles:
        return multimediaFilesRecordInterface.get();
    }

    return nullptr;
}

sys::MessagePointer ServiceDB::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
{
    auto responseMsg = std::static_pointer_cast<sys::ResponseMessage>(ServiceDBCommon::DataReceivedHandler(msgl, resp));
    if (responseMsg) {
        return responseMsg;
    }
    auto type = static_cast<MessageType>(msgl->messageType);
    switch (type) {

        /**
         * Contact records
         */

    case MessageType::DBContactAdd: {
        auto time             = utils::time::Scoped("DBContactAdd");
        DBContactMessage *msg = reinterpret_cast<DBContactMessage *>(msgl);
        auto ret              = contactRecordInterface->Add(msg->record);
        responseMsg           = std::make_shared<DBContactResponseMessage>(nullptr, ret);
        sendUpdateNotification(db::Interface::Name::Contact, db::Query::Type::Create);
    } break;

    case MessageType::DBContactGetByID: {
        auto time             = utils::time::Scoped("DBContactGetByID");
        DBContactMessage *msg = reinterpret_cast<DBContactMessage *>(msgl);
        auto ret              = (msg->withTemporary ? contactRecordInterface->GetByIdWithTemporary(msg->record.ID)
                                                    : contactRecordInterface->GetByID(msg->record.ID));
        auto records          = std::make_unique<std::vector<ContactRecord>>();
        records->push_back(ret);
        responseMsg = std::make_shared<DBContactResponseMessage>(
            std::move(records), true, msg->limit, msg->offset, msg->favourite, 1, MessageType::DBContactGetByID);
    } break;

    case MessageType::DBContactGetBySpeedDial: {
        auto time             = utils::time::Scoped("DBContactGetBySpeedDial");
        DBContactMessage *msg = reinterpret_cast<DBContactMessage *>(msgl);
        auto ret              = contactRecordInterface->GetBySpeedDial(msg->record.speeddial);
        responseMsg           = std::make_shared<DBContactResponseMessage>(std::move(ret),
                                                                 true,
                                                                 msg->limit,
                                                                 msg->offset,
                                                                 msg->favourite,
                                                                 ret->size(),
                                                                 MessageType::DBContactGetBySpeedDial);
    } break;

    case MessageType::DBContactMatchByNumber: {
        auto time = utils::time::Scoped("DBContactMatchByNumber");
        auto *msg = dynamic_cast<DBContactNumberMessage *>(msgl);
        auto ret  = contactRecordInterface->MatchByNumber(msg->numberView);

        if (ret.has_value()) {
            responseMsg = std::make_shared<DBContactNumberResponseMessage>(
                sys::ReturnCodes::Success, std::make_unique<ContactRecord>(std::move(ret->contact)));
        }
        else {
            responseMsg = std::make_shared<DBContactNumberResponseMessage>(sys::ReturnCodes::Failure,
                                                                           std::unique_ptr<ContactRecord>());
        }
    } break;

    case MessageType::DBContactMatchByNumberID: {
        auto time = utils::time::Scoped("DBContactMatchByNumber");
        auto *msg = dynamic_cast<DBMatchContactByNumberIDMessage *>(msgl);
        auto ret  = contactRecordInterface->GetByNumberID(msg->numberID);
        if (ret.has_value()) {
            responseMsg = std::make_shared<DBContactNumberResponseMessage>(
                sys::ReturnCodes::Success, std::make_unique<ContactRecord>(std::move(*ret)));
        }
        else {
            responseMsg = std::make_shared<DBContactNumberResponseMessage>(sys::ReturnCodes::Failure, nullptr);
        }
    } break;

    case MessageType::DBContactRemove: {
        auto time             = utils::time::Scoped("DBContactRemove");
        DBContactMessage *msg = reinterpret_cast<DBContactMessage *>(msgl);
        auto ret              = contactRecordInterface->RemoveByID(msg->id);
        responseMsg           = std::make_shared<DBContactResponseMessage>(nullptr, ret);
        sendUpdateNotification(db::Interface::Name::Contact, db::Query::Type::Delete);
    } break;

    case MessageType::DBContactUpdate: {
        auto time             = utils::time::Scoped("DBContactUpdate");
        DBContactMessage *msg = reinterpret_cast<DBContactMessage *>(msgl);
        auto ret              = contactRecordInterface->Update(msg->record);
        responseMsg           = std::make_shared<DBContactResponseMessage>(nullptr, ret);
        sendUpdateNotification(db::Interface::Name::Contact, db::Query::Type::Update);
    } break;

        /**
         * Calllog records
         */

    case MessageType::DBCalllogAdd: {
        auto time             = utils::time::Scoped("DBCalllogAdd");
        DBCalllogMessage *msg = reinterpret_cast<DBCalllogMessage *>(msgl);
        auto record           = std::make_unique<std::vector<CalllogRecord>>();
        msg->record.ID        = DB_ID_NONE;
        auto ret              = calllogRecordInterface->Add(msg->record);
        if (ret) {
            // return the newly added record
            msg->record = calllogRecordInterface->GetByID(calllogRecordInterface->GetLastID());
        }
        record->push_back(msg->record);
        LOG_INFO("Last ID %" PRIu32, msg->record.ID);
        responseMsg = std::make_shared<DBCalllogResponseMessage>(std::move(record), ret);
        sendUpdateNotification(db::Interface::Name::Calllog, db::Query::Type::Create);
    } break;

    case MessageType::DBCalllogRemove: {
        auto time             = utils::time::Scoped("DBCalllogRemove");
        DBCalllogMessage *msg = reinterpret_cast<DBCalllogMessage *>(msgl);
        auto ret              = calllogRecordInterface->RemoveByID(msg->id);
        responseMsg           = std::make_shared<DBCalllogResponseMessage>(nullptr, ret);
        sendUpdateNotification(db::Interface::Name::Calllog, db::Query::Type::Delete);
    } break;

    case MessageType::DBCalllogUpdate: {
        auto time             = utils::time::Scoped("DBCalllogUpdate");
        DBCalllogMessage *msg = reinterpret_cast<DBCalllogMessage *>(msgl);
        auto ret              = calllogRecordInterface->Update(msg->record);
        responseMsg           = std::make_shared<DBCalllogResponseMessage>(nullptr, ret);
        sendUpdateNotification(db::Interface::Name::Calllog, db::Query::Type::Update);
    } break;

    case MessageType::DBServiceBackup: {
        auto time   = utils::time::Scoped("DBServiceBackup");
        auto msg    = static_cast<DBServiceMessageBackup *>(msgl);
        auto ret    = StoreIntoBackup({msg->backupPath});
        responseMsg = std::make_shared<DBServiceResponseMessage>(ret);
    } break;

    default:
        break;
    }

    if (responseMsg == nullptr) {
        return std::make_shared<sys::ResponseMessage>();
    }

    responseMsg->responseTo = msgl->messageType;
    return responseMsg;
}

sys::ReturnCodes ServiceDB::InitHandler()
{
    if (const auto returnCode = ServiceDBCommon::InitHandler(); returnCode != sys::ReturnCodes::Success) {
        return returnCode;
    }

    // Create databases
    eventsDB        = std::make_unique<EventsDB>((purefs::dir::getUserDiskPath() / "events.db").c_str());
    contactsDB      = std::make_unique<ContactsDB>((purefs::dir::getUserDiskPath() / "contacts.db").c_str());
    smsDB           = std::make_unique<SmsDB>((purefs::dir::getUserDiskPath() / "sms.db").c_str());
    notesDB         = std::make_unique<NotesDB>((purefs::dir::getUserDiskPath() / "notes.db").c_str());
    calllogDB       = std::make_unique<CalllogDB>((purefs::dir::getUserDiskPath() / "calllog.db").c_str());
    countryCodesDB  = std::make_unique<CountryCodesDB>("country-codes.db");
    notificationsDB = std::make_unique<NotificationsDB>((purefs::dir::getUserDiskPath() / "notifications.db").c_str());
    quotesDB        = std::make_unique<Database>((purefs::dir::getUserDiskPath() / "quotes.db").c_str());
    multimediaFilesDB = std::make_unique<db::multimedia_files::MultimediaFilesDB>(
        (purefs::dir::getUserDiskPath() / "multimedia.db").c_str());

    // Create record interfaces
    alarmEventRecordInterface  = std::make_unique<AlarmEventRecordInterface>(eventsDB.get());
    contactRecordInterface     = std::make_unique<ContactRecordInterface>(contactsDB.get());
    smsRecordInterface         = std::make_unique<SMSRecordInterface>(smsDB.get(), contactsDB.get());
    threadRecordInterface      = std::make_unique<ThreadRecordInterface>(smsDB.get(), contactsDB.get());
    smsTemplateRecordInterface = std::make_unique<SMSTemplateRecordInterface>(smsDB.get());
    notesRecordInterface       = std::make_unique<NotesRecordInterface>(notesDB.get());
    calllogRecordInterface     = std::make_unique<CalllogRecordInterface>(calllogDB.get(), contactsDB.get());
    countryCodeRecordInterface = std::make_unique<CountryCodeRecordInterface>(countryCodesDB.get());
    notificationsRecordInterface =
        std::make_unique<NotificationsRecordInterface>(notificationsDB.get(), contactRecordInterface.get());
    quotesRecordInterface = std::make_unique<Quotes::QuotesAgent>(quotesDB.get());
    multimediaFilesRecordInterface =
        std::make_unique<db::multimedia_files::MultimediaFilesRecordInterface>(multimediaFilesDB.get());

    databaseAgents.emplace(std::make_unique<SettingsAgent>(this, "settings_v2.db"));

    for (auto &dbAgent : databaseAgents) {
        dbAgent->initDb();
        dbAgent->registerMessages();
    }

    return sys::ReturnCodes::Success;
}

bool ServiceDB::StoreIntoBackup(const std::filesystem::path &backupPath)
{
    if (eventsDB->storeIntoFile(backupPath / std::filesystem::path(eventsDB->getName()).filename()) == false) {
        LOG_ERROR("eventsDB backup failed");
        return false;
    }

    if (contactsDB->storeIntoFile(backupPath / std::filesystem::path(contactsDB->getName()).filename()) == false) {
        LOG_ERROR("contactsDB backup failed");
        return false;
    }

    if (smsDB->storeIntoFile(backupPath / std::filesystem::path(smsDB->getName()).filename()) == false) {
        LOG_ERROR("smsDB backup failed");
        return false;
    }

    if (notesDB->storeIntoFile(backupPath / std::filesystem::path(notesDB->getName()).filename()) == false) {
        LOG_ERROR("notesDB backup failed");
        return false;
    }

    if (calllogDB->storeIntoFile(backupPath / std::filesystem::path(calllogDB->getName()).filename()) == false) {
        LOG_ERROR("calllogDB backup failed");
        return false;
    }

    if (quotesDB->storeIntoFile(backupPath / std::filesystem::path(quotesDB->getName()).filename()) == false) {
        LOG_ERROR("quotesDB backup failed");
        return false;
    }

    if (quotesDB->storeIntoFile(backupPath / std::filesystem::path(multimediaFilesDB->getName()).filename()) == false) {
        LOG_ERROR("multimediaFilesDB backup failed");
        return false;
    }

    if (notificationsDB->storeIntoFile(backupPath / std::filesystem::path(notificationsDB->getName()).filename()) ==
        false) {
        LOG_ERROR("notificationsDB backup failed");
        return false;
    }

    for (auto &db : databaseAgents) {
        if (db.get() && db.get()->getAgentName() == "settingsAgent") {

            if (db->storeIntoFile(backupPath / std::filesystem::path(db->getDbFilePath()).filename()) == false) {
                LOG_ERROR("settingsAgent backup failed");
                return false;
            }
            break;
        }
    }

    return true;
}
