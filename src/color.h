#include <cstdint>

#ifndef Color_h
#define Color_h

class Color565 {
  public:
    Color565();
    Color565(uint8_t r, uint8_t g, uint8_t b);
    explicit Color565(uint16_t c);

    operator uint16_t() const;

    uint8_t r() const;
    uint8_t g() const;
    uint8_t b() const;

    uint8_t r5() const;
    uint8_t g6() const;
    uint8_t b5() const;

    Color565 with_r(uint8_t r) const;
    Color565 with_g(uint8_t g) const;
    Color565 with_b(uint8_t b) const;

    Color565 dim(float factor);

    bool brighterThan(Color565 color);

  private:
    uint16_t m_color;
};

#endif