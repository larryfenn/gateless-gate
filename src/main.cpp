#include <Adafruit_Protomatter.h>
#include <algorithm>
#include <array>
#include <cstdint>
#include <functional>

uint8_t rgbPins[]  = {7, 8, 9, 10, 11, 12};
uint8_t addrPins[] = {17, 18, 19, 20, 21};
uint8_t clockPin   = 14;
uint8_t latchPin   = 15;
uint8_t oePin      = 16;

Adafruit_Protomatter screen(
  64, 6, 1, rgbPins, 5, addrPins, clockPin, latchPin, oePin, false);

template <std::size_t Rows, std::size_t Cols, class T>
class Matrix;

template <class Mat>
struct MatrixBase {};

template <std::size_t N, class T>
struct MatrixBase<Matrix<N, N, T>> {
  static constexpr auto identity() {
    Matrix<N, N, T> ret;
    for (std::size_t i = 0; i < N; ++i) {
      ret(i, i) = 1;
    }
    return ret;
  }
};

template <std::size_t Rows, std::size_t Cols, class T>
class Matrix : public MatrixBase<Matrix<Rows, Cols, T>> {
 public:
  T& operator()(int r, int c) { return m_data[c + r * Cols]; }
  const T& operator()(int r, int c) const { return m_data[c + r * Cols]; }

  Matrix& operator+=(const Matrix& other) {
    std::transform(m_data.begin(), m_data.end(), other.m_data.begin(),
                   m_data.begin(), std::plus<>{});
    return *this;
  }

  Matrix& operator-=(const Matrix& other) {
    std::transform(m_data.begin(), m_data.end(), other.m_data.begin(),
                   m_data.begin(), std::minus<>{});
    return *this;
  }

 private:
  std::array<T, Rows * Cols> m_data;
};

template <std::size_t R1, std::size_t C1, std::size_t C2, class T>
constexpr Matrix<R1, C2, T> operator*(const Matrix<R1, C1, T>& lhs,
                                      const Matrix<C1, C2, T>& rhs) {
  Matrix<R1, C2, T> ret;
  for (int r = 0; r < R1; ++r) {
    for (int c = 0; c < C2; ++c) {
      ret(r, c) = 0;
      for (int i = 0; i < C1; ++i) {
        ret(r, c) += lhs(r, i) * rhs(i, c);
      }
    }
  }
  return ret;
}

using Matrix4x4f = Matrix<4, 4, float>;
using Vector4f = Matrix<4, 1, float>;

Vector4f transform(const Matrix4x4f& m, const Vector4f& v) { return m * v; }

Matrix4x4f identity() { return Matrix4x4f::identity(); }


void setup(void) {
  Serial.begin(9600);

  // Initialize matrix...
  ProtomatterStatus status = screen.begin();
  Serial.print("Protomatter begin() status: ");
  Serial.println((int)status);
  if (status != PROTOMATTER_OK) {
    for (;;);
  }
}

int counter = 0;

void loop(void) {
  for (int i = 0; i < 1; i++) {
    screen.drawLine(random(64), random(64), random(64), random(64), screen.colorHSV((128 * counter) % 65536, 128 + random(128), 128 + random(128)));
    screen.show();
  }
  counter++;
  screen.fillScreen(screen.color565(0, 0, 0));
}