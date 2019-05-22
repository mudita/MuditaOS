/*
 * unittest_gui.cpp
 *
 *  Created on: 29 kwi 2019
 *      Author: robert
 */
#include <memory>
#include <functional>
#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include "log/log.hpp"
#include "utf8/UTF8.hpp"

#include "gui/core/Font.hpp"
#include "gui/core/BoundingBox.hpp"
#include "gui/core/Context.hpp"
#include "gui/core/Renderer.hpp"
#include "gui/core/DrawCommand.hpp"
#include "gui/core/Font.hpp"
#include "gui/core/PixMapManager.hpp"

#include "gui/widgets/Window.hpp"
#include "gui/widgets/Item.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/BoxLayout.hpp"
#include "gui/widgets/Image.hpp"

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
	int fd_shm;

	//check if shared memory blok is already created
	if ((fd_shm = shm_open (name.c_str(), O_RDWR | O_CREAT , 0660)) == -1) {
		std::cerr<<"shm is already created"<<std::endl;
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

using namespace std;

bool bouindingBoxTest() {

	gui::BoundingBox result;
	if( gui::BoundingBox::intersect( gui::BoundingBox(0,0,15,15), gui::BoundingBox(15,0,15,15), result))
		return false;
	if( gui::BoundingBox::intersect( gui::BoundingBox(0,0,15,15), gui::BoundingBox( 0,15,15,15), result))
		return false;
	if( gui::BoundingBox::intersect( gui::BoundingBox(0,0,15,15), gui::BoundingBox(-15,0,15,15), result))
		return false;
	if( gui::BoundingBox::intersect( gui::BoundingBox(0,0,15,15), gui::BoundingBox( 0, -15,15,15), result))
		return false;
	if( gui::BoundingBox::intersect( gui::BoundingBox(0,0,15,15), gui::BoundingBox(14,0,15,15), result) == false )
		return false;

	return true;
}

bool getContextBoxTest() {

	gui::Context* ctx = new gui::Context( static_cast<unsigned short>(30), static_cast<unsigned short>(30) );
	ctx->fill( 0 );

	gui::Context* test = ctx->get( 17,17, 30,30 );

	delete test;
	test = ctx->get( -17,-17, 30,30 );
	delete test;

	delete ctx;

	return true;
}

bool insertContextTest() {

	gui::Context* dstCtx;
	gui::Context* insCtx;

	LOG_INFO("RECTANGLE INSIDE");
	dstCtx = new gui::Context( static_cast<unsigned short>(30), static_cast<unsigned short>(30) );
	dstCtx->fill( 0 );
	insCtx = new gui::Context( static_cast<unsigned short>(28), static_cast<unsigned short>(28) );
	insCtx->fill( 15 );
	dstCtx->insert(1,1, insCtx );
	delete dstCtx;
	delete insCtx;

	LOG_INFO("2 COLUMNS ON RIGHT SIDE, TOP AND BOTTOM ROW UNTOUCHED");
	dstCtx = new gui::Context( static_cast<unsigned short>(30), static_cast<unsigned short>(30) );
	dstCtx->fill( 0 );
	insCtx = new gui::Context( static_cast<unsigned short>(28), static_cast<unsigned short>(28) );
	insCtx->fill( 15 );
	dstCtx->insert(28,1, insCtx );
	delete dstCtx;
	delete insCtx;

	LOG_INFO("2 COLUMNS ON LEFT SIDE, TOP AND BOTTOM ROW UNTOUCHED");
	dstCtx = new gui::Context( static_cast<unsigned short>(30), static_cast<unsigned short>(30) );
	dstCtx->fill( 0 );
	insCtx = new gui::Context( static_cast<unsigned short>(28), static_cast<unsigned short>(28) );
	insCtx->fill( 15 );
	dstCtx->insert(-26,1, insCtx );
	delete dstCtx;
	delete insCtx;

	LOG_INFO("2 COLUMNS ON RIGHT SIDE");
	dstCtx = new gui::Context( static_cast<unsigned short>(30), static_cast<unsigned short>(30) );
	dstCtx->fill( 0 );
	insCtx = new gui::Context( static_cast<unsigned short>(32), static_cast<unsigned short>(32) );
	insCtx->fill( 15 );
	dstCtx->insert(28,-1, insCtx );
	delete dstCtx;
	delete insCtx;

	LOG_INFO("2 COLUMNS ON LEFT SIDE");
	dstCtx = new gui::Context( static_cast<unsigned short>(30), static_cast<unsigned short>(30) );
	dstCtx->fill( 0 );
	insCtx = new gui::Context( static_cast<unsigned short>(32), static_cast<unsigned short>(32) );
	insCtx->fill( 15 );
	dstCtx->insert(-30,-1, insCtx );
	delete dstCtx;
	delete insCtx;

	return true;
}

bool drawRectangleTest( uint8_t* frameBuffer ) {

	//context for drawing commands
	gui::Context* context = new gui::Context( static_cast<unsigned short>(480), static_cast<unsigned short>(600) );
	context->fill( 15 );

	//renderer class that will perform drawing
	gui::Renderer renderer;

	//vector with draw commands
	std::vector<gui::DrawCommand*> commands;

	//simple filled rectangle with no rounded corners
	gui::CommandRectangle* rectTop = new gui::CommandRectangle();
	gui::CommandRectangle* rectBottom = new gui::CommandRectangle();
	gui::CommandRectangle* rectLeft = new gui::CommandRectangle();
	gui::CommandRectangle* rectRight = new gui::CommandRectangle();
	gui::CommandRectangle* rectFull = new gui::CommandRectangle();

	rectTop->areaX = 5;	rectTop->areaY = 5;
	rectTop->areaW = 8;	rectTop->areaH = 8;
	rectTop->x = 5; rectTop->y = 5;
	rectTop->w = 8;	rectTop->h = 8;
	rectTop->edges = gui::RectangleEdgeFlags::GUI_RECT_EDGE_LEFT |
		gui::RectangleEdgeFlags::GUI_RECT_EDGE_RIGHT |
		gui::RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM;

	*rectBottom = *rectTop;
	rectBottom->areaX = 31;	rectBottom->areaY = 5;
	rectBottom->x = 31; rectBottom->y = 5;
	rectBottom->edges = gui::RectangleEdgeFlags::GUI_RECT_EDGE_LEFT |
		gui::RectangleEdgeFlags::GUI_RECT_EDGE_RIGHT |
		gui::RectangleEdgeFlags::GUI_RECT_EDGE_TOP;

	*rectLeft = *rectTop;
	rectLeft->areaX = 5;	rectLeft->areaY = 31;
	rectLeft->x = 5; rectLeft->y = 31;
	rectLeft->edges = gui::RectangleEdgeFlags::GUI_RECT_EDGE_TOP |
	gui::RectangleEdgeFlags::GUI_RECT_EDGE_RIGHT |
	gui::RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM;

	*rectRight = *rectTop;
	rectRight->areaX = 31;	rectRight->areaY = 31;
	rectRight->x = 31; rectRight->y = 31;
	rectRight->edges = gui::RectangleEdgeFlags::GUI_RECT_EDGE_TOP |
	gui::RectangleEdgeFlags::GUI_RECT_EDGE_LEFT |
	gui::RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM;

	*rectFull = *rectTop;
	rectFull->areaX = 18;	rectFull->areaY = 18;
	rectFull->x = 18; rectFull->y = 18;
	rectFull->edges = gui::RectangleEdgeFlags::GUI_RECT_ALL_EDGES;

	commands.push_back( rectTop );
	commands.push_back( rectBottom );
	commands.push_back( rectLeft );
	commands.push_back( rectRight );
	commands.push_back( rectFull );

	gui::CommandRectangle* rect = new gui::CommandRectangle();
	rect->areaX = 5;	rect->areaY = 50;
	rect->areaW = 50;	rect->areaH = 50;
	rect->x = 5; rect->y = 50;
	rect->w = 50;	rect->h = 50;
	rect->borderColor = gui::Color( 12, 15 );
	rect->fillColor = rect->borderColor;
	rect->filled = true;
	rect->edges = gui::RectangleEdgeFlags::GUI_RECT_ALL_EDGES;

	commands.push_back( rect );

	gui::CommandRectangle* rectRoundTR = new gui::CommandRectangle();
	rectRoundTR->areaX = 5;	rectRoundTR->areaY = 150;
	rectRoundTR->areaW = 150;	rectRoundTR->areaH = 150;
	rectRoundTR->x = 5; rectRoundTR->y = 150;
	rectRoundTR->w = 150;	rectRoundTR->h = 150;
	rectRoundTR->edges = gui::RectangleEdgeFlags::GUI_RECT_ALL_EDGES;
	rectRoundTR->radius = 25;
	rectRoundTR->penWidth = 5;

	commands.push_back( rectRoundTR );

	//render commands
	renderer.render( context, commands );

	//copy context to shared memory
	memcpy( frameBuffer, context->getData(), FrameBufferSize );

	//cleanup
	for( auto cmd : commands )
		delete cmd;

	delete context;

	return true;
}

bool drawWindowWithLabelsTest( uint8_t* frameBuffer ) {

	gui::Context* context = new gui::Context( static_cast<unsigned short>(480), static_cast<unsigned short>(600) );
	context->fill( 15 );

	//renderer class that will perform drawing
	gui::Renderer renderer;

	gui::Window* win = new gui::Window(0);
	win->setSize( 480, 600 );

	//add label with time
	gui::Label* label = new gui::Label(win, 20, 80, 440, 120, "TOP LEFT");
	label->setAlignement(gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_LEFT, gui::Alignment::ALIGN_VERTICAL_TOP));
	label->setRadius(5);
	label->setFont( "gt_pressura_bold_65" );

	label = new gui::Label(win, 20, 280, 440, 120, "2xCENTER#####");
	label->setAlignement(gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));
	label->setRadius(10);
	label->setPenWidth( 5 );
	label->setFont( "gt_pressura_bold_65" );

	label = new gui::Label(win, 20, 480, 440, 120, "BOTTOM RIGHT");
	label->setAlignement(gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_RIGHT, gui::Alignment::ALIGN_VERTICAL_BOTTOM));
	label->setRadius(15);
	label->setDotsMode(true);
	label->setFont( "gt_pressura_bold_65" );

	//vector with draw commands

	//context for drawing commands
	std::list<gui::DrawCommand*> commandsList = win->buildDrawList();
	std::vector<gui::DrawCommand*> commands { commandsList.begin(), commandsList.end()};

	delete win;

	//render commands
	renderer.render( context, commands );

	//copy context to shared memory
	memcpy( frameBuffer, context->getData(), FrameBufferSize );

	//cleanup
	for( auto cmd : commands )
		delete cmd;

	delete context;
	return true;
}

