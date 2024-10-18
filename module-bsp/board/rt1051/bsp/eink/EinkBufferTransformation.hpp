// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ED028TC1.hpp"
#include <cstdint>

namespace bsp::eink
{
    /**
     *  This function makes rotation of the image from the standard GUI coordinate system to the coord system used by
     * the ED028TC1 display.
     *
     *  @note IT ROTATES only the 1Bpp image
     *
     *  @param uint8_t* dataIn          [in]  - input image to be translated. Each byte of that array must represent a
     *  single pixel
     *  @param uint16_t x               [in]  - x coordinate of image in pixels
     *  @param uint16_t y               [in]  - y coordinate of image in pixels
     *  @param uint16_t windowWidthPx   [in]  - width of the image in pixels
     *  @param uint16_t windowHeightPx  [in]  - height of the image in pixels
     *  @param uint8_t* dataOut         [out] - the buffer for rotated image
     *  @param invertColors [in] - Inverted if colors of the image are to be inverted, Standard otherwise
     *
     * @note Assumed dataIn coordinate system is the standard image coordinates system:
     *
     *   (0,0)   X
     *       *-------->
     *       |
     *     Y |
     *       |
     *       v
     *
     *
     * The result of the function is such a conversion of the dataIn array to make the Eink display show it the very
     * same way in its own coordinate system which is:
     *
     *              displayWidth
     *                 _______  ^
     *                |       | |
     *                |       | |
     *  displayHeight |       | |
     *                |       | |
     *                |_______| |  X
     *                   \ /    |
     *    signal tape -  | |    |
     *                          |
     *                 <--------*
     *                    Y     (0,0)
     *
     * @return
     */
    std::uint8_t *transformFrameCoordinateSystem1Bpp(const std::uint8_t *dataIn,
                                                     std::uint16_t windowWidthPx,
                                                     std::uint16_t windowHeightPx,
                                                     std::uint8_t *dataOut,
                                                     EinkDisplayColorMode invertColors);

    /**
     *  This function makes rotation of the image from the standard GUI coordinate system to the coord system used by
     * the ED028TC1 display.
     *
     *  @note IT ROTATES only the 2Bpp image
     *
     *  @param uint8_t* dataIn          [in]  - input image to be translated. Each byte of that array must represent the
     * single pixel
     *  @param uint16_t x               [in]  - x coordinate of image in pixels
     *  @param uint16_t y               [in]  - y coordinate of image in pixels
     *  @param uint16_t windowWidthPx   [in]  - width of the image in pixels
     *  @param uint16_t windowHeightPx  [in]  - height of the image in pixels
     *  @param uint8_t* dataOut         [out] - the buffer for rotated image
     *  @param invertColors [in] - Inverted if colors of the image are to be inverted, Standard otherwise
     *
     * @note Assumed dataIn coordinate system is the standard image coordinates system:
     *
     *   (0,0)   X
     *       *-------->
     *       |
     *     Y |
     *       |
     *       v
     *
     *
     * The result of the function is such a conversion of the dataIn array to make the Eink display show it the very
     * same way in its own coordinate system which is:
     *
     *              displayWidth
     *                 _______  ^
     *                |       | |
     *                |       | |
     *  displayHeight |       | |
     *                |       | |
     *                |_______| |  X
     *                   \ /    |
     *    signal tape -  | |    |
     *                          |
     *                 <--------*
     *                    Y     (0,0)
     *
     * @return
     */
    std::uint8_t *transformFrameCoordinateSystem2Bpp(const std::uint8_t *dataIn,
                                                     std::uint16_t windowWidthPx,
                                                     std::uint16_t windowHeightPx,
                                                     std::uint8_t *dataOut,
                                                     EinkDisplayColorMode invertColors);

    /**
     *  This function makes rotation of the image from the standard GUI coordinate system to the coord system used by
     * the ED028TC1 display.
     *
     *  @note IT ROTATES only the 3Bpp image
     *
     *  @param uint8_t* dataIn          [in]  - input image to be translated. Each byte of that array must represent the
     * single pixel
     *  @param uint16_t x               [in]  - x coordinate of image in pixels
     *  @param uint16_t y               [in]  - y coordinate of image in pixels
     *  @param uint16_t windowWidthPx   [in]  - width of the image in pixels
     *  @param uint16_t windowHeightPx  [in]  - height of the image in pixels
     *  @param uint8_t* dataOut         [out] - the buffer for rotated image
     *  @param invertColors [in] - Inverted if colors of the image are to be inverted, Standard otherwise
     *
     * @note Assumed dataIn coordinate system is the standard image coordinates system:
     *
     *   (0,0)   X
     *       *-------->
     *       |
     *     Y |
     *       |
     *       v
     *
     *
     * The result of the function is such a conversion of the dataIn array to make the Eink display show it the very
     * same way in its own coordinate system which is:
     *
     *              displayWidth
     *                 _______  ^
     *                |       | |
     *                |       | |
     *  displayHeight |       | |
     *                |       | |
     *                |_______| |  X
     *                   \ /    |
     *    signal tape -  | |    |
     *                          |
     *                 <--------*
     *                    Y     (0,0)
     *
     * @return
     */
    std::uint8_t *transformFrameCoordinateSystem3Bpp(const std::uint8_t *dataIn,
                                                     std::uint16_t windowWidthPx,
                                                     std::uint16_t windowHeightPx,
                                                     std::uint8_t *dataOut,
                                                     EinkDisplayColorMode invertColors);

