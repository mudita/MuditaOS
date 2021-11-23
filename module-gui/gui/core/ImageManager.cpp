// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ImageManager.hpp"
#include "ImageMap.hpp"
#include "VecMap.hpp"
#include "PixMap.hpp"
#include "DrawCommand.hpp"
#include "Renderer.hpp"
#include <log/log.hpp>
#include <set>
#include <string>
#include <filesystem>
#include <list>

namespace gui
{

    ImageManager::ImageManager()
    {
        addFallbackImage();
    }

    ImageManager::~ImageManager()
    {
        clear();
    }

    void ImageManager::loadImageMaps(std::string baseDirectory)
    {
        mapFolder                       = baseDirectory + "/images";
        auto [pixMapFiles, vecMapFiles] = getImageMapList(".mpi", ".vpi");

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
        if (file == nullptr) {
            return nullptr;
        }

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
            imageMaps.push_back(pixMap);
        }
        delete[] data;
        return pixMap;
    }

    ImageMap *ImageManager::loadVecMap(std::string filename)
    {

        auto file = std::fopen(filename.c_str(), "rb");
        if (!file) {
            LOG_ERROR(" Unable to open file %s", filename.c_str());
            return nullptr;
        }

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
            imageMaps.push_back(vecMap);
        }
        delete[] data;
        return vecMap;
    }

    void ImageManager::addFallbackImage()
    {
        const std::string fallbackImageName{"FallbackImage"};

        auto *fallbackImage = createFallbackImage();
        fallbackImageId     = imageMaps.size();
        fallbackImage->setID(fallbackImageId);
        fallbackImage->setName(fallbackImageName);
        imageMaps.push_back(fallbackImage);
    }

    ImageMap *ImageManager::createFallbackImage()
    {
        // Creation of square with crossed lines as fallback image
        constexpr auto squareWidth = 15;

        std::list<std::unique_ptr<gui::DrawCommand>> commands;
        auto rectangle    = std::make_unique<DrawRectangle>();
        rectangle->origin = {0, 0};
        rectangle->width  = squareWidth;
        rectangle->height = squareWidth;
        rectangle->areaX  = 0;
        rectangle->areaY  = 0;
        rectangle->areaW  = squareWidth;
        rectangle->areaH  = squareWidth;
        commands.emplace_back(std::move(rectangle));

        auto line1   = std::make_unique<DrawLine>();
        line1->start = {0, 0};
        line1->end   = {squareWidth, squareWidth};
        commands.emplace_back(std::move(line1));

        auto line2   = std::make_unique<DrawLine>();
        line2->start = {squareWidth - 1, 0};
        line2->end   = {0, squareWidth - 1};
        commands.emplace_back(std::move(line2));

        auto renderContext = std::make_unique<Context>(squareWidth, squareWidth);
        Renderer().render(renderContext.get(), commands);

        return new PixMap(squareWidth, squareWidth, renderContext->getData());
    }

    auto ImageManager::getImageMapList(std::string ext1, std::string ext2)
        -> std::pair<std::vector<std::string>, std::vector<std::string>>
    {
        std::vector<std::string> ext1MapFiles;
        std::vector<std::string> ext2MapFiles;

        LOG_INFO("Scanning %s  %s images folder: %s", ext1.c_str(), ext2.c_str(), mapFolder.c_str());

        for (const auto &entry : std::filesystem::recursive_directory_iterator(mapFolder)) {
            if (!entry.is_directory()) {
                if (entry.path().extension() == ext1) {
                    ext1MapFiles.push_back(entry.path().string());
                }
                else if (entry.path().extension() == ext2) {
                    ext2MapFiles.push_back(entry.path().string());
                }
            }
        }
        LOG_INFO("Total number of images: %u", static_cast<unsigned int>(ext2MapFiles.size() + ext1MapFiles.size()));
        return {ext1MapFiles, ext2MapFiles};
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
        if (id >= imageMaps.size()) {
#if DEBUG_MISSING_ASSETS == 1
            LOG_ERROR("Unable to find an image by id: %" PRIu32, id);
#endif
            return imageMaps[fallbackImageId];
        }
        return imageMaps[id];
    }
    uint32_t ImageManager::getImageMapID(const std::string &name, ImageTypeSpecifier specifier)
    {
        auto searchName = checkAndAddSpecifierToName(name, specifier);

        for (uint32_t i = 0; i < imageMaps.size(); ++i) {
            if (imageMaps[i]->getName() == searchName) {
                return i;
            }
        }
#if DEBUG_MISSING_ASSETS == 1
        LOG_ERROR("Unable to find an image: %s , using deafult fallback image instead.", name.c_str());
#endif
        return fallbackImageId;
    }

    std::string ImageManager::checkAndAddSpecifierToName(const std::string &name, ImageTypeSpecifier specifier)
    {
        if (specifier != ImageTypeSpecifier::None) {
            return name + specifierMap[specifier];
        }
        else {
            return name;
        }
    }

} /* namespace gui */
