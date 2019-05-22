#pragma once

#include <memory>


namespace sys
{


	class Service;

class Channel
{

public:
	Channel()
	{
	}


	std::vector<std::shared_ptr<Service>> m_services;
};


}
