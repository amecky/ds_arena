#define DS_IMPLEMENTATION
#include <diesel.h>
#define STB_IMAGE_IMPLEMENTATION
#include <examples\common\stb_image.h>
#define SPRITE_IMPLEMENTATION
#include "utils\SpriteBatchBuffer.h"
#include "states\GameState.h"
#include "states\MainState.h"
#include "utils\GameContext.h"
#include "utils\highscores.h"
#include "utils\json.h"
// ---------------------------------------------------------------
// load image using stb_image
// ---------------------------------------------------------------
RID loadImage(const char* name) {
	int x, y, n;
	unsigned char *data = stbi_load(name, &x, &y, &n, 4);
	ds::TextureInfo info = { x,y,n,data,ds::TextureFormat::R8G8B8A8_UNORM , ds::BindFlag::BF_SHADER_RESOURCE };
	RID textureID = ds::createTexture(info, name);
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
	SpriteBatchBufferInfo sbbInfo = { 2048, textureID };
	SpriteBatchBuffer spriteBuffer(sbbInfo);

	GameContext ctx;
	highscore::load("scores.scr", &ctx);
	ctx.score = 125634;
	//
	// read game settings from json
	//
	SJSONReader settingsReader;
	settingsReader.parse("content\\settings.json");
	settingsReader.get("max_spawn_enemies", &ctx.settings.maxSpawnEnemies, "settings");
	settingsReader.get("player_highlight", &ctx.settings.playerHightlightColor, "settings");
	settingsReader.get("wake_up_hightlight", &ctx.settings.wakeUpHightlightColor, "settings");
	settingsReader.get("grid_base_color", &ctx.settings.gridBaseColor, "settings");
	settingsReader.get("prepare_ttl", &ctx.settings.prepareTTL, "settings");
	settingsReader.get("prepare_flashing_ttl", &ctx.settings.prepareFlashingTTL, "settings");
	//
	// create the state machine and add all the game states
	//
	StateMachine* stateMachine = new StateMachine(&spriteBuffer);
	PrepareState* prepareState = new PrepareState(&ctx);
	BackgroundState* backgroundState = new BackgroundState(&ctx);
	GameOverState* gameOverState = new GameOverState(&ctx);
	HighscoreState* highscoreState = new HighscoreState(&ctx);
	MainMenuState* mainMenuState = new MainMenuState(&ctx);
	MainState* mainState = new MainState(&ctx, backgroundState);
	// just add all of them in the right order
	stateMachine->add(backgroundState);
	stateMachine->add(mainState);
	stateMachine->add(prepareState);
	stateMachine->add(mainMenuState);
	stateMachine->add(gameOverState);
	stateMachine->add(highscoreState);
	// and activate the main menu state
	stateMachine->activate("GameOverState");
	bool rendering = true;
	bool update = true;
	bool pressed = false;

	while (ds::isRunning() && rendering) {

		ds::begin();

		if (ds::isKeyPressed('U') ) {
			if (!pressed) {
				update = !update;
				pressed = true;
			}
		}
		else {
			pressed = false;
		}

		if (update) {
			stateMachine->tick(static_cast<float>(ds::getElapsedSeconds()));
		}
		//
		// handle all the events that might have occured in one frame
		//
		uint32_t num = stateMachine->numEvents();
		for (uint32_t i = 0; i < num; ++i) {
			// the "get ready" message has elapsed so deacvivate the state
			if (stateMachine->getEventType(i) == ET_PREPARE_ELAPSED) {
				stateMachine->deactivate("PrepareState");
				mainState->startSpawning();
			}
			// user clicked "play" in main menu
			else if (stateMachine->getEventType(i) == ET_MAIN_MENU_PLAY) {
				stateMachine->deactivate("MainMenuState");
				stateMachine->activate("PrepareState");
				stateMachine->activate("MainState");
			}
			// user wants to leave
			else if (stateMachine->getEventType(i) == ET_MAIN_MENU_EXIT) {
				rendering = false;
			}
			// highscores
			else if (stateMachine->getEventType(i) == ET_MAIN_MENU_HIGHSCORES) {
				stateMachine->deactivate("MainMenuState");
				stateMachine->activate("HighscoreState");
			}
			// user clicked exit on game over menu
			else if (stateMachine->getEventType(i) == ET_GAME_OVER_EXIT) {
				stateMachine->deactivate("GameOverState");
				stateMachine->deactivate("MainState");
				stateMachine->activate("MainMenuState");
			}
			// player pressed restart on game over menu
			else if (stateMachine->getEventType(i) == ET_GAME_OVER_PLAY) {
				stateMachine->deactivate("GameOverState");
				stateMachine->activate("PrepareState");
				stateMachine->activate("MainState");
			}
			// player died
			else if (stateMachine->getEventType(i) == ET_PLAYER_KILLED) {
				stateMachine->activate("GameOverState");
				mainState->stopSpawning();
				mainState->startKilling();
			}
			// user clicked exit in the highscore screen
			else if (stateMachine->getEventType(i) == ET_HIGHSCORES_EXIT) {
				stateMachine->deactivate("HighscoreState");
				stateMachine->activate("MainMenuState");
			}
		}
		// now render all active states
		stateMachine->render();
		// let us see how we are doing
		ds::dbgPrint(0, 0, "FPS: %d", ds::getFramesPerSecond());
		ds::dbgPrint(0, 1, "Running: %s", update ? "YES" : "NO");
		ds::end();
	}	
	highscore::save("scores.scr", &ctx);
	delete mainState;
	delete mainMenuState;
	delete highscoreState;
	delete gameOverState;
	delete prepareState;
	delete backgroundState;
	delete stateMachine;
	ds::shutdown();
}