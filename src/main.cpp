#define DS_IMPLEMENTATION
#include <diesel.h>
#define STB_IMAGE_IMPLEMENTATION
#include <examples\common\stb_image.h>
#define SPRITE_IMPLEMENTATION
#include <SpriteBatchBuffer.h>
#define DS_STATEMACHINE_IMPLEMENTATION
#include <StateMachine.h>
#include "states\GameState.h"
#include "states\MainState.h"
#include "utils\GameContext.h"
#include "utils\highscores.h"
#define GAMESETTINGS_IMPLEMENTATION
#include <ds_tweakable.h>
#include <ds_imgui.h>
#include "utils\Sandbox.h"

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

void addSettings(const char* catName, EmitterSettings* settings) {
	twk_add(catName,"count", &settings->count);
	twk_add(catName, "angle_variance", &settings->angleVariance);
	twk_add(catName, "radius", &settings->radius);
	twk_add(catName, "radius_variance", &settings->radiusVariance);
	twk_add(catName, "ttl", &settings->ttl);
	twk_add(catName, "velocity", &settings->velocity);
	twk_add(catName, "velocity_variance", &settings->velocityVariance);
	twk_add(catName, "size", &settings->size);
	twk_add(catName, "size_variance", &settings->sizeVariance);
	twk_add(catName, "growth", &settings->growth);
	twk_add(catName, "acceleration", &settings->acceleration);
	twk_add(catName, "decay", &settings->decay);
	twk_add(catName, "color", &settings->color);
	twk_add(catName, "texture_rect", &settings->texRect);
}


void printErrors(const char* message) {
	OutputDebugString(message);
	OutputDebugString("\n");
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
	SpriteBatchBufferInfo sbbInfo = { 2048, textureID, ds::TextureFilters::LINEAR };
	SpriteBatchBuffer spriteBuffer(sbbInfo);

	ParticlesystemDescriptor descriptor = { 4096, textureID };

	GameContext ctx;
	highscore::load("scores.scr", &ctx);
	ctx.score = 125634;

	ctx.particleManager = new ParticleManager(4096, textureID);

	gui::init();

	twk_init("content\\settings.json",&printErrors);


	ctx.enemyExplosion = ctx.particleManager->add(descriptor);
	ctx.playerTrail = ctx.particleManager->add(descriptor);
	ctx.wakeUpSystem = ctx.particleManager->add(descriptor);

	addSettings("explosion", &ctx.emitterSettings[PSystems::PS_EXPLOSION]);
	addSettings("bullet_explosion", &ctx.emitterSettings[PSystems::PS_BULLET]);
	addSettings("player_trail", &ctx.emitterSettings[PSystems::PS_TRAIL]);
	addSettings("wake_up", &ctx.emitterSettings[PSystems::PS_WAKEUP]);
	addSettings("death", &ctx.emitterSettings[PSystems::PS_DEATH]);
	addSettings("light_streaks", &ctx.emitterSettings[PSystems::PS_LIGHT_STREAKS]);

	twk_add("settings","max_spawn_enemies", &ctx.settings.maxSpawnEnemies);
	twk_add("settings", "player_highlight", &ctx.settings.playerHightlightColor);
	twk_add("settings", "wake_up_hightlight", &ctx.settings.wakeUpHightlightColor);
	twk_add("settings", "grid_base_color", &ctx.settings.gridBaseColor);
	twk_add("settings", "prepare_ttl", &ctx.settings.prepareTTL);
	twk_add("settings", "prepare_flashing_ttl", &ctx.settings.prepareFlashingTTL);

	ElasticBorderSettings borderSettings;
	twk_add("border_settings", "Tension", &borderSettings.Tension);
	twk_add("border_settings", "Dampening", &borderSettings.Dampening);
	twk_add("border_settings", "Spread", &borderSettings.Spread);
	twk_add("border_settings", "numX", &borderSettings.numX);
	twk_add("border_settings", "numY", &borderSettings.numY);
	twk_add("border_settings", "thickness", &borderSettings.thickness);
	twk_add("border_settings", "verticalTexture", &borderSettings.verticalTexture);
	twk_add("border_settings", "horizontalTexture", &borderSettings.horizontalTexture);
	twk_add("border_settings", "targetHeight", &borderSettings.targetHeight);
	twk_add("border_settings", "splashForce", &borderSettings.splashForce);
	twk_add("border_settings", "length", &borderSettings.length);

	twk_load();
	//
	// create the state machine and add all the game states
	//
	ArenaStateMachine* stateMachine = new ArenaStateMachine(&spriteBuffer);
	PrepareState* prepareState = new PrepareState(&ctx);
	BackgroundState* backgroundState = new BackgroundState(&ctx, borderSettings);
	GameOverState* gameOverState = new GameOverState(&ctx);
	HighscoreState* highscoreState = new HighscoreState(&ctx);
	MainMenuState* mainMenuState = new MainMenuState(&ctx);
	MainState* mainState = new MainState(&ctx, backgroundState);
	ParticlesTestState* particlesTestState = new ParticlesTestState(&ctx);
	// just add all of them in the right order
	stateMachine->add(backgroundState);
	stateMachine->add(particlesTestState);
	stateMachine->add(mainState);
	stateMachine->add(prepareState);
	stateMachine->add(mainMenuState);
	stateMachine->add(gameOverState);
	stateMachine->add(highscoreState);
	// and activate the main menu state
	//stateMachine->activate("PrepareState");
	//stateMachine->activate("MainState");
	//stateMachine->activate("ParticlesTestState");
	bool rendering = true;
	bool update = true;
	bool pressed = false;


	SandBox sandbox;

	while (ds::isRunning() && rendering) {

		ds::begin();

		if (twk_load()) {
			printErrors("File reloaded");
		}

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
		// handle all the events that might have occurred in one frame
		//
		uint32_t num = stateMachine->numEvents();
		for (uint32_t i = 0; i < num; ++i) {
			// the "get ready" message has elapsed so deactivate the state
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

		sandbox.render();
		// let us see how we are doing
		ds::dbgPrint(0, 0, "FPS: %d", ds::getFramesPerSecond());
		//ds::dbgPrint(0, 1, "Running: %s", update ? "YES" : "NO");
		ds::end();
	}	
	highscore::save("scores.scr", &ctx);
	delete ctx.particleManager;
	delete mainState;
	delete mainMenuState;
	delete highscoreState;
	delete gameOverState;
	delete prepareState;
	delete backgroundState;
	delete stateMachine;
	gui::shutdown();
	ds::shutdown();
}