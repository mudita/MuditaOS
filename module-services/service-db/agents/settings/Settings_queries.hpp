// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace settings::Statements
{
    constexpr auto getValue = R"sql(
                         SELECT value
                         FROM settings_tab  AS ST
                         WHERE ST.path = '%q'
                         COLLATE NOCASE;
                         )sql";

    constexpr auto getAllValues = R"sql(
                                        SELECT path, value
                                        FROM settings_tab;
                                        )sql";

    constexpr auto checkPathExists = R"sql(
                        SELECT COUNT(value) AS PATH_EXISTS FROM  settings_tab AS ST
                        WHERE ST.path = '%q'
                        COLLATE NOCASE;
                        )sql";

    constexpr auto CheckValueExistsInDictionary = R"sql(
                        SELECT COUNT(value) AS VALUE_EXISTS_IN_DICT FROM  dictionary_tab AS DT
                        WHERE DT.path = '%q'
                        AND DT.value = '%q'
                        COLLATE NOCASE;
                        )sql";

    constexpr auto getDictValue = R"sql(
                        SELECT value
                        FROM dictionary_tab
                        WHERE path = '%q'
                        COLLATE NOCASE;
                        )sql";

    // no duplicates in dictionary (PK = path + value)
    constexpr auto addDictValue = R"sql(
                        INSERT OR IGNORE INTO dictionary_tab (path, value)
                        VALUES ( '%q', '%q')
                        )sql";

    constexpr auto checkSettingModified = R"sql(
                        SELECT COUNT(value) AS DATA_CHANGED FROM  settings_tab AS ST
                        WHERE ST.path = '%q'
                        AND ST.value != '%q' COLLATE NOCASE ;
                        )sql";

    constexpr auto insertValue = R"sql(
                        INSERT OR REPLACE INTO settings_tab (path, value) VALUES
                        ( '%q', '%q' ) ;
                        )sql";

    constexpr auto updateValue = R"sql(
                        UPDATE settings_tab SET value = '%q' WHERE path = '%q' ;
                        )sql";

    constexpr auto clearSettingsChangedTable = R"sql(
                        DELETE FROM settings_changed_tab;
                        )sql";

    constexpr auto clearSettingsChangedRow = R"sql(
                        DELETE FROM settings_changed_tab
                        WHERE path = '%q' ;
                        )sql";

    constexpr auto getSettingsChangeTable = R"sql(
                        SELECT path, value FROM settings_changed_tab
                        COLLATE NOCASE;
                        )sql";

    constexpr auto getSettingsChangeRow = R"sql(
                        SELECT path, value FROM settings_changed_tab
                        WHERE path = '%q'
                        COLLATE NOCASE;
                        )sql";

    constexpr auto setNotification = R"sql(
                        INSERT OR REPLACE INTO notifications_tab (path, service) VALUES
                        ( '%q' , '%q' ) ;
                        )sql";

    constexpr auto getAllNotifications = R"sql(
                        SELECT path, service
                        FROM notifications_tab  AS NT;
                        )sql";

    constexpr auto clearNotificationTable = R"sql(
                        DELETE FROM notifications_tab;
                        )sql";

    constexpr auto clearNotificationdRow = R"sql(
                        DELETE FROM notifications_tab
                        WHERE path = '%q' AND service = '%q';
                        )sql";

} // namespace settings::Statements
