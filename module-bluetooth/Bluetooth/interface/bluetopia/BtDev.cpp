#include "BtDev.hpp"
#include <cstring>

extern "C" {
#include <HCITRANS.h>
};

BtDev::BtDev()
{
    val = new BD_ADDR_t;
    memset(val, 0, sizeof *val);
}

BtDev::~BtDev()
{
}

/// might as well return rvalue reference
const std::string BtDev::tostr()
{
    char tmp[15]={0};
    sprintf(tmp, "0x%02X%02X%02X%02X%02X%02X", val->BD_ADDR5, val->BD_ADDR4, val->BD_ADDR3, val->BD_ADDR2, val->BD_ADDR1, val->BD_ADDR0);
    return std::string(tmp);
}

BD_ADDR_t BtDev::fromstr(char* addr)
{
    char *tstr=addr;
    memset(val,0,sizeof *val);
    if(tstr[0] == '0' && tstr[1] == 'x') tstr+=2;
    for(int i=0;i<12;++i) {
        int pos=(12-i-1)/2;
        char *t=(char*)&(val->BD_ADDR0)+pos;
        char cval = tstr[i]-(tstr[i]>='A'?'A'-10:'0');
        cval=i%2?cval:cval*16;
        *t+=cval;
    }
    return *val;
}

BtDev& BtDev::operator=(BD_ADDR_t addr)
{
    val->BD_ADDR0 = addr.BD_ADDR0;
    val->BD_ADDR1 = addr.BD_ADDR1;
    val->BD_ADDR2 = addr.BD_ADDR2;
    val->BD_ADDR3 = addr.BD_ADDR3;
    val->BD_ADDR4 = addr.BD_ADDR4;
    val->BD_ADDR5 = addr.BD_ADDR5;
    return *this;
}
