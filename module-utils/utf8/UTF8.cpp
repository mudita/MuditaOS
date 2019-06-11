/*
 * UTF8.cpp
 *
 *  Created on: 9 kwi 2019
 *      Author: robert
 */
#include <cstring>
#include <cstdint>

#include "UTF8.hpp"
#include "log/log.hpp"

static uint8_t UTF8_EXT           = 0x80;  //1000 0000
static uint8_t UTF8_EXT_MASK      = 0xC0;  //1100 0000
static uint8_t UTF8_HEADER_1      = 0x00;  //0000 0000
static uint8_t UTF8_HEADER_1_MASK = 0x80;  //1000 0000
static uint8_t UTF8_HEADER_2      = 0xC0;  //1100 0000
static uint8_t UTF8_HEADER_2_MASK = 0xE0;  //1110 0000
static uint8_t UTF8_HEADER_3      = 0xE0;  //1110 0000
static uint8_t UTF8_HEADER_3_MASK = 0xF0;  //1111 0000
static uint8_t UTF8_HEADER_4      = 0xF0;  //1111 0000
static uint8_t UTF8_HEADER_4_MASK = 0xF8;  //1111 1000

static bool UTF8_CHAR_IS_1BYTE( const char* pc) {
	return ((pc[0] & UTF8_HEADER_1_MASK) == UTF8_HEADER_1 ); }

static bool UTF8_CHAR_IS_INNER( const char& c) {
	return ((((uint8_t)c) & UTF8_EXT_MASK) == UTF8_EXT); }

static bool UTF8_CHAR_IS_2BYTE(const char* pc) {
	return (((pc)[0] & UTF8_HEADER_2_MASK) == UTF8_HEADER_2 ) &&
		UTF8_CHAR_IS_INNER((pc)[1]); }

static bool UTF8_CHAR_IS_3BYTE(const char* pc) {
	return (((pc)[0] & UTF8_HEADER_3_MASK) == UTF8_HEADER_3) &&
		UTF8_CHAR_IS_INNER((pc)[1]) &&
		UTF8_CHAR_IS_INNER((pc)[2]); }

static bool UTF8_CHAR_IS_4BYTE(const char* pc) {
	return (((pc)[0] & UTF8_HEADER_4_MASK) == UTF8_HEADER_4) &&
		UTF8_CHAR_IS_INNER((pc)[1]) &&
		UTF8_CHAR_IS_INNER((pc)[2]) &&
		UTF8_CHAR_IS_INNER((pc)[3]); }

static inline uint32_t charLength( const char* utf8Char )
{
	uint32_t ret = 0;
    //check if provided char is standars US-ASCII character. 0xxxxxxx
    if( UTF8_CHAR_IS_1BYTE( utf8Char ) )
    {
        return 1;
    }
    if( UTF8_CHAR_IS_2BYTE( utf8Char ) ) //characters number is written on 2 bytes 110xxxxx 10xxxxxx
    {
        return 2;
    }
    if( UTF8_CHAR_IS_3BYTE( utf8Char ) ) //characters number is written on 3 bytes. 1110xxxx 10xxxxxx 10xxxxxx
    {
        return 3;
    }
    if( UTF8_CHAR_IS_4BYTE( utf8Char ) ) //characters number is written on 3 bytes. 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    {
        return 4;
    }

    return ret;
}

const char* UTF8::emptyString = "";
const uint32_t UTF8::stringExpansion = 32;

UTF8::UTF8() :
	data{ new uint8_t[stringExpansion] },
	sizeAllocated{stringExpansion},
	sizeUsed{1},
	strLength{0},
	lastIndex{0},
	lastIndexData{data}
#ifdef BUILD_GTEST_TESTS
	, operator_index_iterations{0}
	#endif
	{
		if( data != nullptr ) {
			memset( data, 0, sizeAllocated );
		}
}

UTF8::UTF8( const char* str ) {
	//bufferSize increased by 1 to ensure ending 0 in new string
	sizeUsed = strlen(str) + 1 ;
	sizeAllocated = getDataBufferSize( sizeUsed );
	data = new uint8_t[sizeAllocated];
	if( data != nullptr ) {
		memset( data, 0, sizeAllocated );
		memcpy( data, str, sizeUsed );
		lastIndexData = data;
	}
	strLength = getCharactersCount( reinterpret_cast<const char*>(data) );
	lastIndex = 0;
#ifdef BUILD_GTEST_TESTS
	operator_index_iterations = 0;
#endif
}

