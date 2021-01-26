// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

/*
 * ImageManager.cpp
 *
 *  Created on: 18 maj 2019
 *      Author: robert
 */

#include <set>
#include <string>

#include <cstdio>
#include <cstring>
#include <filesystem>

#include "ImageManager.hpp"
#include "utf8/UTF8.hpp"
#include "log/log.hpp"
// module-gui
#include "ImageMap.hpp"
#include "VecMap.hpp"
#include "PixMap.hpp"
#include <Utils.hpp>

namespace gui
{

    ImageManager::ImageManager()
    {}

    ImageManager::~ImageManager()
    {
        clear();
    }

    void ImageManager::loadImageMaps(std::string baseDirectory)
    {
        mapFolder                            = baseDirectory + "/images";
        std::vector<std::string> pixMapFiles = getImageMapList(".mpi");
        std::vector<std::string> vecMapFiles = getImageMapList(".vpi");

        for (std::string mapName : pixMapFiles) {
            loadPixMap(mapName);
        }
        for (std::string mapName : vecMapFiles) {
            loadVecMap(mapName);
        }
    }

    void ImageManager::clear()
    {
        for (ImageMap *imageMap : imageMaps) {
            LOG_INFO("deleting image: %s", imageMap->getName().c_str());
            delete imageMap;
        }
        imageMaps.clear();
    }

    std::vector<std::string> splitpath(const std::string &str, const std::set<char> delimiters)
    {
        std::vector<std::string> result;

        char const *pch   = str.c_str();
        char const *start = pch;
        for (; *pch; ++pch) {
            if (delimiters.find(*pch) != delimiters.end()) {
                if (start != pch) {
                    std::string str(start, pch);
                    result.push_back(str);
                }
                else {
                    result.push_back("");
                }
                start = pch + 1;
            }
        }
        result.push_back(start);

        return result;
    }

    ImageMap *ImageManager::loadPixMap(std::string filename)
    {

        auto file = std::fopen(filename.c_str(), "rb");

        auto fileSize = std::filesystem::file_size(filename);

        char *data = new char[fileSize];
        if (data == nullptr) {
            std::fclose(file);
            LOG_ERROR(" Failed to allocate temporary font buffer");
            return nullptr;
        }

        // read data to buffer
        std::fread(data, 1, fileSize, file);

        // close file
        std::fclose(file);

        // allocate memory for new font
        PixMap *pixMap = new PixMap();
        if (pixMap->load(reinterpret_cast<uint8_t *>(data), fileSize) != gui::Status::GUI_SUCCESS) {
            delete pixMap;
            delete[] data;
            return nullptr;
        }
        else {
            // set id and push it to vector
            pixMap->setID(imageMaps.size());
            std::set<char> delims{'/'};
            std::vector<std::string> path = splitpath(filename, delims);
            std::string filename          = path[path.size() - 1];
            filename                      = filename.substr(0, filename.length() - 4);

            pixMap->setName(filename);
            // TODO remove commented code
            LOG_INFO("%s", filename.c_str());
            imageMaps.push_back(pixMap);
        }
        delete[] data;
        return pixMap;
    }

    ImageMap *ImageManager::loadVecMap(std::string filename)
    {

        auto file = std::fopen(filename.c_str(), "rb");

        auto fileSize = std::filesystem::file_size(filename);

        char *data = new char[fileSize];
        if (data == nullptr) {
            std::fclose(file);
            LOG_ERROR(" Failed to allocate temporary font buffer");
            return nullptr;
        }

        // read data to buffer
        std::fread(data, 1, fileSize, file);

        // close file
        std::fclose(file);

        VecMap *vecMap = new VecMap();
        if (vecMap->load(reinterpret_cast<uint8_t *>(data), fileSize) != gui::Status::GUI_SUCCESS) {
            delete vecMap;
            delete[] data;
            return nullptr;
        }
        else {
            // set id and push it to vector
            vecMap->setID(imageMaps.size());
            std::set<char> delims{'/'};
            std::vector<std::string> path = splitpath(filename, delims);
            std::string filename          = path[path.size() - 1];
            filename                      = filename.substr(0, filename.length() - 4);
            vecMap->setName(filename);
            // TODO remove commented code
            //		LOG_INFO("%s",filename.c_str());
            imageMaps.push_back(vecMap);
        }
        delete[] data;
        return vecMap;
    }

    std::vector<std::string> ImageManager::getImageMapList(std::string ext)
    {

        std::vector<std::string> mapFiles;

        LOG_INFO("Scanning %s images folder: %s", ext.c_str(), mapFolder.c_str());

        for (const auto &entry : std::filesystem::directory_iterator(mapFolder)) {
            if (!entry.is_directory() && entry.path().extension() == ext) {
                mapFiles.push_back(entry.path().string());
            }
        }

        LOG_INFO("Total number of images: %u", static_cast<unsigned int>(mapFiles.size()));

        return mapFiles;
    }

    bool ImageManager::init(std::string baseDirectory)
    {
        // load fonts from specified folder
        loadImageMaps(baseDirectory);

        return true;
    }

    ImageManager &ImageManager::getInstance()
    {

        static ImageManager instance;

        return instance;
    }

    ImageMap *ImageManager::getImageMap(uint32_t id)
    {
        if (id >= imageMaps.size())
            return nullptr;
        return imageMaps[id];
    }
    uint32_t ImageManager::getImageMapID(const std::string &name)
    {
        for (uint32_t i = 0; i < imageMaps.size(); i++) {
            if (name.compare(imageMaps[i]->getName()) == 0)
                return i;
        }
        return 0;
    }

} /* namespace gui */
