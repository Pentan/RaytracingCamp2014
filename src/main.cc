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
#include "scene2014.h"
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

	std::cout << "ray&bow- season 1" << std::endl;

    using namespace r1h;

    double startTime = gettimeofday_sec();

    // renderer setup
    Renderer *render = new Renderer();
    Renderer::Config renderConf = render->getConfig();
	
    renderConf.width = 1280 / 1;
    renderConf.height = 720 / 1;
	renderConf.samplesWidth = 8;
	renderConf.samplesDepth = 4;
	renderConf.subSamples = 2;
	renderConf.tileSize = 64;
    renderConf.maxDepth = 12;
	
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
		
#if 0
		printf("usage:%s [file.obj]\n", argv[0]);
		printf("no obj set. setup default scene.\n");
		//loaded = scene->loadWavefrontObj("models/cube.obj");
		//loaded = scene->loadWavefrontObj("models/monky_flat.obj");
		//loaded = scene->loadWavefrontObj("models/monky_smooth.obj");
		//loaded = scene->loadWavefrontObj("models/3objs.obj");
		//loaded = scene->loadWavefrontObj("models/3cubes.obj");
		//loaded = scene->loadWavefrontObj("models/2013/mainscene.obj");
		//loaded = scene->loadWavefrontObj("models/manyobjs.obj");
		//loaded = scene->loadWavefrontObj("extra/bleriot.obj");
		//loaded = scene->loadWavefrontObj("extra/fantasy.obj");
		
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
		
		///// 2014
		loaded = setupMainScene2014(renderConf.width, renderConf.height, scene);
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
        
        int numcntx = (int)render->getRecderContextCount();
        
		do {
			double progress = render->getRenderProgress();
			
			std::this_thread::sleep_for(std::chrono::seconds(1));
			
			double curtime = gettimeofday_sec();
			if(curtime - prevouttime > kProgressOutIntervalSec) {
				// progress output
				char buf[16];
#ifdef WIN32
				sprintf_s(buf, "%03d.bmp", outcount);
#else
				sprintf(buf, "%03d.bmp", outcount);
#endif
				mapper->exportBMP(render->getFrameBuffer(), buf);
                printf("progress image %s saved\n", buf);
				outcount++;
				prevouttime += kProgressOutIntervalSec;
			}
			
			printf("%.2lf%%:", progress);
            for(int i = 0; i < numcntx; i++) {
                const Renderer::Context *cntx = render->getRenderContext(i);
                printf("[%d:%.1lf]", i, cntx->tileProgress * 100.0);
            }
            printf("    \r");
            fflush(stdout);
            
		} while( !render->isFinished() );

		printf("render finished (%.4f sec) [%.4f sec]\n", gettimeofday_sec() - renderStart, gettimeofday_sec() - startTime);
		
		// final image
        const char *finalname = "final.bmp";
		mapper->exportBMP(render->getFrameBuffer(), finalname);
        printf("%s saved\n", finalname);

		printf("saved [%.4f sec]\n", gettimeofday_sec() - startTime);
		
		delete mapper;
	}

    // cleaning
    delete render;
    delete scene;

    printf("done [%.4f sec]\n", gettimeofday_sec() - startTime);

    return 0;
}
