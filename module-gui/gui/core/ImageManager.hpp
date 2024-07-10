// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#pragma once

#include "ImageMap.hpp"
#include <filesystem>
#include <cstdint>
#include <string>
#include <vector>
#include <map>

namespace gui
{
    class ImageManager
    {
      public:
        ImageManager(const ImageManager &) = delete;
        auto operator=(const ImageManager &) -> void = delete;

        virtual ~ImageManager();

        auto init(const std::filesystem::path &baseDirectory) -> bool;
        auto clear() -> void;
        static auto getInstance() -> ImageManager &;

        [[nodiscard]] auto getImageMap(std::uint32_t id) const -> ImageMap *;
        [[nodiscard]] auto getImageMapID(const std::string &name,
                                         ImageTypeSpecifier specifier = ImageTypeSpecifier::None) -> std::uint32_t;

      protected:
        std::filesystem::path mapFolder;
        std::vector<ImageMap *> imageMaps;

        [[nodiscard]] auto getImageMapList(const std::string &ext1, const std::string &ext2) const
            -> std::pair<std::vector<std::string>, std::vector<std::string>>;
        auto loadPixMap(const std::filesystem::path &filename) -> ImageMap *;
        auto loadVecMap(const std::filesystem::path &filename) -> ImageMap *;
        auto addFallbackImage() -> void;
        auto loadImageMaps(const std::filesystem::path &baseDirectory) -> void;

        ImageManager();

      private:
        std::map<ImageTypeSpecifier, std::string> specifierMap = {{ImageTypeSpecifier::None, "None"},
                                                                  {ImageTypeSpecifier::W_G, "_W_G"},
                                                                  {ImageTypeSpecifier::B_G, "_B_G"},
                                                                  {ImageTypeSpecifier::W_M, "_W_M"},
                                                                  {ImageTypeSpecifier::B_M, "_B_M"}};
        std::uint32_t fallbackImageId{0};

        [[nodiscard]] auto checkAndAddSpecifierToName(const std::string &name, ImageTypeSpecifier specifier)
            -> std::string;
        [[nodiscard]] auto createFallbackImage() const -> ImageMap *;
    };
} // namespace gui
