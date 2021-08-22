#include <Adafruit_Protomatter.h>
#include <Adafruit_GFX.h>
#include "ArduinoGL.h"
#include <math.h>

uint8_t rgbPins[]  = {7, 8, 9, 10, 11, 12};
uint8_t addrPins[] = {17, 18, 19, 20, 21};
uint8_t clockPin   = 14;
uint8_t latchPin   = 15;
uint8_t oePin      = 16;
// 6 bit color - 64 brightness levels for each of R, G, B.
Adafruit_Protomatter screen(
  64, 6, 1, rgbPins, 5, addrPins, clockPin, latchPin, oePin, true);

float axis_x;
float axis_y;
float axis_z;
float rot_speed;

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
  axis_x = 0.f;
  axis_y = 1.f;
  axis_z = 0.f;
  rot_speed = 1;
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
  Color565 peach(0xfa8a);
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
  //float angle = (millis() / 30 % 360) * 1.0f;
  //float in_data = analogRead(A0);
  //Serial.println(in_data);
  //float angle = (in_data * 360.0f) / 1023;
  const float scale = 2.5;

  glClear(GL_COLOR_BUFFER_BIT); 
  
  glLoadIdentity();
  gluLookAt(10, 8, -10, 0, 0, 0, 0, 1, 0);
  if(millis() % 1000 < 10) {
    int8_t rand_choice = random(4);
    rot_speed = random(4) + 1;
    if(rand_choice == 0) {
      axis_x = random(100) / 100.f;
    } else if(rand_choice == 1) {
      axis_y = random(100) / 100.f;
    } else {
      axis_z = random(100) / 100.f;
    }
  }
  float angle = ((int16_t)(millis() / (5 * pow(2, rot_speed))) % 360) * 1.0f;
  //glRotatef(angle, 0.f, 1.f, 0.f);
  glRotatef(angle, axis_x, axis_y, axis_z);
  glScalef(scale, scale, scale);
  drawCube();
  screen.fillScreen(Color565(0, 0, 0));
  MyCanvas pixelShader(64, 64);
  MyCanvas tempbuffer(64, 64);
  bool skip_postprocess = false;
  uint8_t max_extra_iterations = 0;
  // background is gonna be whatever crap comes out of the pixel shader
  if(!skip_postprocess) {
    for(uint16_t i = 0; i < 64; i++) {
      for(uint16_t j = 0; j < 64; j++) {
        Color565 color0 = Color565(c.getPixel(i, j));
        Color565 color1 = Color565(c.getPixel(min(63, i + 1), j));
        Color565 color2 = Color565(c.getPixel(i, min(63, j + 1)));
        Color565 color3 = Color565(c.getPixel(max(0, i - 1), j));
        Color565 color4 = Color565(c.getPixel(i, max(0, j - 1)));
        Color565 interp_color = Color565(
          ((uint16_t)((color0.r5() + color1.r5() + color2.r5() + color3.r5() + color4.r5()) / 5.f) << 11) +
          ((uint16_t)((color0.g6() + color1.g6() + color2.g6() + color3.g6() + color4.g6()) / 5.f) << 5) +
          ((uint16_t)((color0.b5() + color1.b5() + color2.b5() + color3.b5() + color4.b5()) / 5.f)));
        tempbuffer.drawPixel(i, j, interp_color);
      }
    }
    for(uint16_t i = 0; i < 64; i++) {
      for(uint16_t j = 0; j < 64; j++) {
        if(c.getPixel(i, j) > 0) {
          pixelShader.drawPixel(i, j, c.getPixel(i, j));
        } else {
          pixelShader.drawPixel(i, j, tempbuffer.getPixel(i, j));
        }
      }
    }
    for(uint16_t iteration = 0; iteration < max_extra_iterations; iteration++) {
      for(uint16_t i = 0; i < 64; i++) {
        for(uint16_t j = 0; j < 64; j++) {
          Color565 color0 = Color565(pixelShader.getPixel(i, j));
          Color565 color1 = Color565(pixelShader.getPixel(min(63, i + 1), j));
          Color565 color2 = Color565(pixelShader.getPixel(i, min(63, j + 1)));
          Color565 color3 = Color565(pixelShader.getPixel(max(0, i - 1), j));
          Color565 color4 = Color565(pixelShader.getPixel(i, max(0, j - 1)));
          Color565 interp_color = Color565(
            ((uint16_t)((color0.r5() + color1.r5() + color2.r5() + color3.r5() + color4.r5()) / 5.f) << 11) +
            ((uint16_t)((color0.g6() + color1.g6() + color2.g6() + color3.g6() + color4.g6()) / 5.f) << 5) +
            ((uint16_t)((color0.b5() + color1.b5() + color2.b5() + color3.b5() + color4.b5()) / 5.f)));
          tempbuffer.drawPixel(i, j, interp_color);
        }
      }
      for(uint16_t i = 0; i < 64; i++) {
        for(uint16_t j = 0; j < 64; j++) {
          if(c.getPixel(i, j) > 0) {
            pixelShader.drawPixel(i, j, c.getPixel(i, j));
          } else {
            pixelShader.drawPixel(i, j, tempbuffer.getPixel(i, j));
          }
        }
      }
    }
  }
  // the darkest that can be rendered apparently is screen.color565(8, 8, 8)
  if(skip_postprocess) {
    screen.drawRGBBitmap(0, 0, c.getBuffer(), 64, 64);
  } else {
    screen.drawRGBBitmap(0, 0, pixelShader.getBuffer(), 64, 64);
  }
  screen.show();
}