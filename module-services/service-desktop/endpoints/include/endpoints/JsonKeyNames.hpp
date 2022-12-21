// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace sdesktop::endpoints::json
{
    inline constexpr auto batteryLevel        = "batteryLevel";
    inline constexpr auto batteryState        = "batteryState";
    inline constexpr auto selectedSim         = "selectedSim";
    inline constexpr auto sim                 = "sim";
    inline constexpr auto trayState           = "trayState";
    inline constexpr auto signalStrength      = "signalStrength";
    inline constexpr auto deviceSpaceTotal    = "deviceSpaceTotal";
    inline constexpr auto systemReservedSpace = "systemReservedSpace";
    inline constexpr auto usedUserSpace       = "usedUserSpace";
    inline constexpr auto gitRevision         = "gitRevision";
    inline constexpr auto gitBranch           = "gitBranch";
    inline constexpr auto gitTag              = "gitTag";
    inline constexpr auto currentRTCTime      = "currentRTCTime";
    inline constexpr auto updateReady         = "updateReady";
    inline constexpr auto updateFileList      = "updateFileList";
    inline constexpr auto factoryRequest      = "factoryRequest";
    inline constexpr auto networkStatus       = "networkStatus";
    inline constexpr auto networkOperatorName = "networkOperatorName";
    inline constexpr auto accessTechnology    = "accessTechnology";

    inline constexpr auto update                 = "update";
    inline constexpr auto updateInfo             = "updateInfo";
    inline constexpr auto updateError            = "updateError";
    inline constexpr auto errorCode              = "errorCode";
    inline constexpr auto statusCode             = "statusCode";
    inline constexpr auto updateHistory          = "updateHistory";
    inline constexpr auto usbMscMode             = "usbMscMode";
    inline constexpr auto versionString          = "string";
    inline constexpr auto fileExists             = "fileExists";
    inline constexpr auto boot                   = "boot";
    inline constexpr auto version                = "version";
    inline constexpr auto taskId                 = "id";
    inline constexpr auto state                  = "state";
    inline constexpr auto success                = "success";
    inline constexpr auto reboot                 = "reboot";
    inline constexpr auto rebootMode             = "rebootMode";
    inline constexpr auto request                = "request";
    inline constexpr auto restore                = "restore";
    inline constexpr auto finished               = "finished";
    inline constexpr auto pending                = "pending";
    inline constexpr auto location               = "location";
    inline constexpr auto reason                 = "reason";
    inline constexpr auto serialNumber           = "serialNumber";
    inline constexpr auto caseColour             = "caseColour";
    inline constexpr auto fileList               = "fileList";
    inline constexpr auto files                  = "files";
    inline constexpr auto recoveryStatusFilePath = "recoveryStatusFilePath";
    inline constexpr auto updateFilePath         = "updateFilePath";
    inline constexpr auto backupFilePath         = "backupFilePath";
    inline constexpr auto syncFilePath           = "syncFilePath";
    inline constexpr auto mtpPath                = "mtpPath";
    inline constexpr auto deviceToken            = "deviceToken";

    namespace updateprocess
    {
        inline constexpr auto command       = "command";
        inline constexpr auto updateAborted = "updateAborted";
        namespace commands
        {
            inline constexpr auto abort = "abort";
        } // namespace commands
    }     // namespace updateprocess

    namespace messages
    {
        inline constexpr auto count            = "count";
        inline constexpr auto category         = "category";
        inline constexpr auto categoryMessage  = "message";
        inline constexpr auto categoryThread   = "thread";
        inline constexpr auto categoryTemplate = "template";
        inline constexpr auto categoryBackup   = "backup";
        inline constexpr auto categorySync     = "sync";

        inline constexpr auto limit              = "limit";
        inline constexpr auto offset             = "offset";
        inline constexpr auto totalCount         = "totalCount";
        inline constexpr auto nextPage           = "nextPage";
        inline constexpr auto entries            = "entries";
        inline constexpr auto messageBody        = "messageBody";
        inline constexpr auto messageCount       = "messageCount";
        inline constexpr auto messageID          = "messageID";
        inline constexpr auto messageType        = "messageType";
        inline constexpr auto phoneNumber        = "phoneNumber";
        inline constexpr auto createdAt          = "createdAt";
        inline constexpr auto lastUsedAt         = "lastUsedAt";
        inline constexpr auto lastUpdatedAt      = "lastUpdatedAt";
        inline constexpr auto isUnread           = "isUnread";
        inline constexpr auto contactID          = "contactID";
        inline constexpr auto number             = "number";
        inline constexpr auto numberID           = "numberID";
        inline constexpr auto threadID           = "threadID";
        inline constexpr auto messageSnippet     = "messageSnippet";
        inline constexpr auto unreadMessageCount = "unreadMessageCount";
        inline constexpr auto messageTemplate    = "messageTemplate";
        inline constexpr auto templateBody       = "templateBody";
        inline constexpr auto templateID         = "templateID";
    } // namespace messages

    namespace outbox
    {
        inline constexpr auto entries   = "entries";
        inline constexpr auto uid       = "uid";
        inline constexpr auto type      = "type";
        inline constexpr auto change    = "change";
        inline constexpr auto record_id = "record_id";
    } // namespace outbox

    namespace usb
    {
        inline constexpr auto config                = "config";
        inline constexpr auto locked                = "locked";
        inline constexpr auto unlocked              = "unlocked";
        inline constexpr auto security              = "usbSecurity";
        inline constexpr auto phoneLockCode         = "phoneLockCode";
        inline constexpr auto phoneLockStatus       = "phoneLockStatus";
        inline constexpr auto phoneLockTime         = "phoneLockTime";
        inline constexpr auto timeLeftToNextAttempt = "timeLeftToNextAttempt";
    } // namespace usb

} // namespace sdesktop::endpoints::json