UTF8::UTF8( const std::string& str ) {
	//bufferSize increased by 1 to ensure ending 0 in new string
	sizeUsed = str.length() + 1;
	sizeAllocated = getDataBufferSize( sizeUsed );
	data = new uint8_t[sizeAllocated];
	if( data != nullptr ) {
		memset( data, 0, sizeAllocated );
		memcpy( data, str.c_str(), sizeUsed );
		lastIndexData = data;
	}
	strLength = getCharactersCount( reinterpret_cast<const char*>(data) );
	lastIndex = 0;
#ifdef BUILD_GTEST_TESTS
	operator_index_iterations = 0;
#endif
}

UTF8::UTF8( const UTF8& utf ) {
	strLength = utf.strLength;
	sizeAllocated = utf.sizeAllocated;
	sizeUsed = utf.sizeUsed;

	//if there is any data used in the string allocate memory and copy usedSize bytes
	if( strLength ) {
		data = new uint8_t[sizeAllocated];
		if( data == nullptr ) {
			//LOG_FATAL("No memory for copy constructor.");
			sizeAllocated = 0;
			sizeUsed = 0;
			return;
		}
		memcpy( data, utf.data, sizeAllocated );
	}
	else {
		sizeAllocated = stringExpansion;
		data = new uint8_t[sizeAllocated];
		memset(data,0,sizeAllocated);
		sizeUsed = 0;
	}
	lastIndex = 0;
	lastIndexData = data;
#ifdef BUILD_GTEST_TESTS
	operator_index_iterations = 0;
#endif
}

UTF8::UTF8( const uint8_t* data, const uint32_t allocated, const uint32_t used, const uint32_t len ) :
	sizeAllocated{allocated},
	sizeUsed{used},
	strLength{len},
	lastIndex{0}{
	this->data = new uint8_t[allocated];
	if( this->data == nullptr ) {
		sizeAllocated = 0;
		sizeUsed = 0;
		strLength = 0;
		return;
	}
	memcpy( this->data, data, allocated );
	lastIndexData = this->data;
#ifdef BUILD_GTEST_TESTS
	operator_index_iterations = 0;
#endif
}

UTF8::~UTF8() {
	if( data )
		delete [] data;
}

bool UTF8::expand( uint32_t size ) {
	return true;
}

uint32_t UTF8::getDataBufferSize( uint32_t dataBytes ){
	return ( ( ( dataBytes ) / stringExpansion ) + 1 ) * stringExpansion;
}

bool UTF8::getStreamLength( const char* stream, uint32_t& size, uint32_t& count ) {
	size = 0;
	count = 0;
	while( (*stream) != 0 ){
		//calculate individual char's byte length.
		int32_t byte_length = charLength( stream);
		//if char is valid add length and increment pointer
		if( byte_length > 0 )
		{
			size += byte_length;
			++count;
			stream += byte_length;
		}
		else {
			size = 0;
			count = 0;
			return false;
		}
	}
	return true;
}

UTF8& UTF8::operator=( const UTF8& utf ) {

	//prevent from making assignment to itself
	if( this == &utf ) {
		return *this;
	}

	sizeAllocated = utf.sizeAllocated;
	sizeUsed = utf.sizeUsed;
	strLength = utf.strLength;

	if( data )
		delete[] data;

	data = new uint8_t[sizeAllocated];
	memcpy( data, utf.data, sizeAllocated );

	lastIndex = 0;
	lastIndexData = data;

	return *this;
}

UTF8& UTF8::operator= ( UTF8&& utf ) noexcept {
	//prevent moving if object is moved to itself
	if( this != &utf ) {
		if( data )
			delete[] data;
		data = utf.data;
		utf.data = nullptr;
		sizeAllocated = utf.sizeAllocated;
		sizeUsed = utf.sizeUsed;
		strLength = utf.strLength;
	}
	return *this;
}

uint32_t UTF8::operator[]( const uint32_t& idx ) const{

	if(idx >= strLength)
		return 0;

	uint8_t* dataPtr;
	uint32_t charCnt = 0;
	if(lastIndex < idx){
		dataPtr = lastIndexData;
		charCnt = lastIndex;
	}
	else{
		dataPtr = data;
		charCnt = 0;
	}
#ifdef BUILD_GTEST_TESTS
	operator_index_iterations = 0;
#endif
	while(charCnt != idx)
	{
		dataPtr += charLength(reinterpret_cast<const char*>(dataPtr));
		charCnt++;
#ifdef BUILD_GTEST_TESTS
		operator_index_iterations++;
#endif
	}

	lastIndex = charCnt;
	lastIndexData = dataPtr;

	uint32_t returnValue = 0;
	memcpy(static_cast<uint32_t*>(&returnValue), dataPtr, charLength(reinterpret_cast<const char*>(dataPtr)));

	return returnValue;
}

