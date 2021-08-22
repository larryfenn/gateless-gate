#include <Adafruit_GFX.h>
#include <algorithm>
#include <type_traits>
#include "MyCanvas.h"

#ifdef abs
#undef abs
#endif

namespace {
template <class T> class Interpolator {
  public:
    Interpolator(T start, T end, int steps)
        : m_delta(end - start), m_value(start), m_steps(std::abs(steps)), m_step(m_delta / m_steps),
          m_errStep(std::abs(m_delta - m_step * m_steps)), m_error(m_steps / 2) {}

    void increment() {
        m_value += m_step;
        m_error -= m_errStep;
        if (m_error < 0) {
            if (m_delta > 0) {
                ++m_value;
            } else {
                --m_value;
            }
            m_error += m_steps;
        }
    }

    T value() const { return m_value; }

  private:
    T m_delta;
    T m_value;
    int m_steps;
    T m_step;
    T m_errStep;
    T m_error;
};
} // namespace

MyCanvas::MyCanvas(int16_t width, int16_t height) : GFXcanvas16(width, height) {
  GFXcanvas16(width, height);
}

void MyCanvas::drawShadedLine(int16_t x0, int16_t y0, float apparent_d0, int16_t x1, int16_t y1, float apparent_d1, Color565 start_color,
                              Color565 end_color) {
    int16_t steep = std::abs(y1 - y0) > std::abs(x1 - x0);
    if (steep) {
        std::swap(x0, y0);
        std::swap(x1, y1);
    }

    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
        std::swap(start_color, end_color);
    }

    int16_t dx = x1 - x0;
    int16_t dy = std::abs(y1 - y0);

    Interpolator<int8_t> r(start_color.r5(), end_color.r5(), dx);
    Interpolator<int8_t> g(start_color.g6(), end_color.g6(), dx);
    Interpolator<int8_t> b(start_color.b5(), end_color.b5(), dx);

    int16_t err = dx / 2;
    int16_t ystep;

    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }

    for (; x0 <= x1; x0++) {
        uint16_t color = (r.value() << 11) + (g.value() << 5) + b.value();
        if (steep) {
            if(Color565(color).brighterThan(Color565(getRawPixel(y0, x0)))) {            
               writePixel(y0, x0, color);
            }
        } else {
            if(Color565(color).brighterThan(Color565(getRawPixel(x0, y0)))) {            
                writePixel(x0, y0, color);
            }
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }

        r.increment();
        g.increment();
        b.increment();
    }
}
