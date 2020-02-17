#pragma once
#include "../WorkerDesktop.hpp"

namespace bsp
{
    int desktopServiceInit();
    void desktopServiceReceive(void *ptr);
    int desktopServiceSend(std::string *msg);
} // namespace bsp
