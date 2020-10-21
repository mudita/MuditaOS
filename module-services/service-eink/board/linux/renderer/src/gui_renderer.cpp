// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

//============================================================================
// Name        : pure-gui.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "RWindow.hpp" // for RWindow

#include <gtkmm/application.h> // for Application
#include <sys/mman.h>          // for mmap, shm_open, MAP_FAILED, MAP_SHARED
#include <sys/stat.h>          // for mkfifo
#include <errno.h>             // for errno
#include <fcntl.h>             // for open, O_WRONLY, O_NONBLOCK, O_CREAT
#include <stdint.h>            // for uint32_t, uint8_t
#include <unistd.h>            // for ftruncate

#include "glibmm/refptr.h" // for RefPtr
#include <cstring>         // for strerror, NULL
#include <iostream>        // for operator<<, basic_ostream, endl, char...
#include <string>          // for string, allocator

using namespace std;

static const int FrameBufferWidth  = 480;
static const int FrameBufferHeight = 600;
static const int FrameBufferSize   = FrameBufferWidth * FrameBufferHeight;

typedef struct
{
    uint32_t frameCount;
    uint32_t width;
    uint32_t height;
} shared_memory;

uint8_t *createSHMBuffer(std::string name)
{
    shared_memory *shared_mem_ptr;
    int fd_shm;

    // check if shared memory blok is already created
    if ((fd_shm = shm_open(name.c_str(), O_RDWR | O_CREAT, 0660)) == -1) {
        std::cerr << "shm is already created" << std::endl;
    }
    else {
        std::cout << "shm created" << std::endl;
        if (ftruncate(fd_shm, sizeof(shared_memory) + FrameBufferSize) == -1) {
            std::cerr << "shm is already created" << std::endl;
        }
    }
    if ((shared_mem_ptr = static_cast<shared_memory *>(
             mmap(NULL, sizeof(shared_memory) + FrameBufferSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0))) ==
        MAP_FAILED) {
        std::cerr << "mmap failed" << std::endl;
    }

    return reinterpret_cast<uint8_t *>(shared_mem_ptr);
}

int createFIFO(void)
{
    int fd;
    const char *myfifo = "/tmp/myfifo3";
    fd                 = open(myfifo, O_WRONLY | O_NONBLOCK);

    if (fd < 0) {
        mkfifo(myfifo, 0666);
        fd = open(myfifo, O_WRONLY);
    }

    std::cerr << "mkfifo " << strerror(errno) << " " << myfifo << std::endl;
    //	ENOENT

    return fd;
}

int createBattFifo(void)
{
    int fd;
    const char *myfifo = "/tmp/fifoBattKeys";
    fd                 = open(myfifo, O_WRONLY | O_NONBLOCK);

    if (fd < 0) {
        mkfifo(myfifo, 0666);
        fd = open(myfifo, O_WRONLY);
    }

    std::cerr << "mkfifo " << strerror(errno) << " " << myfifo << std::endl;
    //	ENOENT

    return fd;
}
int main(int argc, char *argv[])
{

    std::string shnName = "pure_gui_fmbuf";

    shared_memory *shm_ptr = reinterpret_cast<shared_memory *>(createSHMBuffer(shnName));
    char *dataMemory       = reinterpret_cast<char *>(shm_ptr) + sizeof(shared_memory);

    int fifoFd     = createFIFO();
    int fifoBattfd = createBattFifo();

    auto app = Gtk::Application::create(argc, argv, "org.gtkmm.example");

    RWindow rendererWindow(dataMemory, fifoFd, fifoBattfd, FrameBufferWidth, FrameBufferHeight);
    app->run(rendererWindow);

    //    shm_unlink(const char *name);
    return 0;
}
