#include "renderer.h"
#include <chrono>
#include <random>

#include "scene.h"
#include "framebuffer.h"
#include "commandqueue.h"
#include "ray.h"
#include "camera.h"

using namespace r1h;

Renderer::Renderer():
    frameBuffer(0),
    config(),
    renderQueue(0)
{
    // alloc workers
}

Renderer::~Renderer() {
    if( frameBuffer ) {
        delete frameBuffer;
        frameBuffer = NULL;
    }
    if( renderQueue ) {
        delete renderQueue;
        renderQueue = NULL;
    }
}
    
void Renderer::render(Scene *scene) {
    // framebuffer
    frameBuffer = new FrameBuffer(config.width, config.height);
    int w = frameBuffer->getWidth();
    int h = frameBuffer->getHeight();
    
    // queue
    renderQueue = new RenderCommandQueue();
    // push tile command
    for(int iy = 0; iy < h; iy += config.tileSize) {
        for(int ix = 0; ix < w; ix += config.tileSize) {
            FrameBuffer::Tile tile = frameBuffer->makeTile(ix, iy, config.tileSize, config.tileSize);
            renderQueue->pushTileCommand(tile);
        }
    }
    
	
    // detect workers
    int numthreads = std::thread::hardware_concurrency();
    if(numthreads <= 0) {
        numthreads = config.defaultThreads;
    }
    workers.resize(numthreads);
	printf("%d threads\n", numthreads);
    
	// init contexts and workers
	renderContexts = new std::vector<Context>(numthreads);
	std::random_device rnddvice;
	for(int i = 0; i < numthreads; ++i) {
		// context
		Context *cntx = &renderContexts->at(i);
		cntx->init(rnddvice(), &config);
		// worker
		workers[i] = std::thread(startWorker, this, i, scene);
	}
	
	for(int i = 0; i < numthreads; ++i) {
		workers[i].join();
	}
	
	/*
    //+++++
	{
		//workerJob(0, scene);
		Renderer::startWorker(this, 0, scene);
	}
    //+++++
    */
	
    // start render
    printf("render!\n");
};

void Renderer::workerJob(int workerId, Scene *scene) {
    // get tile and render!
    
	Context *cntx = &renderContexts->at(workerId);
	
    bool working = true;
    while(working) {
        RenderCommandQueue::Command cmd = renderQueue->popCommand();
        
        switch(cmd.type) {   
            case RenderCommandQueue::CommandType::kTile:
                // render tile
                renderTile(cntx, scene, cmd.tile);
                break;
            case RenderCommandQueue::kSleep:
                // sleep
                std::this_thread::sleep_for(std::chrono::microseconds(cmd.usec));
                break;
            case RenderCommandQueue::kFinish:
                working = false;
                break;
            default:
                printf("not handled render command 0x%x\n", cmd.type);
                working = false;
        }
    }
}

void Renderer::renderTile(Context *cntx, Scene *scene, FrameBuffer::Tile tile) {
    printf("[0x%x] render tile (x(%d, %d),y(%d, %d))(size(%d, %d))\n", std::this_thread::get_id(), tile.startx, tile.endx, tile.starty, tile.endy, tile.endx - tile.startx, tile.endy - tile.starty);
    
    int ss = config.subSamples;
    R1hFPType ssrate = 1.0 / ss;
    R1hFPType divw = 1.0 / config.width;
    R1hFPType divh = 1.0 / config.height;
    
    Camera *camera = scene->getCamera();
    
    for(int iy = tile.starty; iy < tile.endy; iy++) {
        for(int ix = tile.startx; ix < tile.endx; ix++) {
            for(int ssy = 0; ssy < ss; ssy++) {
                for(int ssx = 0; ssx < ss; ssx++) {
                    R1hFPType px = ix + (ssx + 0.5) * ssrate;
                    R1hFPType py = iy + (ssy + 0.5) * ssrate;
                    R1hFPType cx = px * divw * 2.0 - 1.0;
                    R1hFPType cy = py * divh * 2.0 - 1.0;
                    
                    for(int smpl = 0; smpl < config.samples; smpl++) {
                        Ray ray = camera->getRay(cx, cy);
                        Color c = scene->radiance(cntx, ray);
                        frameBuffer->accumulate(ix, iy, c);
                    }
                }
            }
        }
    }
}

void Renderer::startWorker(Renderer *rndr, int workerId, Scene *scene) {
	rndr->workerJob(workerId, scene);
}


