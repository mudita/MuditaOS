// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ImageManager.hpp"
#include "ImageMap.hpp"
#include "VecMap.hpp"
#include "PixMap.hpp"
#include "DrawCommand.hpp"
#include "Renderer.hpp"
#include <log/log.hpp>
#include <fstream>
#include <string>
#include <list>

namespace
{
    auto getFileSize(const std::string &path) -> std::uintmax_t
    {
        try {
            return std::filesystem::file_size(path);
        }
        catch (const std::filesystem::filesystem_error &e) {
            return 0;
        }
    }
} // namespace

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

    auto ImageManager::init(const std::filesystem::path &baseDirectory) -> bool
    {
        // Load images from specified folder
        loadImageMaps(baseDirectory);
        return true;
    }

    auto ImageManager::clear() -> void
    {
        for (auto imageMap : imageMaps) {
            LOG_INFO("Deleting image '%s'", imageMap->getName().c_str());
            delete imageMap;
        }
        imageMaps.clear();
    }

    auto ImageManager::getInstance() -> ImageManager &
    {
        static ImageManager instance;
        return instance;
    }

    auto ImageManager::getImageMap(std::uint32_t id) const -> ImageMap *
    {
        if (id >= imageMaps.size()) {
#if DEBUG_MISSING_ASSETS == 1
            LOG_ERROR("Unable to find an image by id %" PRIu32, id);
#endif
            return imageMaps[fallbackImageId];
        }
        return imageMaps[id];
    }

    auto ImageManager::getImageMapID(const std::string &name, ImageTypeSpecifier specifier) -> std::uint32_t
    {
        const auto &searchName = checkAndAddSpecifierToName(name, specifier);

        for (auto i = 0U; i < imageMaps.size(); ++i) {
            if (imageMaps[i]->getName() == searchName) {
                return i;
            }
        }
#if DEBUG_MISSING_ASSETS == 1
        LOG_ERROR("Unable to find an image '%s', using default fallback image instead", name.c_str());
#endif
        return fallbackImageId;
    }

    auto ImageManager::getImageMapList(const std::string &ext1, const std::string &ext2) const
        -> std::pair<std::vector<std::string>, std::vector<std::string>>
    {
        std::vector<std::string> ext1MapFiles;
        std::vector<std::string> ext2MapFiles;

        LOG_INFO("Scanning extensions '%s' '%s' in images folder '%s'", ext1.c_str(), ext2.c_str(), mapFolder.c_str());

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
        LOG_INFO("Total number of images: %zu", ext2MapFiles.size() + ext1MapFiles.size());
        return {ext1MapFiles, ext2MapFiles};
    }

    auto ImageManager::loadPixMap(const std::filesystem::path &filename) -> ImageMap *
    {
        const auto fileSize = getFileSize(filename);
        if (fileSize == 0) {
            return nullptr;
        }

        auto imageData = std::make_unique<std::uint8_t[]>(fileSize);

        std::ifstream input(filename, std::ios::in | std::ifstream::binary);
        if (!input.is_open()) {
            LOG_FATAL("Failed to open file '%s'", filename.c_str());
            return nullptr;
        }
        if (!input.read(reinterpret_cast<char *>(imageData.get()), static_cast<std::streamsize>(fileSize))) {
            return nullptr;
        }
        const auto bytesRead = static_cast<std::uintmax_t>(input.gcount());
        if (bytesRead != fileSize) {
            LOG_FATAL("Failed to read from file '%s', expected %" PRIuMAX "B, got %" PRIuMAX "B",
                      filename.c_str(),
                      fileSize,
                      bytesRead);
            return nullptr;
        }

        // Allocate memory for new pixmap
        auto pixMap = new (std::nothrow) PixMap();
        if (pixMap == nullptr) {
            return nullptr;
        }

        if (pixMap->load(imageData.get(), fileSize) != gui::Status::GUI_SUCCESS) {
            delete pixMap;
            return nullptr;
        }

        // Set name, id and push it to vector
        const auto &pixMapName = filename.stem();
        pixMap->setName(pixMapName);
        pixMap->setID(imageMaps.size());
        imageMaps.push_back(pixMap);

        return pixMap;
    }

    auto ImageManager::loadVecMap(const std::filesystem::path &filename) -> ImageMap *
    {
        const auto fileSize = getFileSize(filename);
        if (fileSize == 0) {
            return nullptr;
        }

        auto imageData = std::make_unique<std::uint8_t[]>(fileSize);

        std::ifstream input(filename, std::ios::in | std::ifstream::binary);
        if (!input.is_open()) {
            LOG_FATAL("Failed to open file '%s'", filename.c_str());
            return nullptr;
        }
        if (!input.read(reinterpret_cast<char *>(imageData.get()), static_cast<std::streamsize>(fileSize))) {
            return nullptr;
        }
        const auto bytesRead = static_cast<std::uintmax_t>(input.gcount());
        if (bytesRead != fileSize) {
            LOG_FATAL("Failed to read from file '%s', expected %" PRIuMAX "B, got %" PRIuMAX "B",
                      filename.c_str(),
                      fileSize,
                      bytesRead);
            return nullptr;
        }

        auto vecMap = new (std::nothrow) VecMap();
        if (vecMap == nullptr) {
            return nullptr;
        }

        if (vecMap->load(imageData.get(), fileSize) != gui::Status::GUI_SUCCESS) {
            delete vecMap;
            return nullptr;
        }

        // Set name, id and push it to vector
        const auto &pixMapName = filename.stem();
        vecMap->setName(pixMapName);
        vecMap->setID(imageMaps.size());
        imageMaps.push_back(vecMap);

        return vecMap;
    }

    auto ImageManager::addFallbackImage() -> void
    {
        const std::string fallbackImageName{"FallbackImage"};

        auto fallbackImage  = createFallbackImage();
        fallbackImageId     = imageMaps.size();
        fallbackImage->setID(fallbackImageId);
        fallbackImage->setName(fallbackImageName);
        imageMaps.push_back(fallbackImage);
    }

    auto ImageManager::loadImageMaps(const std::filesystem::path &baseDirectory) -> void
    {
        mapFolder                       = baseDirectory / "images";
        auto [pixMapFiles, vecMapFiles] = getImageMapList(".mpi", ".vpi");

        for (const auto &mapName : pixMapFiles) {
            loadPixMap(mapName);
        }
        for (const auto &mapName : vecMapFiles) {
            loadVecMap(mapName);
        }
    }

    auto ImageManager::checkAndAddSpecifierToName(const std::string &name, ImageTypeSpecifier specifier) -> std::string
    {
        if (specifier != ImageTypeSpecifier::None) {
            return name + specifierMap[specifier];
        }
        return name;
    }

    auto ImageManager::createFallbackImage() const -> ImageMap *
    {
        // Creation of square with crossed lines as fallback image
        constexpr auto squareWidth = 15;

        DrawRectangle rectangle;
        rectangle.origin = {0, 0};
        rectangle.width  = squareWidth;
        rectangle.height = squareWidth;
        rectangle.areaX  = 0;
        rectangle.areaY  = 0;
        rectangle.areaW  = squareWidth;
        rectangle.areaH  = squareWidth;

        DrawLine line1;
        line1.start = {0, 0};
        line1.end   = {squareWidth, squareWidth};

        DrawLine line2;
        line2.start = {squareWidth - 1, 0};
        line2.end   = {0, squareWidth - 1};

        Context renderContext(squareWidth, squareWidth);
        Renderer().render(renderContext, rectangle, line1, line2);

        return new PixMap(squareWidth, squareWidth, renderContext.getData());
    }
} // namespace gui
