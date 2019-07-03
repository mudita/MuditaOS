/*
 * @file ImageMap.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 5 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "ImageMap.hpp"

namespace gui {

ImageMap::ImageMap() : id{0},  width{ 0 }, height{0}, data{nullptr}, name{""} {}

ImageMap::ImageMap( uint16_t w, uint16_t h, uint8_t* data ) : id{0}, width{ w }, height{ h }, name{""} {
}

ImageMap::~ImageMap() {
}
} /* namespace gui */
