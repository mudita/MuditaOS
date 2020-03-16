#pragma once
#include <string>

struct Device
{
  public:
    Device(std::string name = "") : name(name)
    {}
    virtual ~Device(){};
    std::string name;
};
