//============================================================================
// Name        : pure-gui.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <sys/types.h>

#include <gtkmm.h>
#include <gtkmm/application.h>



#include "RWindow.hpp"
using namespace std;

static const int FrameBufferWidth = 480;
static const int FrameBufferHeight = 600;
static const int FrameBufferSize = FrameBufferWidth*FrameBufferHeight;

typedef struct {
	uint32_t frameCount;
	uint32_t width;
	uint32_t height;
}shared_memory;

uint8_t* createSHMBuffer( std::string name )
{
	shared_memory *shared_mem_ptr;
	int fd_shm, fd_log;
	bool initNeeded = true;

	//check if shared memory blok is already created
	if ((fd_shm = shm_open (name.c_str(), O_RDWR | O_CREAT , 0660)) == -1) {
		std::cerr<<"shm is already created"<<std::endl;
		initNeeded = false;
	}
	else {
		std::cout<<"shm created"<<std::endl;
		if (ftruncate (fd_shm, sizeof (shared_memory)+FrameBufferSize) == -1) {
			std::cerr<<"shm is already created"<<std::endl;
		}
	}
	if ((shared_mem_ptr = static_cast<shared_memory*>(mmap (NULL, sizeof (shared_memory)+FrameBufferSize, PROT_READ | PROT_WRITE, MAP_SHARED,
			fd_shm, 0))) == MAP_FAILED) {
		std::cerr<<"mmap failed"<<std::endl;
	}

	return reinterpret_cast<uint8_t*>(shared_mem_ptr);
}

int createFIFO( void )
{
	int fd;
	const char * myfifo = "/tmp/myfifo3";
	fd = open(myfifo,O_WRONLY | O_NONBLOCK);

	if(fd < 0)
	{
		mkfifo(myfifo, 0666);
		fd = open(myfifo,O_WRONLY);
	}

	int error = errno;
	perror("mkfifo");
//	ENOENT

	return fd;
}

int createBattFifo( void )
{
	int fd;
	const char * myfifo = "/tmp/fifoBattKeys";
	fd = open(myfifo,O_WRONLY | O_NONBLOCK);

	if(fd < 0)
	{
		mkfifo(myfifo, 0666);
		fd = open(myfifo,O_WRONLY);
	}

	int error = errno;
	perror("mkfifo");
//	ENOENT

	return fd;
}
int main( int argc, char* argv[] ) {



	std::string shnName = "pure_gui_fmbuf";

	shared_memory* shm_ptr = reinterpret_cast<shared_memory*>(createSHMBuffer(shnName));
	char* dataMemory = reinterpret_cast<char*>(shm_ptr)+sizeof(shared_memory);

	int fifoFd = createFIFO();
	int fifoBattfd = createBattFifo();

	auto app = Gtk::Application::create(argc, argv, "org.gtkmm.example");

	RWindow rendererWindow( dataMemory, fifoFd, fifoBattfd, FrameBufferWidth, FrameBufferHeight );
	app->run( rendererWindow );


//    shm_unlink(const char *name);
	return 0;
}
