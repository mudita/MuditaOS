// Copyright (c) 2017-2025, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

namespace sdesktop::endpoints::json
{
    namespace common
    {
        inline constexpr auto reason = "reason";
    } // namespace common

    namespace deviceInfo
    {
        inline constexpr auto batteryLevel           = "batteryLevel";
        inline constexpr auto batteryState           = "batteryState";
        inline constexpr auto selectedSim            = "selectedSim";
        inline constexpr auto sim                    = "sim";
        inline constexpr auto trayState              = "trayState";
        inline constexpr auto signalStrength         = "signalStrength";
        inline constexpr auto deviceSpaceTotal       = "deviceSpaceTotal";
        inline constexpr auto systemReservedSpace    = "systemReservedSpace";
        inline constexpr auto usedUserSpace          = "usedUserSpace";
        inline constexpr auto gitRevision            = "gitRevision";
        inline constexpr auto gitBranch              = "gitBranch";
        inline constexpr auto currentRTCTime         = "currentRTCTime";
        inline constexpr auto networkStatus          = "networkStatus";
        inline constexpr auto networkOperatorName    = "networkOperatorName";
        inline constexpr auto accessTechnology       = "accessTechnology";
        inline constexpr auto version                = "version";
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
        inline constexpr auto onboardingState        = "onboardingState";
    } // namespace deviceInfo

    namespace factoryReset
    {
        inline constexpr auto factoryRequest = "factoryRequest";
    } // namespace factoryReset

    namespace update
    {
        inline constexpr auto update = "update";
        inline constexpr auto reboot = "reboot";
    } // namespace update

    namespace sync
    {
        inline constexpr auto state = "state";
    } // namespace sync

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
        inline constexpr auto order              = "order";
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

    namespace reboot
    {
        inline constexpr auto rebootType = "rebootType";
        inline constexpr auto msc        = "msc";
        inline constexpr auto reboot     = "reboot";
        inline constexpr auto shutdown   = "shutdown";
    } // namespace reboot

    namespace timeSync
    {
        inline constexpr auto value     = "value";
        inline constexpr auto timestamp = "timestamp";
    }

    namespace quotes
    {
        inline constexpr auto quote    = "quote";
        inline constexpr auto author   = "author";
        inline constexpr auto group    = "group";
        inline constexpr auto interval = "interval";
        inline constexpr auto quoteID = "quoteID";
    } // namespace quotes

} // namespace sdesktop::endpoints::json
