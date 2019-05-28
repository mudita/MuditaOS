/*
 * ImageManager.cpp
 *
 *  Created on: 18 maj 2019
 *      Author: robert
 */

#include <set>
#include <string>

#include <cstring>

//module-utils
#include "utf8/UTF8.hpp"
#include "log/log.hpp"
//module-gui
#include "PixMap.hpp"
#include "PixMapManager.hpp"
//module-vfs
#include "vfs.hpp"

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

	auto file = vfs.fopen( filename.c_str(), "rb" );

	auto fileSize = vfs.filelength( file );

	char* data = new char[fileSize];
	if( data == nullptr ) {
		vfs.fclose( file );
		LOG_ERROR( " Failed to allocate temporary font buffer");
		return nullptr;
	}

	//read data to buffer
	vfs.fread( data, 1, fileSize, file );

	//close file
	vfs.fclose( file );

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
		//TODO remove commented code
		//LOG_INFO("%s",path[path.size()-1].c_str());
		pixMaps.push_back( pixMap );
	}
	delete[] data;
	return pixMap;
}

bool hasEnding2 (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

std::vector<std::string> PixMapManager::getPixMapList() {

	std::vector<std::string> pixMapFiles;

	LOG_INFO( "Scanning images folder: %s", pixMapFolder.c_str());
	auto dirList = vfs.listdir(pixMapFolder.c_str());

	for( vfs::DirectoryEntry ent : dirList ) {
		if( (ent.attributes != vfs::FileAttributes::Directory) &&
			(hasEnding2( ent.fileName, ".mpi") ) )  {
			pixMapFiles.push_back( pixMapFolder + "/" + ent.fileName );
			//TODO remove commented code
//			LOG_INFO("font: %s", (pixMapFolder + "/" + ent.fileName).c_str());
		}
		else {
			LOG_FATAL("font: %s", (pixMapFolder + "/" + ent.fileName).c_str());
		}
	}

	LOG_INFO("Total number of images: %d", pixMapFiles.size());

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