bool drawWindowWithHBoxTest( uint8_t* frameBuffer ) {

	gui::Context* context = new gui::Context( static_cast<unsigned short>(480), static_cast<unsigned short>(600) );
	context->fill( 15 );

	//renderer class that will perform drawing
	gui::Renderer renderer;

	gui::Window* win = new gui::Window(0);
	win->setSize( 480, 600 );

	gui::HBox* hBox = new gui::HBox( win, 50, 50, 380, 500 );

	gui::Rect* maxW1 = new gui::Rect();
	maxW1->setFillColor(gui::Color( 5, 0));
	maxW1->setFilled(true);
	maxW1->setMaxSize( 50, 300 );

	gui::Label* maxW4 = new gui::Label();
	maxW4->setText("Top Left corner");
	maxW4->setDotsMode(true);
	//maxW4->setRadius( 20 );
	//maxW4->setMargins( gui::Margins(20,20,20,20));
	maxW4->setMaxSize( 275, 60 );

	gui::Rect* maxW2 = new gui::Rect();
	maxW2->setFillColor(gui::Color( 8, 0));
	maxW2->setFilled(true);
	maxW2->setMaxSize( 35, 300 );

	gui::Rect* maxW3 = new gui::Rect();
	maxW3->setFillColor(gui::Color( 11, 0));
	maxW3->setFilled(true);
	maxW3->setMaxSize( 30, 300 );

	hBox->addWidget(maxW1);
	hBox->addWidget(maxW4);
	hBox->addWidget(maxW2);

	gui::VBox* vBox = new gui::VBox( hBox, 10, 155, 460, 600-160 );

	gui::Rect* maxH1 = new gui::Rect();
	maxH1->setMaxSize( 10, 80 );

	gui::Rect* maxH2 = new gui::Rect();
	maxH2->setMaxSize( 15, 300 );

	gui::Rect* maxH3 = new gui::Rect();
	maxH3->setMaxSize( 30, 300 );

	gui::Label* maxH4 = new gui::Label();
	maxH4->setText("Hello Mudita");
	maxH4->setRadius( 20 );
	maxH4->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));
	maxH4->setMaxSize( 75, 60 );

	gui::Image* img1 = new gui::Image();
	uint16_t id = gui::PixMapManager::getInstance().getPixMapID("loudspeaker.mpi");
	img1->setImageWithID( id );


	vBox->addWidget(maxH1);
	vBox->addWidget(maxH2);
	vBox->addWidget(maxH4);
	vBox->addWidget( img1 );
	vBox->addWidget(maxH3);

	hBox->addWidget(maxW3);

	//context for drawing commands
	std::list<gui::DrawCommand*> commandsList = win->buildDrawList();
	std::vector<gui::DrawCommand*> commands { commandsList.begin(), commandsList.end()};

	delete win;

	//render commands
	renderer.render( context, commands );

	//copy context to shared memory
	memcpy( frameBuffer, context->getData(), FrameBufferSize );

	//cleanup
	for( auto cmd : commands )
		delete cmd;

	delete context;
	return true;
}

