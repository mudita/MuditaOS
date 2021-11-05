// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <SwitchData.hpp>
#include <module-db/Interface/MultimediaFilesRecord.hpp>
#include <tags_fetcher/TagsFetcher.hpp>

#include <chrono>
#include <memory>
#include <string>

namespace gui
{
    class BGSoundsAudioContext
    {
        db::multimedia_files::MultimediaFilesRecord sound;

      public:
        explicit BGSoundsAudioContext(const db::multimedia_files::MultimediaFilesRecord &sound) : sound{sound}
        {}
        [[nodiscard]] const db::multimedia_files::MultimediaFilesRecord &getSound() const noexcept
        {
            return sound;
        }
    };

    class BGSoundsSwitchData : public SwitchData
    {
        std::unique_ptr<BGSoundsAudioContext> audioContext;

      public:
        explicit BGSoundsSwitchData(std::unique_ptr<BGSoundsAudioContext> audioContext)
            : audioContext{std::move(audioContext)}
        {}

        [[nodiscard]] std::unique_ptr<BGSoundsAudioContext> getAudioContext()
        {
            return std::move(audioContext);
        }
    };
} // namespace gui
