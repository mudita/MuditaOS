#pragma once
#include "../ServiceDesktop.hpp"

int desktopServiceInit();
void desktopServiceReceive(void *ptr);
void desktopServiceSend(void *ptr);
