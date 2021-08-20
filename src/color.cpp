#include "color.h"

Color565::Color565() : m_color(0) {}
Color565::Color565(uint8_t r, uint8_t g, uint8_t b) : m_color(((r & 0xf8) << 8) | ((g & 0xfc) << 3) | (b >> 3)) {}
Color565::Color565(uint16_t c) : m_color(c) {}

Color565::operator uint16_t() const { return m_color; }

uint8_t Color565::r() const { return r5() << 3; }
uint8_t Color565::g() const { return g6() << 2; }
uint8_t Color565::b() const { return b5() << 3; }

uint8_t Color565::r5() const { return m_color >> 11; }
uint8_t Color565::g6() const { return (m_color >> 5) & 0x3f; }
uint8_t Color565::b5() const { return m_color & 0x1f; }

Color565 Color565::with_r(uint8_t r) const { return {r, g(), b()}; }
Color565 Color565::with_g(uint8_t g) const { return {r(), g, b()}; }
Color565 Color565::with_b(uint8_t b) const { return {r(), g(), b}; }

Color565 Color565::dim(float factor) {
    uint16_t new_red = (uint16_t)(r5() * factor);
    new_red = (r() > 0) & (new_red < 1) ? 1 : new_red;
    uint16_t new_green = (uint16_t)(g6() * factor);
    new_green = (g() > 0) & (new_green < 1) ? 1 : new_green;
    uint16_t new_blue = (uint16_t)(b5() * factor);
    new_blue = (b() > 0) & (new_blue < 1) ? 1 : new_blue;
    return Color565((new_red << 11) + (new_green << 5) + new_blue);
}

bool Color565::brighterThan(Color565 color) {
    return (r5() + g6() + b5()) > (color.r5() + color.g6() + color.b5());
}