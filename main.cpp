#define DS_IMPLEMENTATION
#include "..\diesel\diesel.h"
#define STB_IMAGE_IMPLEMENTATION
#include "..\diesel\examples\common\stb_image.h"
#include "src\Game.h"


// ---------------------------------------------------------------
// load image using stb_image
// ---------------------------------------------------------------
RID loadImage(const char* name) {
	int x, y, n;
	unsigned char *data = stbi_load(name, &x, &y, &n, 4);
	RID textureID = ds::createTexture(x, y, n, data, ds::TextureFormat::R8G8B8A8_UNORM);
	stbi_image_free(data);
	return textureID;
}

// ---------------------------------------------------------------
// main method
// ---------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow) {
//int main(int argc, char *argv[]) {
	//
	// prepare application
	//
	ds::RenderSettings rs;
	rs.width = 1024;
	rs.height = 768;
	rs.title = "Dodger";
	rs.clearColor = ds::Color(0.1f, 0.1f, 0.1f, 1.0f);
	rs.multisampling = 4;
	rs.useGPUProfiling = false;
	ds::init(rs);

	RID textureID = loadImage("content\\TextureArray.png");
	sprites::init(2048, textureID);

	Game game;

	while (ds::isRunning()) {

		ds::begin();

		game.tick(static_cast<float>(ds::getElapsedSeconds()));
		
		//sprites::begin();

		game.render();
		
		//sprites::flush();

		ds::dbgPrint(0, 0, "FPS: %d", ds::getFramesPerSecond());

		ds::end();
	}
	sprites::shutdown();
	ds::shutdown();
}