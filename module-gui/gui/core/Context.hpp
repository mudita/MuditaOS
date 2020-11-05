// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>
#include <memory>
#include <gui/Common.hpp>

namespace gui
{

    class Context
    {
      protected:
        int16_t x = 0, y = 0;
        uint32_t w = 0, h = 0;
        std::unique_ptr<uint8_t[]> data;

      public:
        Context() = default;
        Context(uint16_t width, uint16_t height);
        Context(gui::Size size);
        virtual ~Context() = default;

        Context(const Context &) = delete;
        Context &operator=(const Context &) = delete;

        Context(Context &&);
        Context &operator=(Context &&);

        bool empty()
        {
            return data == nullptr;
        }

        /**
         * @brief Creates new context using provided coordinates. If there is no common part Context with 0 width and 0
         * height is returned.
         */
        Context *get(int16_t gx, int16_t gy, uint16_t width, uint16_t height);
        /**
         * @brief Pastes provided context into current one. Overlapping content will be inserted into current context.
         */
        void insert(int16_t ix, int16_t iy, Context *context);
        /**
         * @brief Pastes provided context into current one. Overlapping content will be inserted into current context.
         */
        void insertArea(
            int16_t ix, int16_t iy, int16_t iareaX, int16_t iareaY, int16_t iareaW, int16_t iareaH, Context *context);
        /**
         * @brief Fills whole context with specified colour;
         */
        void fill(uint8_t colour);
        /**
         * @brief returns pointer to context's data;
         */
        inline uint8_t *getData()
        {
            return data.get();
        }

        inline int16_t getX()
        {
            return x;
        };
        inline int16_t getY()
        {
            return y;
        };
        inline uint16_t getW()
        {
            return w;
        };
        inline uint16_t getH()
        {
            return h;
        };

        inline bool addressInData(const uint8_t *ptr) const
        {
            return (ptr >= data.get()) && (ptr < data.get() + w * h);
        }

        [[nodiscard]] auto bufferMemorySize() const noexcept
        {
            return w * h;
        }
    };

} /* namespace gui */
