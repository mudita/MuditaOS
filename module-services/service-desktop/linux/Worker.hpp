#pragma once
#include "../WorkerDesktop.hpp"

int desktopServiceInit();
void desktopServiceReceive(void *pointer);
int desktopServiceSend(std::string msg);
