// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#pragma once

#include "ImageMap.hpp"
#include <vector>
#include <string>
#include <cstdint>

namespace gui
{
    class ImageManager
    {
      private:
        ImageMap *createFallbackImage();
        std::uint32_t fallbackImageId{0};

      protected:
        std::string mapFolder;
        std::vector<ImageMap *> imageMaps;

        std::vector<std::string> getImageMapList(std::string ext);

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
        uint32_t getImageMapID(const std::string &name);
        void clear();
    };

} /* namespace gui */
