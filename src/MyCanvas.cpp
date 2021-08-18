#include <Adafruit_GFX.h>

void MyCanvas::drawShadedLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t start_color, uint16_t end_color) {
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    _swap_int16_t(x0, y0);
    _swap_int16_t(x1, y1);
  }

  if (x0 > x1) {
    _swap_int16_t(x0, x1);
    _swap_int16_t(y0, y1);
    _swap_int16_t(start_color, end_color);
  }
  uint8_t red = start_color >> 11;
  uint8_t green = (start_color & 0x07e0) >> 5;
  uint8_t blue = start_color & 0x001f;
  uint8_t end_red = end_color >> 11;
  uint8_t end_green = (end_color & 0x07e0) >> 5;
  uint8_t end_blue = end_color & 0x001f;

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }

  for (; x0 <= x1; x0++) {
    uint16_t color = (red << 11) + (green << 5) + blue;
    if (steep) {
      writePixel(y0, x0, color);
    } else {
      writePixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}
