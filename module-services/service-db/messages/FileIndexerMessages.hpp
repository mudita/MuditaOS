// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#pragma once

#include <Service/Message.hpp>
#include <MessageType.hpp>

#include <memory>
#include <utility>
#include <map>
#include <vector>

namespace FileIndexer
{
    constexpr unsigned int FILE_ID_NOT_EXISTS = 0;

    // refers to file_tab
    struct FileRecord
    {
        unsigned int file_id;
        std::string path;
        unsigned int size;
        unsigned int mime_type;
        unsigned int mtime;
        std::string directory;
        unsigned int file_type;

        [[nodiscard]] auto RecordEqual(FileRecord fr) const -> bool
        {
            bool records_equal = (fr.path == path) && (fr.mime_type == mime_type) && (fr.size == size);
            return records_equal;
        }
    };

    struct FileList : FileRecord
    {
        unsigned int list_limit;
        unsigned int list_offset;
        unsigned int list_size;
        std::vector<FileIndexer::FileRecord> fileList;
    };

    // refers to metadata_tab
    struct FileMetadata : FileRecord
    {
        std::map<std::string, std::string> properties; // propertyName, value
    };

    namespace Messages
    {
        class FileIndexerMessage : public sys::DataMessage
        {
          public:
            explicit FileIndexerMessage(MessageType type = MessageType::FileIndexer) : sys::DataMessage(type){};
            ~FileIndexerMessage() override = default;
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
