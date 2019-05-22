/*
 * UTF8.hpp
 *
 *  Created on: 9 kwi 2019
 *      Author: robert
 */

#ifndef UTF8_HPP_
#define UTF8_HPP_

#include <string>
#include <cstdint>



class UTF8 {
protected:
	UTF8( const uint8_t* data, const uint32_t allocated, const uint32_t used, const uint32_t len );

	//pointer to buffer
	uint8_t* data;
	//total size of buffer in bytes
	uint32_t sizeAllocated;
	//number of bytes used in buffer
	uint32_t sizeUsed;
	//umber of characters in the string
	uint32_t strLength;
	//last used index
	mutable uint32_t lastIndex;
	//pointer to last indexed character
	mutable uint8_t* lastIndexData;

	//variable used when c_str() is called for a string that has no data yet
	static const char* emptyString;
	//holds number of bytes by which buffer will be expanded in case when current buffer can't hold new data.
	static const uint32_t stringExpansion;
	/**
	 * @brief Calculates size of the buffer to store given number of data bytes.
	 * @param dataBytes number of data bytes
	 * @return Number of bytes needed to store provided data bytes size rounded up and rounded using stringExpansion value.
	 */
	uint32_t getDataBufferSize( uint32_t dataBytes );
	bool expand( uint32_t size = stringExpansion );
public:
	UTF8();
	UTF8( const char* str );
	UTF8( const std::string& str );
	UTF8( const UTF8& utf );
	static const int32_t npos = -1;

	virtual ~UTF8();

	/**
	 * OPERATORS
	 */

	UTF8& operator=( const UTF8& utf );
	UTF8& operator= ( UTF8&& utf ) noexcept;
	uint32_t operator[]( const uint32_t& idx ) const;
	UTF8 operator+( const UTF8& utf );
	UTF8& operator+=( const UTF8& utf );
	bool operator==( const UTF8& utf );

	/**
	 * UTILITY FUNCTIONS
	 */

	uint32_t length() const { return strLength; };
	uint32_t used() { return sizeUsed; };
	const char* c_str() const ;

	/**
	*@brief Creates substring from current string. New string starts from begin parameter and contains number of characters passed by length.
	*@param begin Index of the first character in newly created string.
	*@param length Number of characters to copy.
	*@return substring created from source string.
	*@note In case of start index greater than length of source string or length that exceeds character empty string is returned.
	**/
	UTF8 substr( const uint32_t begin, const uint32_t length );

	/**
	 * @brief Finds first occurrence of substring in string
	 * @param s string to find
	 * @param pos initial searching position
	 * @return index of first matched string
	 * @note returns npos when substring is not found
	 */

	int32_t find(const char* s, uint32_t pos = 0);
	/**
	 * @brief Finds last occurrence of substring in string
	 * @param s string to find
	 * @param pos initial searching position
	 * @return index of first matched string
	 * @note returns npos when substring is not found.
	 */
	int32_t findLast(const char* s, uint32_t pos);
	/**
	 * @brief splits UTF8 sting into two strings.
	 * @param idx index of character from which the division will be made.
	 * @return newly created string, character under specified index will be first character in new string.
	 * Returns empty string in case of invalid index.
	 */
	UTF8 split(const uint32_t& idx);
	/**
	 * @brief Creates substring from current string. New string is limited by /r or /n.
	 * @return subrstring created from curret string. Returns empty string in case of failure.
	 */
	UTF8 getLine(void);
	/**
	 * @brief Remove characters from string.
	 * @param pos position of first char to remove.
	 * @param count count of characters to remove.
	 * @return true if there was no error, false otherwise
	 */
	bool removeChar( const uint32_t& pos = 0, const uint32_t& count = 1);

	/*
	 * @brief Check if string has only ASCII characters
	 * @return true if there are only ASCII characters in string, false otherwise.
	 */
	bool isAscii(void)
	{
		if(this->sizeUsed - 1 == this->length())
			return true;
		return false;
	}
	/**
	 * PUBLIC METHODS
	 */

	/**
	 * @brief Serialize object to the stream of bytes.
	 * @param idx index of character from which the division will be made.
	 * @return newly created string, character under specified index will be first character in new string.
	 * Returns empty string in case of invalid index.
	 */

	uint8_t* serialize( uint32_t& streamSize );

	/**
	 * STATIC METHODS
	 */

	/**
	 * @brief Calculates numer of utf8 characters in provided stream
	 */
	static uint32_t getCharactersCount( const char* stream );
	/**
	 * @brief Calculates number of bytes and character IDs in the provided stream;
	 * @param size Variable where number of bytes in the stream will be saved( till null terminator );
	 * @param count Variable where number of characters in the stream will be saved.
	 * @return true if there was no error, false otherwise;
	 */
	static bool getStreamLength( const char* stream, uint32_t& size, uint32_t& count );
	static UTF8 deserialize( uint8_t* stream );
#ifdef BUILD_GTEST_TESTS
	mutable uint32_t operator_index_iterations;
	#endif
};

#endif /* UTF8_HPP_ */
