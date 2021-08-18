#include <Adafruit_Protomatter.h>
#include <ArduinoGL.h>
#include <Adafruit_GFX.h>

uint8_t rgbPins[]  = {7, 8, 9, 10, 11, 12};
uint8_t addrPins[] = {17, 18, 19, 20, 21};
uint8_t clockPin   = 14;
uint8_t latchPin   = 15;
uint8_t oePin      = 16;

// 6 bit color - 64 brightness levels for each of R, G, B.
Adafruit_Protomatter screen(
  64, 6, 1, rgbPins, 5, addrPins, clockPin, latchPin, oePin, true);

GFXcanvas16 c(64, 64);
void setup(void) {
  Serial.begin(9600);
  ProtomatterStatus status = screen.begin();
  Serial.print("Protomatter begin() status: ");
  Serial.println((int)status);
  if (status != PROTOMATTER_OK) {
    for (;;);
  }
  glUseCanvas(&c);
  glClear(GL_COLOR_BUFFER_BIT);
  glPointSize(4);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  //glOrtho(-5, 5, -5, 5, 0.1, 9999.f);
  gluPerspective(30.0, c.width()/c.height(), 0.1f, 999.f);
  glMatrixMode(GL_MODELVIEW);
}

void drawCube() {                

  glBegin(GL_POLYGON);
    glVertex3f(-1, -1, -1);
    glVertex3f(1, -1, -1);
    glVertex3f(1, 1, -1);
    glVertex3f(-1, 1, -1);
  glEnd();

  glBegin(GL_POLYGON);
    glVertex3f(1, -1, -1);
    glVertex3f(1, -1, 1);
    glVertex3f(1, 1, 1);
    glVertex3f(1, 1, -1);
  glEnd();

  glBegin(GL_POLYGON);
    glVertex3f(1, -1, 1);
    glVertex3f(-1, -1, 1);
    glVertex3f(-1, 1, 1);
    glVertex3f(1, 1, 1);
  glEnd();

  glBegin(GL_POLYGON);
    glVertex3f(-1, -1, 1);
    glVertex3f(-1, -1, -1);
    glVertex3f(-1, 1, -1);
    glVertex3f(-1, 1, 1);
  glEnd();

  glBegin(GL_POLYGON);
    glVertex3f(-1, -1, 1);
    glVertex3f(1, -1, 1);
    glVertex3f(1, -1, -1);
    glVertex3f(-1, -1, -1);
  glEnd();

  glBegin(GL_POLYGON);
    glVertex3f(-1, 1, -1);
    glVertex3f(1, 1, -1);
    glVertex3f(1, 1, 1);
    glVertex3f(-1, 1, 1);
  glEnd();
}

void loop(void) {
  float angle = (millis() / 10 % 360) * 1.0f;
  //float in_data = analogRead(A0);
  //Serial.println(in_data);
  //float angle = (in_data * 360.0f) / 1023;
  const float scale = 2.5;

  glClear(GL_COLOR_BUFFER_BIT); 
  
  glLoadIdentity();
  gluLookAt(10, 8, -10, 0, 0, 0, 0, 1, 0);
  glRotatef(angle, 0.f, 1.f, 0.f);
  glScalef(scale, scale, scale);
  drawCube();
  screen.fillScreen(screen.color565(0, 0, 0));
  // the darkest that can be rendered apparently is screen.color565(8, 8, 8)
  screen.drawRGBBitmap(0, 0, c.getBuffer(), 64, 64);
  screen.show();
}