    /**
     *  This function makes rotation of the image from the standard GUI coordinate system to the coord system used by
     * the ED028TC1 display.
     *
     *  @note IT ROTATES only the 4Bpp image
     *
     *  @param uint8_t* dataIn          [in]  - input image to be translated. Each byte of that array must represent the
     * single pixel
     *  @param uint16_t x               [in]  - x coordinate of image in pixels
     *  @param uint16_t y               [in]  - y coordinate of image in pixels
     *  @param uint16_t windowWidthPx   [in]  - width of the image in pixels
     *  @param uint16_t windowHeightPx  [in]  - height of the image in pixels
     *  @param uint8_t* dataOut         [out] - the buffer for rotated image
     *  @param invertColors [in] - Inverted if colors of the image are to be inverted, Standard otherwise
     *
     * @note Assumed dataIn coordinate system is the standard image coordinates system:
     *
     *   (0,0)   X
     *       *-------->
     *       |
     *     Y |
     *       |
     *       v
     *
     *
     * The result of the function is such a conversion of the dataIn array to make the Eink display show it the very
     * same way in its own coordinate system which is:
     *
     *              displayWidth
     *                 _______  ^
     *                |       | |
     *                |       | |
     *  displayHeight |       | |
     *                |       | |
     *                |_______| |  X
     *                   \ /    |
     *    signal tape -  | |    |
     *                          |
     *                 <--------*
     *                    Y     (0,0)
     *
     * @return
     */
    std::uint8_t *transformFrameCoordinateSystem4Bpp(const std::uint8_t *dataIn,
                                                     std::uint16_t windowWidthPx,
                                                     std::uint16_t windowHeightPx,
                                                     std::uint8_t *dataOut,
                                                     EinkDisplayColorMode invertColors);

    /**
     *  This function makes rotation of the image from the standard GUI coordinate system to the coord system used by
     * the ED028TC1 display.
     *
     *  @note IT ROTATES only the 1Bpp image. It also makes sure that the image is black and white only
     *
     *  @param uint8_t* dataIn          [in]  - input image to be translated. Each byte of that array must represent the
     * single pixel
     *  @param uint16_t x               [in]  - x coordinate of image in pixels
     *  @param uint16_t y               [in]  - y coordinate of image in pixels
     *  @param uint16_t windowWidthPx   [in]  - width of the image in pixels
     *  @param uint16_t windowHeightPx  [in]  - height of the image in pixels
     *  @param uint8_t* dataOut         [out] - the buffer for rotated image
     *  @param invertColors [in] - Inverted if colors of the image are to be inverted, Standard otherwise
     *
     * @note Assumed dataIn coordinate system is the standard image coordinates system:
     *
     *   (0,0)   X
     *       *-------->
     *       |
     *     Y |
     *       |
     *       v
     *
     *
     * The result of the function is such a conversion of the dataIn array to make the Eink display show it the very
     * same way in its own coordinate system which is:
     *
     *              displayWidth
     *                 _______  ^
     *                |       | |
     *                |       | |
     *  displayHeight |       | |
     *                |       | |
     *                |_______| |  X
     *                   \ /    |
     *    signal tape -  | |    |
     *                          |
     *                 <--------*
     *                    Y     (0,0)
     *
     * @return
     */
    std::uint8_t *transformAnimationFrameCoordinateSystem1Bpp(const std::uint8_t *dataIn,
                                                              std::uint16_t windowWidthPx,
                                                              std::uint16_t windowHeightPx,
                                                              std::uint8_t *dataOut,
                                                              EinkDisplayColorMode invertColors);

