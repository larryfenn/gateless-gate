#include <Adafruit_GFX.h>
#include "color.h"

#ifndef MyCanvas_h
#define MyCanvas_h


class MyCanvas : public GFXcanvas16 {
    public:
        MyCanvas(int16_t width, int16_t height);

    void drawShadedLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, Color565 start_color, Color565 end_color);

    uint16_t &pixel(int16_t x, int16_t y) { return getBuffer()[x + y * WIDTH]; }
};

#endif