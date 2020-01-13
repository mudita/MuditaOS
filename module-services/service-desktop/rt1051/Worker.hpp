#pragma once
#include "../ServiceDesktop.hpp"

int desktopServiceInit();
void desktopServiceReceive(void *ptr);
int desktopServiceSend(int fd, uint8_t *data, size_t dataSize);
