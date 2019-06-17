/*
 * @file SwitchData.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 15 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_APPS_SWITCHDATA_HPP_
#define MODULE_APPS_SWITCHDATA_HPP_

namespace app {

//class template that stores information that was sent along with switch message
class SwitchData {
public:

	SwitchData( uint8_t* data, uint32_t size ) : data{nullptr}, size{0} {
//		this->data = new uint8_t[size];
//		if( data ) {
//			memcpy( this->data, data, size );
//			this->size = size;
//		}
//		else
//			this->size = 0;
	}
	~SwitchData() {
		if( data )
			delete []data;
	}

	uint8_t* data;
	uint32_t size;
};

} /* namespace app */

#endif /* MODULE_APPS_SWITCHDATA_HPP_ */
