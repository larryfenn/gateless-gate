#include <Adafruit_Protomatter.h>
#include <Adafruit_GFX.h>
#include <Adafruit_LIS3DH.h>
#include <cstring>
#include "ArduinoGL.h"

#define DEG2RAD (3.14159265358979323846 / 180.0)
uint8_t rgbPins[]  = {7, 8, 9, 10, 11, 12};
uint8_t addrPins[] = {17, 18, 19, 20, 21};
uint8_t clockPin   = 14;
uint8_t latchPin   = 15;
uint8_t oePin      = 16;

Adafruit_Protomatter screen(
  64, 5, 1, rgbPins, 5, addrPins, clockPin, latchPin, oePin, true);
MyCanvas c(64, 64);

// IMU readings
// These are global in case a hiccup occurs and we don't get serial data for one loop
int16_t imu_w = 0;
int16_t imu_x = 0;
int16_t imu_y = 0;
int16_t imu_z = 0;
float mag_x = 0;
float mag_y = 0;
float mag_z = 0;

// Idle animation
float axis_x = 0.f;
float axis_y = 1.f;
float axis_z = 0.f;
int rot_speed = 2;
float angle = 0;
Adafruit_LIS3DH lis = Adafruit_LIS3DH();
int16_t old_lis_x;
int16_t old_lis_y;
int16_t old_lis_z;
bool idle = true;
int16_t idle_threshold = 2000; // lowering this makes the device 'wake up' on gentler nudges
int inactivity_timer_start = 60; // time before the device goes to sleep/idle mode
int inactivity_timer = 0;
int idle_acceleration_timer = 0;

void setup(void) {
  pinMode(0, INPUT);
  pinMode(1, OUTPUT);
  lis.begin(0x19);
  lis.setRange(LIS3DH_RANGE_2_G);
  lis.read();
  old_lis_x = lis.x;
  old_lis_y = lis.y;
  old_lis_z = lis.z;
  Serial.begin(9600);
  Serial1.begin(115200);
  Serial1.write(0x1);
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
  gluPerspective(50.0, 1, 5.669f, 18.661f);
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
  // We have to make a tradeoff here -
  // We can either signal and then block the render loop until the IMU gets back to us
  // which might cause rendering hiccups but guarantees (?) low latency on the IMU update
  // Or we can pick up the IMU data whenever its ready, which might mean rendering a frame or two
  // - giving apparent latency]
  if(Serial1.available() >= 20) {
    char buffer[2];
    Serial1.readBytes(buffer, 2);
    std::memcpy(&imu_w, &buffer, 2);
    Serial1.readBytes(buffer, 2);
    std::memcpy(&imu_x, &buffer, 2);
    Serial1.readBytes(buffer, 2);
    std::memcpy(&imu_y, &buffer, 2);
    Serial1.readBytes(buffer, 2);
    std::memcpy(&imu_z, &buffer, 2);
    char float_buffer[4];
    Serial1.readBytes(float_buffer, 4);
    std::memcpy(&mag_x, &float_buffer, 4);
    Serial1.readBytes(float_buffer, 4);
    std::memcpy(&mag_y, &float_buffer, 4);
    Serial1.readBytes(float_buffer, 4);
    std::memcpy(&mag_z, &float_buffer, 4);
    // Empty the rest in case theres garbage data
    while(Serial1.available() > 0) {
      Serial1.read();
    }
  }
  const float scale = 2.5;

  glClear(GL_COLOR_BUFFER_BIT);
  glLoadIdentity();
  gluLookAt(0, 0, -10, 0, 0, 0, -1, 0, 0);
  if(idle) {
    float speed_penalty = 1;
    if(idle_acceleration_timer++ < 60) {
      speed_penalty = idle_acceleration_timer / 60.f;
    }
    angle += rot_speed * speed_penalty;
    if(angle > 360) {
      angle -= 360;
      if(random(3) == 0) {
        axis_x = random(100) / 100.f;
        axis_y = random(100) / 100.f;
        axis_z = random(100) / 100.f;
        float magnitude = sqrt(axis_x * axis_x + axis_y * axis_y + axis_z * axis_z);
        axis_x = axis_x / magnitude;
        axis_y = axis_y / magnitude;
        axis_z = axis_z / magnitude;
      }
    }
  } else {
    Serial1.write(0x1); // request a new sample be taken next loop
    inactivity_timer--;
  }
  float idle_w = cos(.5 * DEG2RAD * angle);
  float sine_component = sin(.5 * DEG2RAD * angle);
  float idle_x = axis_x * sine_component;
  float idle_y = axis_y * sine_component;
  float idle_z = axis_z * sine_component;
  glRotateq(imu_w / 16384.0f, imu_x / 16384.0f, imu_y / 16384.0f, imu_z / 16384.0f);
  glRotateq(idle_w, idle_x, idle_y, idle_z);
  glScalef(scale, scale, scale);
  drawCube();
  MyCanvas tempbuffer(64, 64);
  bool skip_postprocess = false;
  if(!skip_postprocess) {
    for(uint16_t i = 1; i < 63; i++) {
      for(uint16_t j = 1; j < 63; j++) {
        const uint16_t present_color = c.pixel(i, j);
        if(present_color == 0) {
          Color565 color1 = Color565(c.pixel(i + 1, j));
          Color565 color2 = Color565(c.pixel(i, j + 1));
          Color565 color3 = Color565(c.pixel(i - 1, j));
          Color565 color4 = Color565(c.pixel(i, j - 1));
          Color565 interp_color = Color565(
            (((color1.r5() + color2.r5() + color3.r5() + color4.r5()) / 5) << 11) +
            (((color1.g6() + color2.g6() + color3.g6() + color4.g6()) / 5) << 5) +
            (((color1.b5() + color2.b5() + color3.b5() + color4.b5()) / 5)));
          if(interp_color > 0) {
            tempbuffer.pixel(i, j) = interp_color;
          }
        } else {
          tempbuffer.pixel(i, j) = present_color;
        }
      }
    }
  }
  if(skip_postprocess) {
    screen.drawRGBBitmap(0, 0, c.getBuffer(), 64, 64);
  } else {
    screen.drawRGBBitmap(0, 0, tempbuffer.getBuffer(), 64, 64);
  }
  screen.show();

  // Idle check
  lis.read();
  int16_t deviation = abs(old_lis_x - lis.x) + abs(old_lis_y - lis.y) + abs(old_lis_z - lis.z);
  old_lis_x = lis.x;
  old_lis_y = lis.y;
  old_lis_z = lis.z;
  if(inactivity_timer == 0) {
    idle = true;
  }
  if(deviation > idle_threshold) {
    idle = false;
    inactivity_timer = inactivity_timer_start;
    idle_acceleration_timer = 0;
  }
}