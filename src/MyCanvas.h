#include "color.h"
#include <Adafruit_GFX.h>

#ifndef MyCanvas_h
#define MyCanvas_h

class MyCanvas : public GFXcanvas16 {
    void drawShadedLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, Color565 start_color, Color565 end_color);
};

#endif