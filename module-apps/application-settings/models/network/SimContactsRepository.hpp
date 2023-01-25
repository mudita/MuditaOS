// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/ApplicationCommon.hpp>
#include <module-db/Interface/ContactRecord.hpp>
#include <service-cellular/CellularMessage.hpp>

class AbstractSimContactsRepository
{
  public:
    using OnReadCallback             = std::function<void()>;
    using OnDupplicatesCheckCallback = std::function<void(bool duplicatesFound)>;
    using OnSaveCallback             = std::function<void()>;

    virtual ~AbstractSimContactsRepository() noexcept = default;

    virtual const std::vector<ContactRecord> &getImportedRecords()                                              = 0;
    virtual const std::vector<ContactRecord> &getDuplicatedRecords()                                            = 0;
    virtual void read(OnReadCallback callback)                                                                  = 0;
    virtual void clear()                                                                                        = 0;
    virtual void save(const std::vector<bool> &selectedContacts, bool duplicatesFound, OnSaveCallback callback) = 0;
    virtual void findDuplicates(const std::vector<bool> &selectedContacts, OnDupplicatesCheckCallback callback) = 0;
};

class SimContactsRepository : public AbstractSimContactsRepository, public app::AsyncCallbackReceiver
{
  public:
    using NotificationData = std::pair<db::Query::Type, uint32_t>;
    explicit SimContactsRepository(app::ApplicationCommon *application);

    const std::vector<ContactRecord> &getImportedRecords() override;
    const std::vector<ContactRecord> &getDuplicatedRecords() override;
    void read(OnReadCallback callback) override;
    void clear() override;
    void save(const std::vector<bool> &selectedContacts, bool duplicatesFound, OnSaveCallback callback) override;
    void findDuplicates(const std::vector<bool> &selectedContacts, OnDupplicatesCheckCallback callback) override;
    void updateImportedRecords(const std::vector<cellular::SimContact> &simData);

  protected:
    void sendNotification(const NotificationData &notificationData);

  private:
    std::vector<ContactRecord> importedRecords;
    std::vector<ContactRecord> uniqueRecords;
    std::vector<ContactRecord> duplicatedRecords;
    app::ApplicationCommon *application;

#if DEBUG_SIM_IMPORT_DATA == 1
    void printRecordsData(const std::string &name, const std::vector<ContactRecord> &data);
#endif
};
