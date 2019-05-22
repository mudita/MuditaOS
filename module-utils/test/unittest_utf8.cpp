/*
 * unittest_utf8.cpp
 *
 *  Created on: 29 kwi 2019
 *      Author: robert
 */

#include <iostream>
#include <string>
#include <cstring>

using namespace std;

#include "utf8/UTF8.hpp"

const char* strings[] = {
	"TEST",
	"Rąbać",
	"I want to have pure phone",
	"Мой адрес: улица Тверская, дом пять, квартира семнадцать"
};

const char* strings2[] = {
	"TEST\r",
	"Rąbać\n",
	"I want to have \rpure phone",
	"Мой адрес: улица Тверская, дом пять,\n квартира семнадцать"
};

void test_raw_char_constructor( const char* str ) {
	UTF8 ustr = UTF8{ str };
	cout<< "    "<< ustr.c_str()<< " len: " << ustr.length() << " used bytes: " << ustr.used() << endl;
}

void test_string_constructor( const std::string& str ) {
	UTF8 ustr = UTF8{ str };
	cout<< "    "<<ustr.c_str()<< " len: " << ustr.length() << " used bytes: " << ustr.used() << endl;
}

void test_copy_constructor( const std::string& str ) {
	UTF8 bstr = UTF8{ str };
	UTF8 ustr = UTF8{ bstr };
	cout<< "    "<< ustr.c_str()<< " len: " << ustr.length() << " used bytes: " << ustr.used() << endl;
}

void test_operator_index( const std::string& str ){
	UTF8 ustr = UTF8{ str };
	char s[4] = {0,0,0,0};

	cout << "    ";
	for(uint32_t i = 0; i < ustr.length(); i++)
		{
			uint32_t value = ustr[i];
			memcpy(s, &value, 4);
			cout << s;
		}
	cout << endl;
}

void test_split(const std::string& str )
{
	UTF8 sourceString = UTF8{str};

	uint32_t divisionIdx = sourceString.length() / 2;

	cout << "    " << "Before split." << endl;
	cout << "        " << "srcString:    [" << sourceString.c_str() << "]    len: " << sourceString.length() << " used bytes: " << sourceString.used() << endl;
	char s[4] = {0,0,0,0};
	uint32_t value = sourceString[ divisionIdx ];
	memcpy(s, &value , 4);
	cout << "        " << "division character: [" << s << "] division index: " << divisionIdx << endl;

	UTF8 splitString = sourceString.split(divisionIdx);

	cout << "    " << "After split." << endl;
	cout << "        " << "srcString:    [" << sourceString.c_str() << "]    len: " << sourceString.length() << " used bytes: " << sourceString.used() << endl;
	cout << "        " << "sptString:    [" << splitString.c_str() << "]    len: " << splitString.length() << " used bytes: " << splitString.used() << endl;
	cout << "        " << "summary len: " << splitString.length() + sourceString.length()
	    			   << " summary used bytes : " << splitString.used() + sourceString.used()<< endl;
	cout << endl;
}
void test_getLine( const std::string& str ){
	UTF8 ustr = UTF8{ str };

	UTF8 dstr = ustr.getLine();

	cout << "    " << dstr.c_str() << endl;
}

void test_removeChar( const std::string& str, uint32_t i ){
	UTF8 ustr(str);

	cout<< "    Source: " << endl;
	cout<< "    " << ustr.c_str()<< " len: " << ustr.length() << " used bytes: " << ustr.used() << endl;
	//remove 'ś'
	bool retVal = ustr.removeChar(i, i*2);
	cout<< "    After remove: " << " remove pos: " << i << " remove count: " << i*2 << endl;
	cout<< "    " << ustr.c_str()<< " len: " << ustr.length() << " used bytes: " << ustr.used() << " returned value: " << retVal << endl;

}

int main() {

	cout<<"RUNNING UNIT TEST FOR UTF8"<<endl;

	cout<< "UTF8( const char* str )"<<endl;
	for( uint32_t i=0; i<sizeof(strings)/sizeof(char*); ++i )
		test_raw_char_constructor( strings[i] );
	cout<<endl;

	cout<<"UTF8( const std::string& str )"<<endl;
	for( uint32_t i=0; i<sizeof(strings)/sizeof(char*); ++i )
		test_string_constructor( std::string{strings[i]} );
	cout<<endl;

	//copy constructor
	cout<<"UTF8( const UTF8& utf )"<<endl;
	for( uint32_t i=0; i<sizeof(strings)/sizeof(char*); ++i ){
		test_copy_constructor( std::string{strings[i]} );
	}
	cout<<endl;

	cout<<"uint32_t operator[]( const uint32_t& idx ) "<<endl;
	for( uint32_t i=0; i<sizeof(strings)/sizeof(char*); ++i ){
		test_operator_index( std::string{strings[i]} );
	}
	cout<<endl;

	cout<<"UTF8 split( uint32_t idx ) "<<endl;
	for( uint32_t i=0; i<sizeof(strings)/sizeof(char*); ++i ){
		test_split( std::string{strings[i]} );
	}
	cout<<"UTF8 getLine( void ) "<<endl;
	for( uint32_t i=0; i<sizeof(strings2)/sizeof(char*); ++i ){
		test_getLine( std::string{strings2[i]} );
	}
	cout<<endl;
	cout<<"bool removeChar(uint32_t pos, uint32_t count) "<<endl;
	for( uint32_t i=0; i<sizeof(strings)/sizeof(char*); ++i ){
		test_removeChar( std::string{strings[i]}, i);
	}
	cout<<endl;

	UTF8 sourceString = UTF8("Rąbać drewno siekierą");
	UTF8 destinationString = sourceString.substr(0, 5);

	cout << "UTF8 : " << '[' << sourceString.c_str() << ']'<< endl;
	cout << "UTF8 substr: " << '[' << destinationString.c_str() << ']'<< endl;

	cout << "" << endl; // prints
	return 0;
}

