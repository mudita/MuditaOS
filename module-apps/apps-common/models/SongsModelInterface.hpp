// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "SongContext.hpp"
#include <string>
#include <ListItemProvider.hpp>
#include <apps-common/ApplicationCommon.hpp>
#include <apps-common/DatabaseModel.hpp>
#include <module-db/Interface/MultimediaFilesRecord.hpp>

namespace app::music
{
    class SongsListItemProvider : public app::DatabaseModel<db::multimedia_files::MultimediaFilesRecord>,
                                  public gui::ListItemProvider
    {
      public:
        using OnShortReleaseCallback           = std::function<bool(const std::string &fileName)>;
        using OnLongPressCallback              = std::function<void()>;
        using OnSetNavBarTemporaryCallback     = std::function<void(const UTF8 &)>;
        using OnRestoreNavBarTemporaryCallback = std::function<void()>;
        explicit SongsListItemProvider(app::ApplicationCommon *app);
        virtual ~SongsListItemProvider() noexcept = default;

        virtual void createData(OnShortReleaseCallback shortReleaseCallback,
                                OnLongPressCallback longPressCallback,
                                OnSetNavBarTemporaryCallback navBarTemporaryMode,
                                OnRestoreNavBarTemporaryCallback navBarRestoreFromTemporaryMode) = 0;
        virtual void clearData()                                                                 = 0;
    };

    class SongsModelInterface : public SongsListItemProvider
    {
      public:
        explicit SongsModelInterface(app::ApplicationCommon *app);
        virtual ~SongsModelInterface() noexcept = default;

        virtual bool isSongPlaying() const noexcept                                                            = 0;
        virtual void setCurrentSongState(SongState songState) noexcept                                         = 0;
        virtual std::optional<audio::Token> getCurrentFileToken() const noexcept                               = 0;
        virtual std::optional<db::multimedia_files::MultimediaFilesRecord> getActivatedRecord() const noexcept = 0;
        virtual SongContext getCurrentSongContext() const noexcept                                             = 0;
        virtual void setCurrentSongContext(SongContext context)                                                = 0;
        virtual void clearCurrentSongContext()                                                                 = 0;
        virtual std::string getNextFilePath(const std::string &filePath) const                                 = 0;
        virtual std::string getPreviousFilePath(const std::string &filePath) const                             = 0;
        virtual void updateRepository(const std::string &filePath)                                             = 0;
    };
} // namespace app::music
