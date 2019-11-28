/*
 * UCS2.hpp
 *
 *  Created on: 24 wrz 2019
 *      Author: kuba
 */

#ifndef MODULE_UTILS_UCS2_UCS2_HPP_
#define MODULE_UTILS_UCS2_UCS2_HPP_

#include <cstdint>
#include "utf8/UTF8.hpp"


class UCS2{
private:
    //pointer to memory where ucs2 characters are stored.
    uint16_t* buffer;
    //size in bytes of memory that was allcated to the buffer
	uint32_t sizeAllocated;
	//size in bytes of memory used in buffer
	uint32_t sizeUsed;
    //number of characters in the string. its equal to size of allocated memory plus null terminator
    uint32_t length;
    //
    static const uint32_t ucs2bufferExt;
public:
    //default constructor
    UCS2(void);
    //
    UCS2(const uint16_t* text);
    /*
     * @brief Initializes new ucs2 string from utf8 string. It's used to convert text from
     * utf8 to ucs2.
     * @param string utf8 string to convert
     */
    UCS2(const UTF8& string);
    /*
	 * @brief Initializes new ucs2 string from std::string. It's used to convert text from
	 * modem message format to ucs2.
	 * @param string std::string to convert
	 */
    UCS2(const std::string& string);
    UCS2(UCS2& ucs);
    ~UCS2(void);
    const char* c_str(void){ return reinterpret_cast<const char*>(buffer);}
    /*
     * @brief It's converting ucs2 to utf string.
     * @return utf8 string
     */
    UTF8 toUTF8(void);
    void append(const uint16_t& ucs2char);
    /*
	 * @brief It's converting text coded in ucs2 to string. Used to send data to modem.
	 * @return coded string
	 */
    std::string modemStr(void);
    uint32_t getLength(void) { return length; };
    uint32_t getSizeUsed(void) { return sizeUsed; };
	uint32_t getSizeAlocated(void) { return sizeAllocated; };
	uint16_t* getData(void) {return buffer; };
};



#endif /* MODULE_UTILS_UCS2_UCS2_HPP_ */
