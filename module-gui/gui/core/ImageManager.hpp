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

#include "ImageMap.hpp"

namespace gui {

class ImageManager {
protected:
	std::string mapFolder;
	std::vector<ImageMap*> imageMaps;

	std::vector<std::string> getImageMapList(std::string ext);

	ImageMap* loadPixMap( std::string filename );
	ImageMap* loadVecMap( std::string filename );
	void loadImageMaps( std::string baseDirectory );

	ImageManager();
public:
	ImageManager( const ImageManager& ) = delete;
	void operator= (const ImageManager& ) = delete;

	bool init( std::string baseDirectory );
	static ImageManager& getInstance();

	virtual ~ImageManager();

	ImageMap* getImageMap( uint32_t id );
	uint32_t getImageMapID( const std::string& name );
};

} /* namespace gui */

#endif /* GUI_CORE_PIXMAPMANAGER_HPP_ */
