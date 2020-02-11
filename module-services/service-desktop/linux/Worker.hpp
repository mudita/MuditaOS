#pragma once
#include "../ServiceDesktop.hpp"

int desktopServiceInit();
void desktopServiceReceive(void *pointer);
void desktopServiceSend(void *pointer);
