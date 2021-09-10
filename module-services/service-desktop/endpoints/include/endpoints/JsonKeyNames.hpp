// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
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
    inline constexpr auto fsTotal             = "fsTotal";
    inline constexpr auto fsFreePercent       = "fsFreePercent";
    inline constexpr auto fsFree              = "fsFree";
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
    inline constexpr auto fileName            = "fileName";
    inline constexpr auto fileSize            = "fileSize";
    inline constexpr auto fileCrc32           = "fileCrc32";
    inline constexpr auto update              = "update";
    inline constexpr auto updateInfo          = "updateInfo";
    inline constexpr auto updateError         = "updateError";
    inline constexpr auto errorCode           = "errorCode";
    inline constexpr auto statusCode          = "statusCode";
    inline constexpr auto updateHistory       = "updateHistory";
    inline constexpr auto versionString       = "string";
    inline constexpr auto fileExists          = "fileExists";
    inline constexpr auto version             = "version";
    inline constexpr auto task                = "task";
    inline constexpr auto state               = "state";
    inline constexpr auto success             = "success";
    inline constexpr auto request             = "request";
    inline constexpr auto finished            = "finished";
    inline constexpr auto pending             = "pending";
    inline constexpr auto location            = "location";
    inline constexpr auto reason              = "reason";
    inline constexpr auto serialNumber        = "serialNumber";

    namespace filesystem
    {
        inline constexpr auto command   = "command";
        inline constexpr auto chunkSize = "chunkSize";
        inline constexpr auto chunkNo   = "chunkNo";
        inline constexpr auto data      = "data";
        inline constexpr auto rxID      = "rxID";
        inline constexpr auto txID      = "txID";

        namespace commands
        {
            inline constexpr auto upload    = "upload";
            inline constexpr auto rm        = "rm";
            inline constexpr auto download  = "download";
            inline constexpr auto checkFile = "checkFile";
        } // namespace commands

        namespace reasons
        {
            inline constexpr auto fileDoesNotExist = "file does not exist";
        }
    } // namespace filesystem

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

    namespace usb
    {
        inline constexpr auto config          = "config";
        inline constexpr auto locked          = "locked";
        inline constexpr auto unlocked        = "unlocked";
        inline constexpr auto security        = "usbSecurity";
        inline constexpr auto phoneLockCode   = "phoneLockCode";
        inline constexpr auto phoneLockStatus = "phoneLockStatus";
        inline constexpr auto phoneLockTime   = "phoneLockTime";
    } // namespace usb

} // namespace sdesktop::endpoints::json
