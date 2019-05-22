/*
 * ImageManager.hpp
 *
 *  Created on: 18 maj 2019
 *      Author: robert
 */

#ifndef GUI_CORE_PIXMAPMANAGER_HPP_
#define GUI_CORE_PIXMAPMANAGER_HPP_

#include <vector>
#include <string>
#include <cstdint>
#include <cstring>

#include "PixMap.hpp"

namespace gui {

class PixMapManager {
protected:
	std::string pixMapFolder;
	std::vector<PixMap*> pixMaps;

	std::vector<std::string> getPixMapList();

	PixMap* loadPixMap( std::string filename );
	void loadPixMaps( std::string baseDirectory );

	PixMapManager();
public:
	PixMapManager( const PixMapManager& ) = delete;
	void operator= (const PixMapManager& ) = delete;

	bool init( std::string baseDirectory );
	static PixMapManager& getInstance();

	virtual ~PixMapManager();

	PixMap* getPixMap( uint32_t id );
	uint32_t getPixMapID( const std::string& name );
};

} /* namespace gui */

#endif /* GUI_CORE_PIXMAPMANAGER_HPP_ */