UTF8 UTF8::operator+( const UTF8& utf ) {
	//this will call copy constructor and operator += on created copy.
	return UTF8(*this)+=utf;
}

UTF8& UTF8::operator+=( const UTF8& utf ) {

	if( utf.strLength == 0 )
		return *this;

	uint32_t newSizeAllocated = getDataBufferSize( sizeUsed + utf.sizeUsed );
	uint8_t* newData = new uint8_t[newSizeAllocated];
	if( newData != nullptr ) {

		memcpy( newData, data, sizeUsed );
		//-1 comes from the fact that null terminator is counted as a used byte in string's buffer.
		memcpy( newData + sizeUsed - 1, utf.data, utf.sizeUsed );
		delete [] data;
		data = newData;
		sizeAllocated = newSizeAllocated;
		strLength += utf.strLength;
		sizeUsed += utf.sizeUsed;
		lastIndex = 0;
		lastIndexData = data;
	}
	return *this;
}

bool UTF8::operator==( const UTF8& utf ) const {
	uint32_t len = strLength - utf.strLength;
	uint32_t used = sizeUsed - utf.sizeUsed;
	if( (len | used) == 0 ){
		return !memcmp( data, utf.data, sizeUsed );
	}
	return false;
}

const char* UTF8::c_str() const {
	return reinterpret_cast<const char*>(data);
}
UTF8 UTF8::substr( const uint32_t begin, const uint32_t length) const {

	if( ( begin + length > this->length() ) ||
        ( length == 0) )
		return UTF8();

	uint8_t* beginPtr = this->data;
	uint8_t* endPtr;

	uint32_t bufferSize = 0;
	uint32_t strCounter = 0;
	//find pointer to begin char
	while(strCounter != begin)
	{
		beginPtr += charLength(reinterpret_cast<const char*>(beginPtr));
		strCounter++;
	}
	//find pinter to end char
	endPtr = beginPtr;
	for(strCounter = 0; strCounter < length; strCounter++)
	{
		uint32_t charSize = 0;
		charSize = charLength(reinterpret_cast<const char*>(endPtr));
		endPtr += charSize;
		bufferSize += charSize;
	}
	//copy data to buffer
	//bufferSize increased by 1 to ensure ending 0 in new string
	uint8_t* buffer = new uint8_t[bufferSize + 1];
	memset(buffer, 0, bufferSize + 1);
	memcpy(buffer, beginPtr, bufferSize);

	UTF8 retString = UTF8(reinterpret_cast<const char*>(buffer));
	delete [] buffer;

	return retString;
}

int32_t UTF8::find(const char* s, uint32_t pos)
{
	uint32_t stringCount;
	uint32_t stringSize;

	getStreamLength(s, stringSize, stringCount);

	if( ( stringCount == 0 ) && ( s[0] != 0 ) )
	{
		LOG_ERROR( "corrupted string" );
		return npos;
	}

	if( pos + stringCount >= this->length())
		return npos;

	uint32_t position = 0;
	uint8_t* dataPtr = this->data;

	for( position = 0; position < pos; position++ )
	{
		dataPtr += charLength(reinterpret_cast<const char*>(dataPtr));
	}

	for(position = pos; position < this->length(); position++)
	{

		if( memcmp(dataPtr, s, stringSize) == 0 )
			return static_cast<int32_t>(position);
		dataPtr += charLength(reinterpret_cast<const char*>(dataPtr));
	}
	return npos;
}

int32_t UTF8::findLast(const char* s, uint32_t pos)
{
	uint32_t stringCount;
	uint32_t stringSize;

	getStreamLength(s, stringSize, stringCount);
	//check input substring
	if( ( stringCount == 0 ) && ( s[0] != 0 ) )
	{
		LOG_ERROR( "corrupted string" );
		return npos;
	}
	//check if pos is in range of source string
	if( pos > this->length())
		return npos;

	if(pos < stringCount - 1)
		return npos;

	uint32_t position = 0;
	uint8_t* dataPtr = this->data;
	int32_t lastFoundPosition = npos;

	//calculate position of last string to compare
	uint32_t positionEnd = pos - stringCount + 1;
	for(position = 0; position <= positionEnd; position++)
	{
		if( memcmp(dataPtr, s, stringSize) == 0 )
			lastFoundPosition = position;

		dataPtr += charLength(reinterpret_cast<const char*>(dataPtr));
	}
	return lastFoundPosition;
}

