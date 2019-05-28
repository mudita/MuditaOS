/*
 * ServiceGUI.hpp
 *
 *  Created on: 22 maj 2019
 *      Author: robert
 */

#ifndef MODULE_SERVICES_SERVICE_GUI_SERVICEGUI_HPP_
#define MODULE_SERVICES_SERVICE_GUI_SERVICEGUI_HPP_

#include "Service/Service.hpp"
#include "Service/Message.hpp"

class FrameBuffer {
	uint8_t* data;
	uint32_t width;
	uint32_t height;
public:
	FrameBuffer( uint32_t w, uint32_t h ) :width{w}, height{h} {
		data = new uint8_t[w*h];
	}
	virtual ~FrameBuffer() { if( data ) delete [] data;} ;

	uint8_t* getData() { return data; };
	uint32_t getWidth() { return width; };
	uint32_t getHeight() { return height; };
	bool copy( const FrameBuffer& buf ) {
		if( (width == buf.width) && (height == buf.height) &&
			(data != nullptr) && (buf.data != nullptr )) {
			memcpy( data, buf.data, width*height );
			return true;
		}
		return false;
	}
};

class ServiceGUI: public sys::Service {

	//this is where every incomming frame is painted.
	FrameBuffer* renderBuffer;
	//this buffer is provided to eink
	FrameBuffer* einkBuffer;
public:
    ServiceGUI(const std::string& name);
    ~ServiceGUI();

    sys::Message_t DataReceivedHandler(sys::DataMessage* msgl) override;
    // Invoked when timer ticked
    void TickHandler(uint32_t id) override;

    // Invoked during initialization
    sys::ReturnCodes InitHandler() override;

    sys::ReturnCodes DeinitHandler() override;

    sys::ReturnCodes WakeUpHandler() override;

    sys::ReturnCodes SleepHandler() override;

};

#endif /* MODULE_SERVICES_SERVICE_GUI_SERVICEGUI_HPP_ */
