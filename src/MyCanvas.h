#include <Adafruit_GFX.h>
#include "color.h"

#ifndef MyCanvas_h
#define MyCanvas_h


class MyCanvas : public GFXcanvas16 {
    public:
        MyCanvas(int16_t width, int16_t height);

    void drawShadedLine(int16_t x0, int16_t y0, float apparent_d0, int16_t x1, int16_t y1, float apparent_d1, Color565 start_color, Color565 end_color);
};

#endif