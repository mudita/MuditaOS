/*
 * @file ImageMessage.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 28 maj 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_SERVICES_SERVICE_EINK_MESSAGES_IMAGEMESSAGE_HPP_
#define MODULE_SERVICES_SERVICE_EINK_MESSAGES_IMAGEMESSAGE_HPP_

#include <cstdint>
#include "EinkMessage.hpp"

namespace seink {

/*
 *
 */
class ImageMessage: public EinkMessage {
protected:
	uint32_t x,y,w,h;
	uint8_t* data;
public:
	ImageMessage( uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* data);
	virtual ~ImageMessage();

	uint8_t* const getData() { return data; } ;
	uint32_t getSize() { return w*h; };
};

} /* namespace seink */

#endif /* MODULE_SERVICES_SERVICE_EINK_MESSAGES_IMAGEMESSAGE_HPP_ */
