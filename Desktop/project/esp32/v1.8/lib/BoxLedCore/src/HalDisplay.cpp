#include "HalDisplay.h"
#include <Wire.h>

HalDisplay::HalDisplay() : _u8g2(U8G2_R0, U8X8_PIN_NONE) {}

void HalDisplay::begin() {
  Wire.begin(8, 9); // Custom I2C pins from original code
  _u8g2.begin();
  _u8g2.setFontMode(1);
  _u8g2.setDrawColor(1);
}

void HalDisplay::clear() {
  _u8g2.clearBuffer();
}

void HalDisplay::display() {
  _u8g2.sendBuffer();
}

void HalDisplay::setPowerSave(bool enable) {
  _u8g2.setPowerSave(enable ? 1 : 0);
}

void HalDisplay::setFont(DisplayFont font) {
  if (font == FONT_SMALL) {
    _u8g2.setFont(u8g2_font_6x10_tf);
  } else {
    _u8g2.setFont(u8g2_font_wqy16_t_gb2312a);
  }
}

void HalDisplay::setDrawColor(uint8_t c) {
  _u8g2.setDrawColor(c);
}

void HalDisplay::drawStr(int x, int y, const char* s) {
  _u8g2.drawStr(x, y, s);
}

void HalDisplay::drawUTF8(int x, int y, const char* s) {
  _u8g2.drawUTF8(x, y, s);
}

int HalDisplay::getStrWidth(const char* s) {
  return _u8g2.getStrWidth(s);
}

int HalDisplay::getUTF8Width(const char* s) {
  return _u8g2.getUTF8Width(s);
}

void HalDisplay::drawFrame(int x, int y, int w, int h) {
  _u8g2.drawFrame(x, y, w, h);
}

void HalDisplay::drawBox(int x, int y, int w, int h) {
  _u8g2.drawBox(x, y, w, h);
}

void HalDisplay::drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3) {
  _u8g2.drawTriangle(x1, y1, x2, y2, x3, y3);
}
