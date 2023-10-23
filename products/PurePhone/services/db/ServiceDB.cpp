// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <db/ServiceDB.hpp>
#include <db/PureFactorySettings.hpp>

#include <module-db/databases/EventsDB.hpp>
#include <module-db/databases/MultimediaFilesDB.hpp>
#include <module-db/databases/NotificationsDB.hpp>
#include <module-db/Interface/AlarmEventRecord.hpp>
#include <module-db/Interface/CalllogRecord.hpp>
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
#include <CrashdumpMetadataStore.hpp>
#include <product/version.hpp>

ServiceDB::~ServiceDB()
{
    eventsDB.reset();
    contactsDB.reset();
    smsDB.reset();
    notesDB.reset();
    notificationsDB.reset();
    predefinedQuotesDB.reset();
    customQuotesDB.reset();
    multimediaFilesDB.reset();

    Database::deinitialize();
    LOG_INFO("Cleaning resources");
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
        auto time   = utils::time::Scoped("DBContactAdd");
        auto msg    = static_cast<DBContactMessage *>(msgl);
        auto ret    = contactRecordInterface->Add(msg->record);
        auto record = std::make_unique<std::vector<ContactRecord>>();
        record->push_back(msg->record);
        LOG_DEBUG("Last ID %" PRIu32, msg->record.ID);
        responseMsg = std::make_shared<DBContactResponseMessage>(std::move(record), ret);
        sendUpdateNotification(db::Interface::Name::Contact, db::Query::Type::Create, msg->record.ID);
    } break;

    case MessageType::DBContactGetByID: {
        auto time    = utils::time::Scoped("DBContactGetByID");
        auto msg     = static_cast<DBContactMessage *>(msgl);
        auto ret     = (msg->withTemporary ? contactRecordInterface->GetByIdWithTemporary(msg->record.ID)
                                           : contactRecordInterface->GetByID(msg->record.ID));
        auto records = std::make_unique<std::vector<ContactRecord>>();
        records->push_back(ret);
        responseMsg = std::make_shared<DBContactResponseMessage>(
            std::move(records), true, msg->limit, msg->offset, msg->favourite, 1, MessageType::DBContactGetByID);
    } break;

    case MessageType::DBContactGetBySpeedDial: {
        auto time   = utils::time::Scoped("DBContactGetBySpeedDial");
        auto msg    = static_cast<DBContactMessage *>(msgl);
        auto ret    = contactRecordInterface->GetBySpeedDial(msg->record.speeddial);
        responseMsg = std::make_shared<DBContactResponseMessage>(std::move(ret),
                                                                 true,
                                                                 msg->limit,
                                                                 msg->offset,
                                                                 msg->favourite,
                                                                 ret->size(),
                                                                 MessageType::DBContactGetBySpeedDial);
    } break;

    case MessageType::DBContactMatchByNumber: {
        auto time = utils::time::Scoped("DBContactMatchByNumber");
        auto msg  = static_cast<DBContactNumberMessage *>(msgl);
        auto ret  = contactRecordInterface->MatchByNumber(msg->numberView);

        if (ret.has_value()) {
            responseMsg = std::make_shared<DBContactNumberResponseMessage>(
                sys::ReturnCodes::Success, std::make_unique<ContactRecord>(std::move(ret->contact)));
        }
        else {
            responseMsg = std::make_shared<DBContactNumberResponseMessage>(sys::ReturnCodes::Success,
                                                                           std::unique_ptr<ContactRecord>());
        }
    } break;

    case MessageType::DBMatchContactNumberBesidesOfContactID: {
        auto time = utils::time::Scoped("DBMatchContactNumberBesidesOfContactID");
        auto msg  = static_cast<DBMatchContactNumberBesidesOfContactIDMessage *>(msgl);
        auto ret  = contactRecordInterface->MatchByNumber(msg->numberView,
                                                         ContactRecordInterface::CreateTempContact::False,
                                                         utils::PhoneNumber::Match::POSSIBLE,
                                                         msg->contactIDToOmit);

        if (ret.has_value()) {
            responseMsg = std::make_shared<DBContactNumberResponseMessage>(
                sys::ReturnCodes::Success, std::make_unique<ContactRecord>(std::move(ret->contact)));
        }
        else {
            responseMsg = std::make_shared<DBContactNumberResponseMessage>(sys::ReturnCodes::Success,
                                                                           std::unique_ptr<ContactRecord>());
        }
    } break;

    case MessageType::DBCheckContactNumbersIsSame: {
        auto time   = utils::time::Scoped("DBCheckContactNumbersIsSame");
        auto msg    = static_cast<DBContactMessage *>(msgl);
        auto ret    = contactRecordInterface->hasContactRecordSameNumbers(msg->record);
        auto record = std::make_unique<std::vector<ContactRecord>>();
        LOG_DEBUG("Has contact same numbers: %d" PRIu32, ret);
        responseMsg = std::make_shared<DBResponseMessage>(ret, 0, MessageType::MessageTypeUninitialized);
    } break;

    case MessageType::DBContactMatchByNumberID: {
        auto time = utils::time::Scoped("DBContactMatchByNumberID");
        auto msg  = static_cast<DBMatchContactByNumberIDMessage *>(msgl);
        auto ret  = contactRecordInterface->GetByNumberID(msg->numberID);
        if (ret.has_value()) {
            responseMsg = std::make_shared<DBContactNumberResponseMessage>(
                sys::ReturnCodes::Success, std::make_unique<ContactRecord>(std::move(*ret)));
        }
        else {
            responseMsg = std::make_shared<DBContactNumberResponseMessage>(sys::ReturnCodes::Success, nullptr);
        }
    } break;

    case MessageType::DBContactRemove: {
        auto time   = utils::time::Scoped("DBContactRemove");
        auto msg    = static_cast<DBContactMessage *>(msgl);
        auto ret    = contactRecordInterface->RemoveByID(msg->id);
        responseMsg = std::make_shared<DBContactResponseMessage>(nullptr, ret);
        sendUpdateNotification(db::Interface::Name::Contact, db::Query::Type::Delete, msg->id);
    } break;

    case MessageType::DBContactUpdate: {
        auto time   = utils::time::Scoped("DBContactUpdate");
        auto msg    = static_cast<DBContactMessage *>(msgl);
        auto ret    = contactRecordInterface->Update(msg->record);
        responseMsg = std::make_shared<DBContactResponseMessage>(nullptr, ret);
        sendUpdateNotification(db::Interface::Name::Contact, db::Query::Type::Update, msg->record.ID);
    } break;

        /**
         * Calllog records
         */

    case MessageType::DBCalllogAdd: {
        auto time      = utils::time::Scoped("DBCalllogAdd");
        auto msg       = static_cast<DBCalllogMessage *>(msgl);
        auto record    = std::make_unique<std::vector<CalllogRecord>>();
        msg->record.ID = DB_ID_NONE;
        auto ret       = calllogRecordInterface->Add(msg->record);
        if (ret) {
            // return the newly added record
            msg->record = calllogRecordInterface->GetByID(calllogRecordInterface->GetLastID());
        }
        record->push_back(msg->record);
        LOG_INFO("Last ID %" PRIu32, msg->record.ID);
        responseMsg = std::make_shared<DBCalllogResponseMessage>(std::move(record), ret);
        sendUpdateNotification(db::Interface::Name::Calllog, db::Query::Type::Create, msg->record.ID);
    } break;

    case MessageType::DBCalllogRemove: {
        auto time   = utils::time::Scoped("DBCalllogRemove");
        auto msg    = static_cast<DBCalllogMessage *>(msgl);
        auto ret    = calllogRecordInterface->RemoveByID(msg->id);
        responseMsg = std::make_shared<DBCalllogResponseMessage>(nullptr, ret);
        sendUpdateNotification(db::Interface::Name::Calllog, db::Query::Type::Delete, msg->id);
    } break;

    case MessageType::DBCalllogUpdate: {
        auto time   = utils::time::Scoped("DBCalllogUpdate");
        auto msg    = static_cast<DBCalllogMessage *>(msgl);
        auto ret    = calllogRecordInterface->Update(msg->record);
        responseMsg = std::make_shared<DBCalllogResponseMessage>(nullptr, ret);
        sendUpdateNotification(db::Interface::Name::Calllog, db::Query::Type::Update, msg->record.ID);
    } break;

    case MessageType::DBSyncPackage: {
        auto time   = utils::time::Scoped("DBSyncPackage");
        auto msg    = static_cast<DBServiceMessageSyncPackage *>(msgl);
        auto ret    = StoreIntoSyncPackage({msg->syncPackagePath});
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
    eventsDB        = std::make_unique<EventsDB>((purefs::dir::getDatabasesPath() / "events.db").c_str());
    contactsDB      = std::make_unique<ContactsDB>((purefs::dir::getDatabasesPath() / "contacts.db").c_str());
    smsDB           = std::make_unique<SmsDB>((purefs::dir::getDatabasesPath() / "sms.db").c_str());
    notesDB         = std::make_unique<NotesDB>((purefs::dir::getDatabasesPath() / "notes.db").c_str());
    calllogDB       = std::make_unique<CalllogDB>((purefs::dir::getDatabasesPath() / "calllog.db").c_str());
    notificationsDB = std::make_unique<NotificationsDB>((purefs::dir::getDatabasesPath() / "notifications.db").c_str());
    predefinedQuotesDB = std::make_unique<Database>((purefs::dir::getDatabasesPath() / "predefined_quotes.db").c_str());
    customQuotesDB     = std::make_unique<Database>((purefs::dir::getDatabasesPath() / "custom_quotes.db").c_str());
    multimediaFilesDB  = std::make_unique<db::multimedia_files::MultimediaFilesDB>(
        (purefs::dir::getDatabasesPath() / "multimedia.db").c_str());

    // Create record interfaces
    alarmEventRecordInterface  = std::make_unique<AlarmEventRecordInterface>(eventsDB.get());
    contactRecordInterface     = std::make_unique<ContactRecordInterface>(contactsDB.get());
    smsRecordInterface         = std::make_unique<SMSRecordInterface>(smsDB.get(), contactsDB.get());
    threadRecordInterface      = std::make_unique<ThreadRecordInterface>(smsDB.get(), contactsDB.get());
    smsTemplateRecordInterface = std::make_unique<SMSTemplateRecordInterface>(smsDB.get());
    notesRecordInterface       = std::make_unique<NotesRecordInterface>(notesDB.get());
    calllogRecordInterface     = std::make_unique<CalllogRecordInterface>(calllogDB.get(), contactsDB.get());
    notificationsRecordInterface =
        std::make_unique<NotificationsRecordInterface>(notificationsDB.get(), contactRecordInterface.get());
    multimediaFilesRecordInterface =
        std::make_unique<db::multimedia_files::MultimediaFilesRecordInterface>(multimediaFilesDB.get());

    const auto factorySettings =
        std::make_unique<settings::PureFactorySettings>(purefs::dir::getMfgConfPath() / "personalization.json");
    databaseAgents.emplace(std::make_unique<SettingsAgent>(this, "settings_v2.db", factorySettings.get()));

    for (auto &dbAgent : databaseAgents) {
        dbAgent->registerMessages();
    }

    auto settings = std::make_unique<settings::Settings>();
    settings->init(service::ServiceProxy(shared_from_this()));

    /* Save metadata for crashdump generation purpose */
    const auto serialNumberPath =
        settings::factory::entry_key + std::string("/") + settings::factory::serial_number_key;
    Store::CrashdumpMetadata::getInstance().setSerialNumber(
        settings->getValue(serialNumberPath, settings::SettingsScope::Global));
    Store::CrashdumpMetadata::getInstance().setCommitHash(GIT_REV);
    Store::CrashdumpMetadata::getInstance().setOsVersion(VERSION);
    Store::CrashdumpMetadata::getInstance().setProductName("PurePhone");

    quotesRecordInterface =
        std::make_unique<Quotes::QuotesAgent>(predefinedQuotesDB.get(), customQuotesDB.get(), std::move(settings));

    return sys::ReturnCodes::Success;
}

bool ServiceDB::StoreIntoSyncPackage(const std::filesystem::path &syncPackagePath)
{
    if (!contactsDB->storeIntoFile(syncPackagePath / std::filesystem::path(contactsDB->getName()).filename())) {
        LOG_ERROR("Store contactsDB in sync package failed");
        return false;
    }

    if (!smsDB->storeIntoFile(syncPackagePath / std::filesystem::path(smsDB->getName()).filename())) {
        LOG_ERROR("Store smsDB in sync package failed");
        return false;
    }

    return true;
}
