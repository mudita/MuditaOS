// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

// This is minimalistic example how our GUI renderer works - this code won't compile right now
// there shouldn't be much work to do to make it compile again, there is no target like that in need so it's not added
// this could be potentially used instead running whole PurePhone simulator to test/create UI only code

#include <gui/widgets/Window.hpp>
#include <gui/core/Context.hpp>
#include <gui/core/Renderer.hpp>
#include <gui/core/DrawCommand.hpp>

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

void drawWindow(auto *window, auto *frameBuffer)
{
    // create renderer object
    gui::Renderer renderer;
    // create canvas
    gui::Context *context = new gui::Context(static_cast<unsigned short>(480), static_cast<unsigned short>(600));
    context->fill(15);

    /// generate draw commands from window
    std::list<gui::DrawCommand *> commandsList = win->buildDrawList();
    std::vector<gui::DrawCommand *> commands{commandsList.begin(), commandsList.end()};

    // render commands
    renderer.render(context, commands);

    // copy context to shared memory (our framebuffer)
    memcpy(frameBuffer, context->getData(), FrameBufferSize);

    // cleanup context
    delete context;
}

int main(int argc, char **argv)
{
    gui::Window *win = new gui::AppWindow("MAIN");
    win->setSize(480, 600);
    win->buildInterface();
    win->setTitle("LOL");
    drawWindow(win, createSHMBuffer());
}
