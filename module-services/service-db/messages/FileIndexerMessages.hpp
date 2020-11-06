// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#pragma once

#include <Service/Message.hpp>
#include <MessageType.hpp>
#include <module-db/Database/QueryResult.hpp>

#include <memory>
#include <utility>
#include <map>
#include <vector>

namespace FileIndexer
{
    constexpr unsigned int FILE_ID_NOT_EXISTS = 0;
    constexpr unsigned int FILE_RECORD_COLUMN_COUNT = 7;

    // refers to file_tab
    struct FileRecord
    {
        unsigned int file_id = FILE_ID_NOT_EXISTS;
        std::string path     = "";
        unsigned int size;
        unsigned int mime_type;
        unsigned int mtime;
        std::string directory = "";
        unsigned int file_type;

        [[nodiscard]] auto RecordEqual(FileRecord fr) const -> bool
        {
            bool records_equal = (fr.path == path) && (fr.mime_type == mime_type) && (fr.size == size);
            return records_equal;
        }

        FileRecord() = default;
        explicit FileRecord(QueryResult *query)
        {
            if (query->getFieldCount() == FILE_RECORD_COLUMN_COUNT) {
                file_id   = (*query)[0].getInt32();
                path      = (*query)[1].getString();
                size      = (*query)[2].getInt32();
                mime_type = (*query)[3].getInt32();
                mtime     = (*query)[4].getInt32();
                directory = (*query)[5].getString();
                file_type = (*query)[6].getInt32();
            }
        }
    };

    // refers to metadata_tab
    struct FileMetadata : FileRecord
    {
        std::map<std::string, std::string> properties; // propertyName, value
    };

    struct ListDirData : FileRecord
    {
        std::vector<FileRecord> fileList;
        unsigned int list_limit;
        unsigned int list_offset;
        unsigned int list_size;
        unsigned int count;
    };

    namespace Messages
    {
        class FileIndexerMessage : public sys::DataMessage
        {
          public:
            explicit FileIndexerMessage(MessageType type = MessageType::FileIndexer) : sys::DataMessage(type){};
            ~FileIndexerMessage() override = default;
        };

        class GetListDirMessage : public FileIndexerMessage
        {
          public:
            GetListDirMessage() = default;
            explicit GetListDirMessage(std::unique_ptr<ListDirData> listDir)
                : FileIndexerMessage(), listDir(std::move(listDir))
            {}
            std::unique_ptr<ListDirData> listDir;
        };

        class GetListDirResponseMessage : public sys::ResponseMessage
        {
          public:
            explicit GetListDirResponseMessage(std::unique_ptr<ListDirData> listDir,
                                               sys::ReturnCodes code = sys::ReturnCodes::Success)
                : sys::ResponseMessage(code), listDir(std::move(listDir))
            {}
            std::unique_ptr<ListDirData> listDir;
            unsigned int getCount()
            {
                return listDir->count;
            }
        };

        class RecordMessage : public FileIndexerMessage
        {
          public:
            RecordMessage() = default;
            explicit RecordMessage(std::unique_ptr<FileRecord> record) : FileIndexerMessage(), record(std::move(record))
            {}
            std::unique_ptr<FileRecord> record;
            FileRecord dbRecord;
        };

        class GetRecordMessage : public RecordMessage
        {
          public:
            GetRecordMessage() = default;
            explicit GetRecordMessage(std::unique_ptr<FileRecord> record) : RecordMessage(std::move(record))
            {}
        };

        class SetRecordMessage : public RecordMessage
        {
          public:
            SetRecordMessage() = default;
            SetRecordMessage(std::unique_ptr<FileRecord> record) : RecordMessage(std::move(record))
            {}
        };

        class RecordChangedMessage : public RecordMessage
        {
          public:
            RecordChangedMessage() = default;
            explicit RecordChangedMessage(std::unique_ptr<FileRecord> record, std::unique_ptr<FileRecord> old_record)
                : RecordMessage(std::move(record)), old_record(std::move(old_record))

            {}
            std::unique_ptr<FileRecord> old_record{new FileRecord()};
        };

        class GetRecordResponseMessage : public sys::ResponseMessage
        {
          public:
            explicit GetRecordResponseMessage(std::unique_ptr<FileRecord> record,
                                              sys::ReturnCodes code = sys::ReturnCodes::Success)
                : sys::ResponseMessage(code), record(std::move(record))
            {}
            std::unique_ptr<FileRecord> record;
        };

        /// Property manipulation
        class PropertyMessage : public FileIndexerMessage
        {
          public:
            PropertyMessage() = default;
            explicit PropertyMessage(std::unique_ptr<FileIndexer::FileMetadata> metaData)
                : FileIndexerMessage(), metaData(std::move(metaData))
            {}

            std::unique_ptr<FileIndexer::FileMetadata> metaData;
            FileMetadata dbMetaData;
        };

        class GetPropertyMessage : public PropertyMessage
        {
          public:
            GetPropertyMessage() = default;
            explicit GetPropertyMessage(std::unique_ptr<FileIndexer::FileMetadata> metaData)
                : PropertyMessage(std::move(metaData))
            {}
        };

        class GetAllPropertiesMessage : public PropertyMessage
        {
          public:
            GetAllPropertiesMessage() = default;
            explicit GetAllPropertiesMessage(std::unique_ptr<FileIndexer::FileMetadata> metaData)
                : PropertyMessage(std::move(metaData))
            {}
        };

        class SetPropertyMessage : public PropertyMessage
        {
          public:
            SetPropertyMessage() = default;
            SetPropertyMessage(std::unique_ptr<FileIndexer::FileMetadata> metaData)
                : PropertyMessage(std::move(metaData))
            {}
        };

        class SetPropertiesMessage : public PropertyMessage
        {
          public:
            SetPropertiesMessage() = default;
            SetPropertiesMessage(std::unique_ptr<FileIndexer::FileMetadata> metaData)
                : PropertyMessage(std::move(metaData))
            {}
        };

        class PropertyChangedMessage : public PropertyMessage
        {
          public:
            PropertyChangedMessage() = default;
            explicit PropertyChangedMessage(std::unique_ptr<FileIndexer::FileMetadata> metaData,
                                            std::unique_ptr<FileIndexer::FileMetadata> oldMetaData)
                : PropertyMessage(std::move(metaData)), oldMetaData(std::move(oldMetaData))
            {}

          protected:
            std::unique_ptr<FileMetadata> oldMetaData{new FileMetadata()};
        };

        class GetPropertyResponseMessage : public sys::ResponseMessage
        {
          public:
            explicit GetPropertyResponseMessage(std::unique_ptr<FileIndexer::FileMetadata> metaData,
                                                sys::ReturnCodes code = sys::ReturnCodes::Success)
                : sys::ResponseMessage(code), metaData(std::move(metaData))
            {}
            std::unique_ptr<FileIndexer::FileMetadata> metaData;
        };

    } // namespace Messages
} // namespace FileIndexer
