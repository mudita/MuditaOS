// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#pragma once

#include "ImageMap.hpp"
#include <vector>
#include <string>
#include <cstdint>
#include <map>

namespace gui
{
    class ImageManager
    {
      private:
        std::map<ImageTypeSpecifier, std::string> specifierMap = {{ImageTypeSpecifier::None, "None"},
                                                                  {ImageTypeSpecifier::W_G, "_W_G"},
                                                                  {ImageTypeSpecifier::B_G, "_B_G"},
                                                                  {ImageTypeSpecifier::W_M, "_W_M"},
                                                                  {ImageTypeSpecifier::B_M, "_B_M"}};
        std::string checkAndAddSpecifierToName(const std::string &name, ImageTypeSpecifier specifier);
        ImageMap *createFallbackImage();
        std::uint32_t fallbackImageId{0};

      protected:
        std::string mapFolder;
        std::vector<ImageMap *> imageMaps;

        auto getImageMapList(std::string ext1, std::string ext2)
            -> std::pair<std::vector<std::string>, std::vector<std::string>>;
        ImageMap *loadPixMap(std::string filename);
        ImageMap *loadVecMap(std::string filename);
        void addFallbackImage();
        void loadImageMaps(std::string baseDirectory);

        ImageManager();

      public:
        ImageManager(const ImageManager &) = delete;
        void operator=(const ImageManager &) = delete;

        bool init(std::string baseDirectory);
        static ImageManager &getInstance();

        virtual ~ImageManager();

        ImageMap *getImageMap(uint32_t id);
        uint32_t getImageMapID(const std::string &name, ImageTypeSpecifier specifier = ImageTypeSpecifier::None);
        void clear();
    };

} /* namespace gui */