UTF8 UTF8::split(const uint32_t& idx)
{

	if( idx >= this->length() )
		return UTF8();

	uint8_t* dataPtr = this->data;

	//move data pointer to split index
	for( uint32_t i = 0; i < idx; i ++ )
	{
		dataPtr += charLength( reinterpret_cast<const char*>(dataPtr) );
	}
	//create new string
	UTF8 retString( reinterpret_cast<const char*>(dataPtr) );

	//re-create source string
	//create temp copy of string
	uint32_t tempStringSize = dataPtr - this->data;
	uint32_t tempStringBufferSize = getDataBufferSize(tempStringSize);
	uint8_t* tempString = new uint8_t [ tempStringBufferSize ];

	memset( tempString, 0, tempStringBufferSize);
	memcpy( tempString, this->data, tempStringSize);

	//add 1 to ensure string terminating zero
	this->sizeUsed = tempStringSize + 1;
	this->sizeAllocated = tempStringBufferSize;
	this->strLength = idx;

	//clear used memory
	memset(this->data, 0, this->sizeAllocated);
	delete [] this->data;

	this->data = tempString;

	this->lastIndex = 0;
	this->lastIndexData = this->data;

	return retString;
}

UTF8 UTF8::getLine(void)
{
	for(uint32_t i = 0; i < this->length(); i++)
	{
		uint32_t character = this->operator[](i);
		if( ( character == '\r' ) || ( character == '\n' ) )
			return this->substr(0, i);
	}
	return UTF8();
}

bool UTF8::removeChar(const uint32_t& pos, const uint32_t& count)
{

	if( ( pos + count >= this->length() ) || ( count == 0 ) )
		return false;

	//get pointer to begin of string to remove
	uint8_t* beginPtr = this->data;
	for(uint32_t i = 0; i < pos; i++)
	{
		beginPtr += charLength(reinterpret_cast<const char*>(beginPtr));
	}

	//get pointer to end of string to remove
	uint8_t* endPtr = beginPtr;
	for(uint32_t i = 0; i <  count; i++)
	{
		endPtr += charLength(reinterpret_cast<const char*>(endPtr));
	}

	uint32_t bytesToRemove = endPtr - beginPtr;
	uint32_t newStringSize = this->sizeUsed - bytesToRemove;

	uint32_t tempStringBufferSize = getDataBufferSize(newStringSize);
	uint8_t* tempString = new uint8_t [tempStringBufferSize];
	if( tempString == nullptr)
		return false;

	//create new data buffer
	uint32_t copyOffset = beginPtr - this->data;
	memset(tempString, 0, tempStringBufferSize);
	memcpy(tempString, this->data, beginPtr - this->data);
	memcpy(tempString + copyOffset, endPtr, this->sizeUsed - bytesToRemove);

	this->sizeAllocated = tempStringBufferSize;
	this->strLength -= count;
	this->sizeUsed -= bytesToRemove;

	//delete old data buffer
	memset(this->data, 0, this->sizeAllocated);
	delete [] this->data;

	//assign new data buffer
	this->data = tempString;

	return true;
}


uint32_t UTF8::getCharactersCount(  const char* stream ) {
	uint32_t size, count;
	if( getStreamLength( stream, size, count ) )
		return count;
	return 0;
}

uint8_t* UTF8::serialize( uint32_t& streamSize ) {

	streamSize = 0;
	//size of the buffer
	uint32_t size = sizeAllocated + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t);
	//buffer for storing data
	uint8_t* buf = (uint8_t*)malloc( size );

	if( buf == NULL ) {
		//LOG_FATAL("No memory to serialize string");
		return NULL;
	}
	memcpy( buf, &sizeAllocated, sizeof(uint32_t) );
	memcpy( buf + sizeof(uint32_t), &sizeUsed, sizeof(uint32_t) );
	memcpy( buf + sizeof(uint32_t) + sizeof(uint32_t), &strLength, sizeof(uint32_t) );
	memcpy( buf + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t), data, sizeAllocated );

	streamSize = size;

	return buf;
}

UTF8 UTF8::deserialize(uint8_t* stream) {

	if( stream == nullptr ) {
		//LOG_ERROR("null stream provided.");
		return UTF8();
	}
	uint32_t* sizeAllocated = reinterpret_cast<uint32_t*>(stream);
	uint32_t* sizeUsed = reinterpret_cast<uint32_t*>(stream + sizeof(uint32_t));
	uint32_t* strLength = reinterpret_cast<uint32_t*>(stream + 2*sizeof(uint32_t));
	uint8_t* data = stream + 3*sizeof(uint32_t);

	//sanity check
	if( ( (*strLength) > (*sizeUsed)) ||
		((*sizeUsed) > (*sizeAllocated))) {
		//LOG_FATAL("Errors in stream detected.");
		return UTF8();
	}
	return UTF8( data, *sizeAllocated, *sizeUsed, *strLength );
}
