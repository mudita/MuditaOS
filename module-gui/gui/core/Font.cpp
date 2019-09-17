/*
 * Font.cpp
 *
 *  Created on: 25 kwi 2019
 *      Author: robert
 */

#include <unistd.h>
#include <string.h>

#include <string>
#include "Font.hpp"

#include "utf8/UTF8.hpp"
#include "log/log.hpp"
//for loading files
#include "vfs.hpp"


namespace gui {

FontGlyph::~FontGlyph() {
	if( data != nullptr )
		delete[] data;
}

gui::Status FontGlyph::load( uint8_t* data, uint32_t& offset ) {
	//character id
	memcpy( &id, data + offset, sizeof(uint16_t) ); offset += sizeof(uint16_t);
	//offset in glyph data field
	memcpy( &glyph_offset, data + offset, sizeof(uint32_t) ); offset += sizeof(uint32_t);
	//width of the character image in the texture
	memcpy( &width, data + offset, sizeof(uint16_t) ); offset += sizeof(uint16_t);
	//height of the character image in the texture
	memcpy( &height, data + offset, sizeof(uint16_t) ); offset += sizeof(uint16_t);
	//how much the current position should be offset when copying the image from the texture to the screen
	memcpy( &xoffset, data + offset, sizeof(uint16_t) ); offset += sizeof(uint16_t);
	//how much the current position should be offset when copying the image from the texture to the screen
	memcpy( &yoffset, data + offset, sizeof(uint16_t) ); offset += sizeof(uint16_t);
	//how much the current position should be advanced after drawing the character
	memcpy( &xadvance, data + offset, sizeof(uint16_t) ); offset += sizeof(uint16_t);

	return gui::Status::GUI_SUCCESS;
}

gui::Status FontGlyph::loadImage( uint8_t* data, uint32_t offset ) {

	//image data of the glyph
	this->data = new uint8_t[width*height];

	memcpy( this->data, data+offset, width*height );

	//TODO convert image to the vector of vectors where every last vector contains pairs of
	//TODO offset - from column 0, length - number of pixels that should be drawn.
	//TODO First vector contains row and pointer to the vector with pairs

	return gui::Status::GUI_SUCCESS;
}

gui::Status FontKerning::load( uint8_t* data, uint32_t& offset ) {
	//utf16 id of the first character
	memcpy( &first, data + offset, sizeof(uint16_t) ); offset += sizeof(uint16_t);
	//utf16 id of the following character
	memcpy( &second, data + offset, sizeof(uint16_t) ); offset += sizeof(uint16_t);
	//distance in pixels between beginning of first character and beginning of second character
	memcpy( &amount, data + offset, sizeof(uint16_t) ); offset += sizeof(uint16_t);

	return gui::Status::GUI_SUCCESS;
}

gui::Status FontInfo::load( uint8_t* data, uint32_t& offset ) {

	//read up to 63 chars of the fonts name
	char name[64] = {0};
	memcpy( name, data + offset, 63 ); offset += 64;
	face = std::string( name );

	//size of the true type font
	memcpy( &size, data + offset, sizeof(uint16_t) ); offset += sizeof(uint16_t);
	//flag that informs if font is bold
	memcpy( &bold, data + offset, sizeof(uint16_t) ); offset += sizeof(uint16_t);
	//flag that informs if font is italic
	memcpy( &italic, data + offset, sizeof(uint16_t) ); offset += sizeof(uint16_t);
	//flag that informs if smoothing was turned on. 1 - smoothing was turned on.
	memcpy( &smooth, data + offset, sizeof(uint16_t) ); offset += sizeof(uint16_t);
	//TODO additional space between characters????
	memcpy( &char_spacing, data + offset, sizeof(uint16_t) ); offset += sizeof(uint16_t);
	//TODO additional space between lines
	memcpy( &line_spacing, data + offset, sizeof(uint16_t) ); offset += sizeof(uint16_t);
	//distance in pixels between each line of text
	memcpy( &line_height, data + offset, sizeof(uint16_t) ); offset += sizeof(uint16_t);
	//number of pixels from the absolute top of the line to the base of the characters
	memcpy( &base, data + offset, sizeof(uint16_t) ); offset += sizeof(uint16_t);
	//width of the texture, normally used to scale the x pos of the character image
	memcpy( &scale_w, data + offset, sizeof(uint16_t) ); offset += sizeof(uint16_t);
	//height of the texture, normally used to scale the y pos of the character image
	memcpy( &scale_h, data + offset, sizeof(uint16_t) ); offset += sizeof(uint16_t);

	return gui::Status::GUI_SUCCESS;
}

Font::~Font() {
	for (std::map< uint32_t, FontGlyph* >::iterator it=glyphs.begin(); it!=glyphs.end(); ++it)
		delete it->second;
	glyphs.clear();

	for (std::map< uint32_t, std::map< uint32_t, FontKerning* >* >::iterator fit=kerning.begin(); fit!=kerning.end(); ++fit) {
		for (std::map< uint32_t, FontKerning*  >::iterator sit=fit->second->begin(); sit!=fit->second->end(); ++sit) {
			delete sit->second;
		}
		delete fit->second;
	}
	kerning.clear();
}

gui::Status Font::load( uint8_t* data ) {

	uint32_t offset = 0;

	if( info.load( data, offset ) != gui::Status::GUI_SUCCESS )
		return gui::Status::GUI_FAILURE;

	//number of glyphs in the font
	memcpy( &glyph_count, data + offset, sizeof(uint32_t) ); offset += sizeof(uint32_t);
	//offset to the beginning of the glyph data
	memcpy( &glyph_data_offset, data + offset, sizeof(uint32_t) ); offset += sizeof(uint32_t);
	//number of kerning pairs
	memcpy( &kern_count, data + offset, sizeof(uint32_t) ); offset += sizeof(uint32_t);
	//array of glyphs structures
	memcpy( &kern_data_offset, data + offset, sizeof(uint32_t) ); offset += sizeof(uint32_t);
	//offset to the beginning of the image data
	memcpy( &image_data_offset, data + offset, sizeof(uint32_t) ); offset += sizeof(uint32_t);
	//id of the font asigned by the font manager
	id = 1;

//	LOG_INFO("font: %s glyphs: %d, kern: %d", info.face.c_str(), glyph_count, kern_count );
//	LOG_INFO("glyph offset: %d", glyph_data_offset );
//	LOG_INFO("kerning offset: %d", kern_data_offset );
//	LOG_INFO("image offset: %d", image_data_offset );

	//load glyphs
	uint32_t glyphOffset = glyph_data_offset;
	for( unsigned int i=0; i<glyph_count; i++ ) {
		FontGlyph* glyph = new FontGlyph();
		glyph->load( data, glyphOffset );
		glyph->loadImage(data, glyph->glyph_offset );
		glyphs.insert( std::pair<uint32_t, FontGlyph*>( glyph->id, glyph ) );
	}

	//load kerning
	//first map contains index of the character and the map that holds values for kerning between
	//first and second character character. In second map key is the value of the second character
	//and value is the kerning between first and second element.
	uint32_t kernOffset = kern_data_offset;
	for( unsigned int i=0; i<kern_count; i++ ) {
		FontKerning* kern = new FontKerning();
		kern->load( data, kernOffset );

		//find map using first element of kerning as a key
		auto it = kerning.find( kern->first );

		//element wasn't found
		std::map< uint32_t, FontKerning* >* kernMap;
		if( it == kerning.end() ) {
			kernMap = new std::map< uint32_t, FontKerning* >();

			//insert element to the first map
			kerning.insert( std::pair< uint32_t, std::map< uint32_t, FontKerning* >* >( kern->first, kernMap));
		}
		else {
			kernMap = it->second;
		}
		kernMap->insert( std::pair<uint32_t, FontKerning* >( kern->second, kern) );
	}

	return gui::Status::GUI_SUCCESS;
}

int32_t Font::getKerning( uint32_t id1, uint32_t id2 ) {
	//search for a map with kerning for given character (id1)
	auto it1 = kerning.find( id1 );

	//if there is no map with kerning for id1 return 0;
	if( it1 == kerning.end() )
		return 0;

	std::map< uint32_t, FontKerning* >* kernMap = it1->second;

	//otherwise search for id2 in kernMap and return value or 0 if it's not found
	auto it2 = kernMap->find( id2 );
	if( it2 == kernMap->end() )
		return 0;

	FontKerning* kern = it2->second;
	return kern->amount;
}

uint32_t Font::getCharCountInSpace( const UTF8& str, const uint32_t space, uint32_t& spaceConsumed, const uint32_t& delimiter )
{
	spaceConsumed = 0;

    //width of text in pixels
	int32_t availableSpace = space;
	uint32_t stringChars = 0;
	uint16_t idLast = 0, idCurrent = 0;

	for( uint32_t i=0; i<str.length(); ++i )
	{
		if( i == 0 )
		{
			idCurrent = str[i];
			FontGlyph* glyph = glyphs.find(idCurrent)->second;

			if( glyph != NULL)
			{

				if( availableSpace - glyph->xadvance < 0 )
				{
					if( spaceConsumed )
						spaceConsumed = space - availableSpace;
					return stringChars;
				}
				availableSpace -= glyph->xadvance;
			}
		}
		else
		{
			idCurrent = str[i];
			FontGlyph* glyph = glyphs.find(idCurrent)->second;

			int32_t kern_value = getKerning( idLast, idCurrent);
			if( glyph )
			{
				if( availableSpace - (glyph->xadvance+kern_value) < 0 )
				{
					spaceConsumed = space - availableSpace;
					return stringChars;
				}
				availableSpace -= glyph->xadvance+kern_value;
			}
		}
		idLast = idCurrent;
		++stringChars;

		//check for delimiter
		if( idCurrent == delimiter ) {
			break;
		}
	}
	spaceConsumed = space - availableSpace;
	return stringChars;
}

uint32_t Font::getPixelWidth( const UTF8& str, const uint32_t start, const uint32_t count )
{

	if( count == 0 )
		return 0;

	if( str.length() == 0 )
		return 0;

	if( (start >= str.length()) || (start+count-1 >= str.length() ) )
	{
		LOG_ERROR("incorrect string index provided");
		return 0;
	}

    //width of text in pixels
	uint32_t stringPixelWidth = 0;
	uint16_t idLast = 0, idCurrent = 0;

	for( uint32_t i=0; i<count; ++i )
	{
		if( i == 0 )
		{
			idCurrent = str[start+i];
			FontGlyph* glyph = glyphs.find(idCurrent)->second;

			if( glyph != NULL)
			{
				stringPixelWidth += glyph->xadvance ;
			}
		}
		else
		{
			idCurrent = str[start+i];
			FontGlyph* glyph = glyphs.find(idCurrent)->second;

			int32_t kern_value = getKerning( idLast, idCurrent);
			if( glyph != nullptr )
			{
				stringPixelWidth += glyph->xadvance+kern_value;
			}
		}
		idLast = idCurrent;
	}

	return stringPixelWidth;
}

uint32_t Font::getPixelWidth( const UTF8& str ) {
	return getPixelWidth( str, 0, str.length() );
}

uint32_t Font::getCharPixelWidth( uint32_t charCode ) {
	FontGlyph* glyph = glyphs.find(charCode)->second;

	if( glyph != NULL)
		return  glyph->xadvance ;

	return 0;
}

uint32_t Font::getCharPixelHeight( uint32_t charCode ) {
	FontGlyph* glyph = glyphs.find(charCode)->second;

	if( glyph != NULL)
		return  glyph->height ;

	return 0;
}

FontManager::FontManager() {
}

FontManager::~FontManager() {
	clear();
}

void FontManager::clear() {
	for( Font* font: fonts ) {
		LOG_INFO("deleting font: %s", font->getName().c_str());
		delete font;
	}
	fonts.clear();
}

void FontManager::loadFonts( std::string baseDirectory ) {
	fontFolder = baseDirectory+"/fonts";
	std::vector<std::string> fontFiles = getFontsList();

	for( std::string fontName : fontFiles ) {
		LOG_INFO("Loading font: %s", fontName.c_str());
		loadFont( fontName );
	}
}

Font* FontManager::loadFont( std::string filename ) {

	auto file = vfs.fopen( filename.c_str(), "rb" );

	auto fileSize = vfs.filelength( file );

	char* fontData = new char[fileSize];
	if( fontData == nullptr ) {
		vfs.fclose( file );
		LOG_ERROR( " Failed to allocate temporary font buffer");
		return nullptr;
	}

	//read data to buffer
 	auto bytesRead= vfs.fread( fontData, 1, fileSize, file );

	//close file
	vfs.fclose( file );
	if( bytesRead != fileSize) {
		LOG_ERROR("Failed to read all file");
		delete [] fontData;
		return nullptr;
	}

	//allocate memory for new font
	Font* font = new Font();
	if( font->load( reinterpret_cast<uint8_t*>(fontData) ) != gui::Status::GUI_SUCCESS ){
		delete font;
		delete[] fontData;
		return nullptr;
	}
	else {
		//set id and push it to vector
		font->id = fonts.size();
		fonts.push_back( font );
	}
	delete[] fontData;
	return font;
}

bool hasEnding (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

std::vector<std::string> FontManager::getFontsList() {

	std::vector<std::string> fontFiles;

	LOG_INFO( "Scanning fonts folder: %s", fontFolder.c_str());
	auto dirList = vfs.listdir(fontFolder.c_str());

	for( vfs::DirectoryEntry ent : dirList ) {
		if( (ent.attributes != vfs::FileAttributes::Directory) &&
			hasEnding( ent.fileName, ".mpf") ) {
			fontFiles.push_back( fontFolder + "/" + ent.fileName );
			//TODO remove commented code
			LOG_INFO("font: %s", (fontFolder + "/" + ent.fileName).c_str());
		}
	}

	LOG_INFO("Total number of images: %d", fontFiles.size());

	return fontFiles;
}

bool FontManager::init( std::string baseDirectory ) {
	//load fonts from specified folder
	loadFonts(baseDirectory);

	return true;
}

FontManager& FontManager::getInstance(){

	static FontManager instance;

	return instance;
}

Font* FontManager::getFont( uint32_t id ) {
	if( id >= fonts.size())
		return nullptr;
	return fonts[id];
}
uint32_t FontManager::getFontID( const std::string& name ) {
	for( uint32_t i=0; i<fonts.size(); i++ ) {
		if( name.compare( fonts[i]->info.face ) == 0 )
			return i;
	}
	return 0;
}

} /* namespace gui */
