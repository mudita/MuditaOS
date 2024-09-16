// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <cstdint>
#include <string>
#include "../Common.hpp"

namespace gui
{
    class ImageMap
    {
      public:
        enum class Type
        {
            None,
            Pixmap,
            Vecmap
        };

        ImageMap();
        ImageMap(std::uint16_t w, std::uint16_t h, std::uint8_t *data);

        virtual ~ImageMap();

        [[nodiscard]] auto getType() const -> Type
        {
            return type;
        };

        [[nodiscard]] auto getWidth() const -> std::uint16_t
        {
            return width;
        };

        [[nodiscard]] auto getHeight() const -> std::uint16_t
        {
            return height;
        };

        auto getData() -> std::uint8_t *
        {
            return data;
        };

        [[nodiscard]] auto getName() const -> std::string
        {
            return name;
        };

        [[nodiscard]] auto getID() const -> std::uint32_t
        {
            return id;
        };

        auto setID(std::uint32_t newId) -> void
        {
            id = newId;
        };

        auto setName(const std::string &newName) -> void
        {
            name = newName;
        };

        virtual auto load(std::uint8_t *imageMapData, std::uint32_t size = 0) -> gui::Status
        {
            return gui::Status::GUI_SUCCESS;
        };

      protected:
        // Id of the pixmap asigned by the pixmap manager
        std::uint32_t id;
        // Number of columns in the pixmap
        std::uint16_t width;
        // Number of rows in the image
        std::uint16_t height;
        // Data of the image
        std::uint8_t *data = nullptr;
        // File name
        std::string name;
        // Type of the image
        Type type = Type::None;
    };
} /* namespace gui */
