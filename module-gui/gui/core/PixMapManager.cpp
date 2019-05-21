/*
 * ImageManager.cpp
 *
 *  Created on: 18 maj 2019
 *      Author: robert
 */

#include <set>
#include <fstream>
#include <string>

#include <dirent.h>
#include <unistd.h>
#include <string.h>

#include "utf8/UTF8.hpp"
#include "log/log.hpp"

#include "PixMap.hpp"
#include "PixMapManager.hpp"

namespace gui {

PixMapManager::PixMapManager() {
}

PixMapManager::~PixMapManager() {
	for( PixMap* pixMap: pixMaps )
		delete pixMap;
}

void PixMapManager::loadPixMaps( std::string baseDirectory ) {
	pixMapFolder = baseDirectory+"/images";
	std::vector<std::string> pixMapFiles = getPixMapList();

	for( std::string pixMapName : pixMapFiles ) {
		loadPixMap( pixMapName );
	}
}

#if ( __linux__ || __APPLE__ )

std::vector<std::string> splitpath(
  const std::string& str
  , const std::set<char> delimiters)
{
  std::vector<std::string> result;

  char const* pch = str.c_str();
  char const* start = pch;
  for(; *pch; ++pch)
  {
    if (delimiters.find(*pch) != delimiters.end())
    {
      if (start != pch)
      {
        std::string str(start, pch);
        result.push_back(str);
      }
      else
      {
        result.push_back("");
      }
      start = pch + 1;
    }
  }
  result.push_back(start);

  return result;
}


PixMap* PixMapManager::loadPixMap( std::string filename ) {

	std::ifstream file;

	//open the file
	file.open ( filename.c_str(), std::fstream::in | std::fstream::binary );

	//get the size of the file
	file.seekg (0, file.end);
	size_t size = file.tellg();
	file.seekg (0, file.beg);

	char* data = new char[size];
	if( data == nullptr ) {
		file.close();
		LOG_ERROR( " Failed to allocate temporary font buffer");
		return nullptr;
	}

	//read data to buffer
	file.read( data, size );

	//close file
	file.close();

	//allocate memory for new font
	PixMap* pixMap = new PixMap();
	if( pixMap->load( reinterpret_cast<uint8_t*>(data) ) != gui::Status::GUI_SUCCESS ){
		delete pixMap;
		delete[] data;
		return nullptr;
	}
	else {
		//set id and push it to vector
		pixMap->setID( pixMaps.size());
		std::set<char> delims{'/'};
		std::vector<std::string> path = splitpath( filename, delims);
		pixMap->setName( path[path.size()-1] );
		LOG_INFO("%s",path[path.size()-1].c_str());
		pixMaps.push_back( pixMap );
	}
	delete[] data;
	return pixMap;
}

//#elif // loadFont __linux__

#endif // loadFont __linux__

std::vector<std::string> PixMapManager::getPixMapList() {

	DIR *dir;
	std::vector<std::string> pixMapFiles;

	LOG_INFO( "Scanning images folder: %s", pixMapFolder.c_str());

	struct dirent *ent;
	if ((dir = opendir (pixMapFolder.c_str())) != NULL) {
	/* print all the files and directories within directory */
		while ((ent = readdir (dir)) != NULL) {
			if( ent->d_type == DT_REG ) {
//				LOG_INFO("file: %s",ent->d_name);
				pixMapFiles.push_back( pixMapFolder + "/" + ent->d_name );
			}
		}
		closedir (dir);
	} else {
	  /* could not open directory */
		LOG_ERROR("failed to open directory.");
	}
	return pixMapFiles;
}

bool PixMapManager::init( std::string baseDirectory ) {
	//load fonts from specified folder
	loadPixMaps(baseDirectory);

	return true;
}

PixMapManager& PixMapManager::getInstance(){

	static PixMapManager instance;

	return instance;
}

PixMap* PixMapManager::getPixMap( uint32_t id ) {
	if( id >= pixMaps.size())
		return nullptr;
	return pixMaps[id];
}
uint32_t PixMapManager::getPixMapID( const std::string& name ) {
	for( uint32_t i=0; i<pixMaps.size(); i++ ) {
		if( name.compare( pixMaps[i]->getName() ) == 0 )
			return i;
	}
	return 0;
}

} /* namespace gui */
