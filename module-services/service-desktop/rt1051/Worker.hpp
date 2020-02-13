#pragma once
#include "../WorkerDesktop.hpp"

int desktopServiceInit();
void desktopServiceReceive(void *ptr);
int desktopServiceSend(std::string msg);
