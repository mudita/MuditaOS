#pragma once
#include "../ServiceDesktop.hpp"

int desktop_init();
void desktop_receive(void *ptr);
int desktop_send(int fd, uint8_t *data, size_t dataSize);
