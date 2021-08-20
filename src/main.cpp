#include <Adafruit_Protomatter.h>
#include <Adafruit_GFX.h>
#include "ArduinoGL.h"

uint8_t rgbPins[]  = {7, 8, 9, 10, 11, 12};
uint8_t addrPins[] = {17, 18, 19, 20, 21};
uint8_t clockPin   = 14;
uint8_t latchPin   = 15;
uint8_t oePin      = 16;

// 6 bit color - 64 brightness levels for each of R, G, B.
Adafruit_Protomatter screen(
  64, 6, 1, rgbPins, 5, addrPins, clockPin, latchPin, oePin, true);

MyCanvas c(64, 64);
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
  /* 
           2
          /|\
        /  |  1
      3    7 /|
      |\   / \|
      |  4    8
      6  |   /
       \ | /
         5
  */
  Color565 red(255, 0, 0);
  Color565 yellow(255, 255, 0);
  Color565 green(0, 255, 0);
  Color565 cyan(0, 255, 255);
  Color565 blue(0, 0, 255);
  Color565 magenta(255, 0, 255);
  Color565 white(255, 255, 255);
  Color565 peach(0xfd51);
  Color565 lime(0x97f1);
  Color565 robinsegg(0x9fff);
  Color565 lavender(0x9c3f);
  Color565 azure(0x043f);
 
  abstract_point point1 = { 1, 1, 1, red };
  abstract_point point2 = { -1, 1, 1, blue };
  abstract_point point3 = { -1, -1, 1, green };
  abstract_point point4 = { 1, -1, 1, blue };
  abstract_point point5 = { 1, -1, -1, green };
  abstract_point point6 = { -1, -1, -1, red };
  abstract_point point7 = { -1, 1, -1, green };
  abstract_point point8 = { 1, 1, -1, blue };

  // Bottom (from the center of the cube looking at the camera)
  glBegin(GL_POLYGON);
    glVertex3f(point6);
    glVertex3f(point5);
    glVertex3f(point8);
    glVertex3f(point7);
  glEnd();
  // Front
  glBegin(GL_POLYGON);
    glVertex3f(point5);
    glVertex3f(point4);
    glVertex3f(point1);
    glVertex3f(point8);
  glEnd();
  // Top
  glBegin(GL_POLYGON);
    glVertex3f(point4);
    glVertex3f(point3);
    glVertex3f(point2);
    glVertex3f(point1);
  glEnd();
  // Back
  glBegin(GL_POLYGON);
    glVertex3f(point3);
    glVertex3f(point6);
    glVertex3f(point7);
    glVertex3f(point2);
  glEnd();
  // Right
  glBegin(GL_POLYGON);
    glVertex3f(point3);
    glVertex3f(point4);
    glVertex3f(point5);
    glVertex3f(point6);
  glEnd();
  // Left
  glBegin(GL_POLYGON);
    glVertex3f(point7);
    glVertex3f(point8);
    glVertex3f(point1);
    glVertex3f(point2);
  glEnd();
}

void loop(void) {
  float angle = (millis() / 100 % 360) * 1.0f;
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