#include <iostream>
#include <cstdio>
#include <string>
#include <thread>
#include <chrono>

#include "r1htypes.h"
#include "scene.h"
#include "renderer.h"
#include "framebuffer.h"
#include "tonemapper.h"

//
#include "eduptscene.h"
#include "scene2013.h"
//

#ifdef WIN32
#include <windows.h>
static double gettimeofday_sec() {
	return timeGetTime() / 1000.0;
}
#else
#include <sys/time.h>
static double gettimeofday_sec() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + (double)tv.tv_usec * 1e-6;
}
#endif

///
#define kProgressOutIntervalSec	60.0

///
int main(int argc, char *argv[]) {

    using namespace r1h;

    double startTime = gettimeofday_sec();

    // renderer setup
    Renderer *render = new Renderer();
    Renderer::Config renderConf = render->getConfig();
	
    renderConf.width = 1280 / 4;
    renderConf.height = 720 / 4;
	renderConf.samples = 16;
	renderConf.subSamples = 2;
	renderConf.tileSize = 64;
	
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
#if 0
		//+++++
		//loaded = scene->loadWavefrontObj("models/cube.obj");
		//loaded = scene->loadWavefrontObj("models/monky_flat.obj");
		//loaded = scene->loadWavefrontObj("models/monky_smooth.obj");
		//loaded = scene->loadWavefrontObj("models/3objs.obj");
		//loaded = scene->loadWavefrontObj("models/3cubes.obj");
		//loaded = scene->loadWavefrontObj("models/2013/mainscene.obj");
		//loaded = scene->loadWavefrontObj("models/manyobjs.obj");
		//loaded = false;
#else
		// On code scene
		//loaded = scene->load();
		loaded = true;
		
		//setupTestCubeScene(*scene); //+++++
		
		//+++++ edupt cornel box scene +++++
		//EduptScene::load(scene, (double)renderConf.width / renderConf.height);
		EduptScene::load2(scene, (double)renderConf.width / renderConf.height);
		//+++++
		//+++++ Render Camp 2013 scene +++++
		//setupMainScene2013(*scene);
		//+++++
#endif
	}
	
    printf("scene loaded [%.4f sec]\n", gettimeofday_sec() - startTime);

	if(loaded) {

		// set tone mapper
		ToneMapper *mapper = new ToneMapper();
		
		// render
		double renderStart = gettimeofday_sec();
		render->render(scene, true); // detach
		
		// wait to finish
		int outcount = 0;
		double prevouttime = gettimeofday_sec();
		double progress = 0;
		do {
			progress = render->getRenderProgress();
			
			printf("rendering : %.2lf %%    \r", progress);
			fflush(stdout);
			
			std::this_thread::sleep_for(std::chrono::seconds(1));
			
			double curtime = gettimeofday_sec();
			if(curtime - prevouttime > kProgressOutIntervalSec) {
				// progress output
				char buf[16];
				sprintf(buf, "%03d.bmp", outcount);
				mapper->exportBMP(render->getFrameBuffer(), buf);
				outcount++;
				prevouttime += kProgressOutIntervalSec;
			}
			
		} while( progress < 100 );

		printf("render finished (%.4f sec) [%.4f sec]\n", gettimeofday_sec() - renderStart, gettimeofday_sec() - startTime);
		
		// final image
		mapper->exportBMP(render->getFrameBuffer(), "final.bmp");

		printf("saved [%.4f sec]\n", gettimeofday_sec() - startTime);
		
		delete mapper;
	}

    // cleaning
    delete render;
    delete scene;

    printf("done [%.4f sec]\n", gettimeofday_sec() - startTime);

    return 0;
}
