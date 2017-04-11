#define DS_IMPLEMENTATION
#include "..\diesel\diesel.h"
#define STB_IMAGE_IMPLEMENTATION
#include "..\diesel\examples\common\stb_image.h"
#include "src\GameState.h"

// ---------------------------------------------------------------
// load image using stb_image
// ---------------------------------------------------------------
RID loadImage(const char* name) {
	int x, y, n;
	unsigned char *data = stbi_load(name, &x, &y, &n, 4);
	RID textureID = ds::createTexture(x, y, n, data, ds::TextureFormat::R8G8B8A8_UNORM, name);
	stbi_image_free(data);
	return textureID;
}

// ---------------------------------------------------------------
// main method
// ---------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow) {
	//
	// prepare application
	//
	ds::RenderSettings rs;
	rs.width = 1024;
	rs.height = 768;
	rs.title = "ds_arena";
	rs.clearColor = ds::Color(0.0f, 0.0f, 0.0f, 1.0f);
	rs.multisampling = 4;
	rs.useGPUProfiling = false;
	ds::init(rs);
	//
	// load the one and only texture
	//
	RID textureID = loadImage("content\\TextureArray.png");
	sprites::init(2048, textureID);
	//
	// create the state machine and add all the game states
	//
	StateMachine stateMachine;
	PrepareState prepareState;
	BackgroundState backgroundState;
	GameOverState gameOverState;
	MainMenuState mainMenuState;
	MainState mainState(&backgroundState);
	// just add all of them in the right order
	stateMachine.add(&backgroundState);
	stateMachine.add(&prepareState);
	stateMachine.add(&mainMenuState);
	stateMachine.add(&gameOverState);
	stateMachine.add(&mainState);
	// and activate the main menu state
	stateMachine.activate("MainMenuState");

	while (ds::isRunning()) {

		ds::begin();

		stateMachine.tick(static_cast<float>(ds::getElapsedSeconds()));
		//
		// handle all the events that might have occured in one frame
		//
		uint32_t num = stateMachine.numEvents();
		for (uint32_t i = 0; i < num; ++i) {
			// the "get ready" message has elapsed so deacvivate the state
			if (stateMachine.getEventType(i) == ET_PREPARE_ELAPSED) {
				stateMachine.deactivate("PrepareState");
				mainState.startSpawning();
			}
			// user clicked "play" in main menu
			else if (stateMachine.getEventType(i) == ET_MAIN_MENU_PLAY) {
				stateMachine.deactivate("MainMenuState");
				stateMachine.activate("PrepareState");
			}
			// user wants to leave
			else if (stateMachine.getEventType(i) == ET_MAIN_MENU_EXIT) {
				ds::stopRendering();
			}
		}
		// now render all active states
		stateMachine.render();
		// yeah, let us see how we are doing
		ds::dbgPrint(0, 0, "FPS: %d", ds::getFramesPerSecond());

		ds::end();
	}
	sprites::shutdown();
	ds::shutdown();
}