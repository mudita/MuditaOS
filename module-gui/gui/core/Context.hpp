/*
 * Context.hpp
 *
 *  Created on: 6 maj 2019
 *      Author: robert
 */

#ifndef GUI_CORE_CONTEXT_HPP_
#define GUI_CORE_CONTEXT_HPP_

#include <cstdint>
#include <iostream>
#include "Common.hpp"

namespace gui
{

    class Context
    {
      protected:
        int16_t x, y;
        uint32_t w, h;
        uint8_t *data;

      public:
        Context();
        Context(uint16_t width, uint16_t height, uint8_t *buffer = nullptr);
        virtual ~Context();

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
            return data;
        };
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

        friend std::ostream &operator<<(std::ostream &out, const Context &c);
    };

} /* namespace gui */

#endif /* GUI_CORE_CONTEXT_HPP_ */