    /**
     *  This function makes rotation of the image from the standard GUI coordinate system to the coord system used by
     * the ED028TC1 display.
     *
     *  @note IT ROTATES only the 2Bpp image. It also makes sure that the image is black and white only
     *
     *  @param uint8_t* dataIn          [in]  - input image to be translated. Each byte of that array must represent the
     * single pixel
     *  @param uint16_t x               [in]  - x coordinate of image in pixels
     *  @param uint16_t y               [in]  - y coordinate of image in pixels
     *  @param uint16_t windowWidthPx   [in]  - width of the image in pixels
     *  @param uint16_t windowHeightPx  [in]  - height of the image in pixels
     *  @param uint8_t* dataOut         [out] - the buffer for rotated image
     *  @param invertColors [in] - Inverted if colors of the image are to be inverted, Standard otherwise
     *
     * @note Assumed dataIn coordinate system is the standard image coordinates system:
     *
     *   (0,0)   X
     *       *-------->
     *       |
     *     Y |
     *       |
     *       v
     *
     *
     * The result of the function is such a conversion of the dataIn array to make the Eink display show it the very
     * same way in its own coordinate system which is:
     *
     *              displayWidth
     *                 _______  ^
     *                |       | |
     *                |       | |
     *  displayHeight |       | |
     *                |       | |
     *                |_______| |  X
     *                   \ /    |
     *    signal tape -  | |    |
     *                          |
     *                 <--------*
     *                    Y     (0,0)
     *
     * @return
     */
    std::uint8_t *transformAnimationFrameCoordinateSystem2Bpp(const std::uint8_t *dataIn,
                                                              std::uint16_t windowWidthPx,
                                                              std::uint16_t windowHeightPx,
                                                              std::uint8_t *dataOut,
                                                              EinkDisplayColorMode invertColors);

    /**
     *  This function makes rotation of the image from the standard GUI coordinate system to the coord system used by
     * the ED028TC1 display.
     *
     *  @note IT ROTATES only the 3Bpp image. It also makes sure that the image is black and white only
     *
     *  @param uint8_t* dataIn          [in]  - input image to be translated. Each byte of that array must represent the
     * single pixel
     *  @param uint16_t x               [in]  - x coordinate of image in pixels
     *  @param uint16_t y               [in]  - y coordinate of image in pixels
     *  @param uint16_t windowWidthPx   [in]  - width of the image in pixels
     *  @param uint16_t windowHeightPx  [in]  - height of the image in pixels
     *  @param uint8_t* dataOut         [out] - the buffer for rotated image
     *  @param invertColors [in] - Inverted if colors of the image are to be inverted, Standard otherwise
     *
     * @note Assumed dataIn coordinate system is the standard image coordinates system:
     *
     *   (0,0)   X
     *       *-------->
     *       |
     *     Y |
     *       |
     *       v
     *
     *
     * The result of the function is such a conversion of the dataIn array to make the Eink display show it the very
     * same way in its own coordinate system which is:
     *
     *              displayWidth
     *                 _______  ^
     *                |       | |
     *                |       | |
     *  displayHeight |       | |
     *                |       | |
     *                |_______| |  X
     *                   \ /    |
     *    signal tape -  | |    |
     *                          |
     *                 <--------*
     *                    Y     (0,0)
     *
     * @return
     */
    std::uint8_t *transformAnimationFrameCoordinateSystem3Bpp(const std::uint8_t *dataIn,
                                                              std::uint16_t windowWidthPx,
                                                              std::uint16_t windowHeightPx,
                                                              std::uint8_t *dataOut,
                                                              EinkDisplayColorMode invertColors);

    /**
     *  This function makes rotation of the image from the standard GUI coordinate system to the coord system used by
     * the ED028TC1 display.
     *
     *  @note IT ROTATES only the 4Bpp image. It also makes sure that the image is black and white only
     *
     *  @param uint8_t* dataIn          [in]  - input image to be translated. Each byte of that array must represent the
     * single pixel
     *  @param uint16_t x               [in]  - x coordinate of image in pixels
     *  @param uint16_t y               [in]  - y coordinate of image in pixels
     *  @param uint16_t windowWidthPx   [in]  - width of the image in pixels
     *  @param uint16_t windowHeightPx  [in]  - height of the image in pixels
     *  @param uint8_t* dataOut         [out] - the buffer for rotated image
     *  @param invertColors [in] - Inverted if colors of the image are to be inverted, Standard otherwise
     *
     * @note Assumed dataIn coordinate system is the standard image coordinates system:
     *
     *   (0,0)   X
     *       *-------->
     *       |
     *     Y |
     *       |
     *       v
     *
     *
     * The result of the function is such a conversion of the dataIn array to make the Eink display show it the very
     * same way in its own coordinate system which is:
     *
     *              displayWidth
     *                 _______  ^
     *                |       | |
     *                |       | |
     *  displayHeight |       | |
     *                |       | |
     *                |_______| |  X
     *                   \ /    |
     *    signal tape -  | |    |
     *                          |
     *                 <--------*
     *                    Y     (0,0)
     *
     * @return
     */

    /*
     * Non-rotating version of s_EinkTransformAnimationFrameCoordinateSystem_4Bpp.
     * It is used when EINK_ROTATE_90_CLOCKWISE is not defined.
     */
    std::uint8_t *transformFrameCoordinateSystemNoRotation4Bpp(const std::uint8_t *dataIn,
                                                               std::uint16_t windowWidthPx,
                                                               std::uint16_t windowHeightPx,
                                                               std::uint8_t *dataOut,
                                                               EinkDisplayColorMode invertColors);
} // namespace bsp::eink
