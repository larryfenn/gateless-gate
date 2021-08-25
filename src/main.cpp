#include <Adafruit_Protomatter.h>
#include <Adafruit_GFX.h>
#include <math.h>
#include "ArduinoGL.h"
#include <cstring>
#include <array>
#include <numeric>

uint8_t rgbPins[]  = {7, 8, 9, 10, 11, 12};
uint8_t addrPins[] = {17, 18, 19, 20, 21};
uint8_t clockPin   = 14;
uint8_t latchPin   = 15;
uint8_t oePin      = 16;
// 6 bit color - 64 brightness levels for each of R, G, B.
Adafruit_Protomatter screen(
  64, 6, 1, rgbPins, 5, addrPins, clockPin, latchPin, oePin, true);

float axis_x = 0.f;
float axis_y = 1.f;
float axis_z = 0.f;
float rot_speed = 0;
float angle;

int16_t imu_w;
int16_t imu_x;
int16_t imu_y;
int16_t imu_z;
float mag_x;
float mag_y;
float mag_z;

bool idle = true;

MyCanvas c(64, 64);
void setup(void) {
  Serial1.write(0x1);
  pinMode(0, INPUT);
  pinMode(1, OUTPUT);
  Serial.begin(9600);
  Serial1.begin(115200);
  ProtomatterStatus status = screen.begin();
  Serial.print("Protomatter begin() status: ");
  Serial.println((int)status);
  if (status != PROTOMATTER_OK) {
    for (;;);
  }
  glUseCanvas(&c);
  glClear(GL_COLOR_BUFFER_BIT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(30.0, 1, 11.9f, 20.6f);
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
}

void loop(void) {
  int16_t idle_stickiness = 100;
  // We have to make a tradeoff here -
  // We can either signal and then block the render loop until the IMU gets back to us
  // which might cause rendering hiccups but guarantees (?) low latency on the IMU update
  // Or we can pick up the IMU data whenever its ready, which might mean rendering a frame or two
  // - giving apparent latency
  int16_t new_w;
  int16_t new_x;
  int16_t new_y;
  int16_t new_z;
  if(Serial1.available() >= 20) {
    char buffer[2];
    Serial1.readBytes(buffer, 2);
    std::memcpy(&new_w, &buffer, 2);
    Serial1.readBytes(buffer, 2);
    std::memcpy(&new_x, &buffer, 2);
    Serial1.readBytes(buffer, 2);
    std::memcpy(&new_y, &buffer, 2);
    Serial1.readBytes(buffer, 2);
    std::memcpy(&new_z, &buffer, 2);
    char float_buffer[4];
    Serial1.readBytes(float_buffer, 4);
    std::memcpy(&mag_x, &float_buffer, 4);
    Serial1.readBytes(float_buffer, 4);
    std::memcpy(&mag_y, &float_buffer, 4);
    Serial1.readBytes(float_buffer, 4);
    std::memcpy(&mag_z, &float_buffer, 4);
    int16_t deviation = abs(new_w - imu_w) + abs(new_x - imu_x) + abs(new_y - imu_y) + abs(new_z - imu_z);
    idle = deviation < idle_stickiness;
    // Empty the rest in case theres garbage data
    while(Serial1.available() > 0) {
      Serial1.read();
    }
    Serial1.write(0x1);
  }
  const float scale = 2.5;

  glClear(GL_COLOR_BUFFER_BIT); 
  glLoadIdentity();
  gluLookAt(10, 8, -10, 0, 0, 0, 0, 1, 0);
  if(idle) {
    angle += pow(2, rot_speed) + 2;
    if(angle > 360) {
      angle -= 360;
      if(random(3) == 0) {
        rot_speed = random(3);
        axis_x = random(100) / 100.f;
        axis_y = random(100) / 100.f;
        axis_z = random(100) / 100.f;
      }
    }
    glRotatef(angle, axis_x, axis_y, axis_z);
  } else {
    glRotateq(imu_w / 16384.0f, imu_x / 16384.0f, imu_y / 16384.0f, imu_z / 16384.0f);
  }
  glScalef(scale, scale, scale);
  drawCube();
  MyCanvas tempbuffer(64, 64);
  bool skip_postprocess = false;
  // background is gonna be whatever crap comes out of the pixel shader
  if(!skip_postprocess) {
    for(uint16_t i = 0; i < 64; i++) {
      for(uint16_t j = 0; j < 64; j++) {
        const uint16_t present_color = c.getPixel(i, j);
        if(present_color == 0) {
          Color565 color0 = Color565(present_color);
          Color565 color1 = Color565(c.getPixel(min(63, i + 1), j));
          Color565 color2 = Color565(c.getPixel(i, min(63, j + 1)));
          Color565 color3 = Color565(c.getPixel(max(0, i - 1), j));
          Color565 color4 = Color565(c.getPixel(i, max(0, j - 1)));
          Color565 interp_color = Color565(
            ((uint16_t)((color0.r5() + color1.r5() + color2.r5() + color3.r5() + color4.r5()) / 5.f) << 11) +
            ((uint16_t)((color0.g6() + color1.g6() + color2.g6() + color3.g6() + color4.g6()) / 5.f) << 5) +
            ((uint16_t)((color0.b5() + color1.b5() + color2.b5() + color3.b5() + color4.b5()) / 5.f)));
          if(interp_color > 0) {
            tempbuffer.drawPixel(i, j, interp_color);
          }
        } else {
          tempbuffer.drawPixel(i, j, present_color);
        }
      }
    }
  }
  // the darkest that can be rendered apparently is screen.color565(8, 8, 8)
  if(skip_postprocess) {
    screen.drawRGBBitmap(0, 0, c.getBuffer(), 64, 64);
  } else {
    screen.drawRGBBitmap(0, 0, tempbuffer.getBuffer(), 64, 64);
  }
  screen.show();
}