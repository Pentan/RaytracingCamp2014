#ifndef R1H_FRAME_BUFFER_H
#define R1H_FRAME_BUFFER_H

#include "r1htypes.h"

namespace r1h {

class FrameBuffer {
public:
    struct Tile {
        int startx, endx;
        int starty, endy;
        
        Tile(int sx, int sy, int ex, int ey):
            startx(sx), endx(ex), starty(sy), endy(ey)
        {}
    };
    
public:
    FrameBuffer(int w, int h);
    ~FrameBuffer();
    
    int getWidth() const { return width; };
    int getHeight() const { return height; };
    Color getColorAt(const int x, const int y) const;
    
    Tile makeTile(const int x, const int y, const int w, const int h) const;
    
    void accumulate(const int x, const int y, const Color& col);
    
private:
    struct Pixel {
        Color color;
        unsigned long sampleCount;
        
        void clear();
        void addColor(const Color &col);
    } *buffer;
    
    int width;
    int height;
};

}

#endif