int main( int argc, char* argv[] ) {

	LOG_INFO("RUNNING UNIT TEST FOR GUI");

	char cwd[256] = {0};
	if (getcwd(cwd, sizeof(cwd)) != NULL) {
		LOG_INFO("Current working directory: %s", cwd);
	} else {
		LOG_ERROR("getcwd() error");
	}

	string basePath = cwd;
	if( argc != 2 ) {
		LOG_INFO("No working directory provided. Using current one.");
	}
	else {
		basePath = argv[1];
	}

	chdir(basePath.c_str());
	string assetsPath = basePath+"/assets";
	gui::FontManager::getInstance().init( assetsPath );
	assetsPath = basePath+"/assets";
	gui::PixMapManager::getInstance().init( assetsPath );

	//create and map shared memory
	std::string shmName = "pure_gui_fmbuf";
	uint8_t* shmMemPtr = createSHMBuffer( shmName.c_str() ) + sizeof(shared_memory);

	//BoundingBox tests
	if (!bouindingBoxTest())
		LOG_ERROR("BOUNDING BOX TEST: failed");
	else
		LOG_INFO("BOUNDING BOX TEST: pass");

	//getContextBox tests
	if (!getContextBoxTest())
		LOG_ERROR("GET CONTEXT TEST: failed");
	else
		LOG_INFO("GET CONTEXT TEST: pass");

	//insertContext tests
	if (!insertContextTest())
		LOG_ERROR("INSERT CONTEXT TEST: failed");
	else
		LOG_INFO("INSERT CONTEXT TEST: pass");

	//draw rectangle tests
	if (!drawRectangleTest( shmMemPtr ))
		LOG_ERROR("DRAW RECTANGLE TEST: failed");
	else
		LOG_INFO("DRAW RECTANGLE TEST: pass");

	//draw labels
	if (!drawWindowWithLabelsTest( shmMemPtr ))
		LOG_ERROR("DRAW WINDOW TEST: failed");
	else
		LOG_INFO("DRAW WINDOW TEST: pass");

	//draw labels
	if (!drawWindowWithHBoxTest( shmMemPtr ))
		LOG_ERROR("DRAW HBOX TEST: failed");
	else
		LOG_INFO("DRAW HBOX TEST: pass");


	return 0;
}
