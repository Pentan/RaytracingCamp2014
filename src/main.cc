#include <cstdio>
#include <string>

#include "r1htypes.h"
#include "scene.h"
#include "renderer.h"
#include "framebuffer.h"
#include "tonemapper.h"

//
#include "eduptscene.h"
#include "scene2013.h"
//

#include <sys/time.h>
static double gettimeofday_sec() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + (double)tv.tv_usec * 1e-6;
}

int main(int argc, char *argv[]) {

    using namespace r1h;

    double startTime = gettimeofday_sec();

    // renderer setup
    Renderer *render = new Renderer();
    Renderer::Config renderConf = render->getConfig();
	
    renderConf.width = 1280 / 4;
    renderConf.height = 720 / 4;
	renderConf.samples = 8;
	renderConf.subSamples = 1;
	
	// parse render config from arguments?
	
    render->setConfig(renderConf);
	
    printf("renderer configured [%.4f sec]\n", gettimeofday_sec() - startTime);

    // scene setup
    Scene *scene;
	
	bool loaded = false;
	
	scene = new Scene();
	
	if(argc > 1) {
		loaded = scene->loadWavefrontObj(argv[1]);
	} else {
		printf("usage:%s [file.obj]\n", argv[0]);
		printf("no obj set. setup default scene.\n");
#if 1
		//+++++
		//loaded = scene->loadWavefrontObj("models/cube.obj");
		//loaded = scene->loadWavefrontObj("models/monky_flat.obj");
		//loaded = scene->loadWavefrontObj("models/monky_smooth.obj");
		loaded = scene->loadWavefrontObj("models/3objs.obj");
		//loaded = scene->loadWavefrontObj("models/3cubes.obj");
		//loaded = scene->loadWavefrontObj("models/2013/mainscene.obj");
		//loaded = false;
#else
		// On code scene
		loaded = scene->load();
		
		setupTestCubeScene(*scene); //+++++
		
		//+++++ edupt cornel box scene +++++
		//EduptScene::load(scene, (double)renderConf.width / renderConf.height);
		//+++++
		//+++++ Render Camp 2013 scene +++++
		//setupMainScene2013(*scene);
		//+++++
#endif
	}
	
    printf("scene loaded [%.4f sec]\n", gettimeofday_sec() - startTime);

	if(loaded) {
		double renderStart = gettimeofday_sec();

		// render
		render->render(scene);

		printf("renderered (%.4f sec) [%.4f sec]\n", gettimeofday_sec() - renderStart, gettimeofday_sec() - startTime);

		// tone map and save
		FrameBuffer *buffer = render->getFrameBuffer();
		ToneMapper *mapper = new ToneMapper(buffer);
		mapper->exportBMP("r1h2014.bmp");

		printf("saved [%.4f sec]\n", gettimeofday_sec() - startTime);
		
		delete mapper;
	}

    // cleaning
    delete render;
    delete scene;

    printf("done [%.4f sec]\n", gettimeofday_sec() - startTime);

    return 0;
}
