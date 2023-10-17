// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint> // for uint32_t
/**
 * Types of sms message
 */
enum class SMSType : uint32_t
{
    DRAFT   = 0x01,
    FAILED  = 0x02,
    INBOX   = 0x04,
    OUTBOX  = 0x08,
    QUEUED  = 0x10,
    INPUT   = 0x12,
    UNKNOWN = 0xFF
};

enum class EntryState
{
    ALL,   /// read and unread (by user) entries
    READ,  /// only read (by user) entries
    UNREAD /// only unread (by user) entries
};

enum class ContactNumberType
{
    CELL = 0, /*!< ID defines that number is a cellphone number. */
    HOME,     /*!< ID defines that number is a home number. */
    WORK,     /*!< ID defines that number belongs to predefined work number’s group. */
    FAKS,     /*!< ID defines that number if a faks number. */
    PAGER,    /*!< ID defines that number is a pager number. */
    OTHER     /*!< ID defines that number is for device other than defined above. */
};

enum class ContactAddressType
{
    HOME = 0, /*!< ID defines that address is home address. */
    WORK,     /*!< ID defines that address is work address. */
    OTHER     /*!< ID defines that address doesn’t belong to any of the types defined above. */
};

enum class SettingsLanguage
{
    ENGLISH = 0x01,
    POLISH  = 0x02,
    GERMAN  = 0x04,
    SPANISH = 0x08,
};

enum class SettingsPinMode
{
    ALWAYS = 0,
    DAYS   = 1
};
