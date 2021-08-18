#include <Adafruit_GFX.h>

class MyCanvas : public GFXcanvas16 {
  void drawShadedLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t start_color, uint16_t end_color); 
}