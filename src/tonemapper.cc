#include "tonemapper.h"
#include "framebuffer.h"

using namespace r1h;

/// pass-through tone mapper
ToneMapper::ToneMapper(FrameBuffer *buffer) : AbstractToneMapper()
{
    framebuffer = buffer;
};

ToneMapper::~ToneMapper() {
    printf("tone mapper destructed\n");
};
