#include "WSBreath.h"
/*
  关键算法说明
  亮度采用余弦缓动：e = 0.5 * (1 - cos(phase))
  再进行 Gamma 校正：e = pow(e, gamma)
  映射到范围：v = min + e * (max - min)
  相位推进：phase += (PI * intervalMs) / halfMs
  其中 halfMs 为半周期（最低到最高）的时间，intervalMs 固定10ms
*/
WSBreath::WSBreath(uint16_t count, uint8_t pin, neoPixelType type)
    : _strip(count, pin, type), _count(count) {}

bool WSBreath::begin(uint8_t initialBrightness) {
  if (_count == 0) return false;
  _strip.begin();
  _strip.setBrightness(initialBrightness);
  _strip.show();
  _inited = true;
  return true;
}

uint8_t WSBreath::clamp255(int v) {
  if (v < 0) return 0;
  if (v > 255) return 255;
  return (uint8_t)v;
}

bool WSBreath::setPeriodMs(uint16_t halfPeriodMs) {
  if (halfPeriodMs < 200) halfPeriodMs = 200;
  if (halfPeriodMs > 20000) halfPeriodMs = 20000;
  _halfMs = halfPeriodMs;
  return true;
}

bool WSBreath::setRange(uint8_t minVal, uint8_t maxVal) {
  if (minVal > maxVal) { uint8_t t = minVal; minVal = maxVal; maxVal = t; }
  _minVal = minVal;
  _maxVal = maxVal;
  return true;
}

bool WSBreath::setColor(uint8_t r, uint8_t g, uint8_t b) {
  _r = r; _g = g; _b = b;
  return true;
}

void WSBreath::setEnabled(bool en) {
  _enabled = en;
  if (!_enabled) fill(0, 0, 0);
}

uint16_t WSBreath::getPeriodMs() const { return _halfMs; }

void WSBreath::getRange(uint8_t& minVal, uint8_t& maxVal) const {
  minVal = _minVal; maxVal = _maxVal;
}

void WSBreath::getColor(uint8_t& r, uint8_t& g, uint8_t& b) const {
  r = _r; g = _g; b = _b;
}

bool WSBreath::getEnabled() const { return _enabled; }

void WSBreath::fill(uint8_t r, uint8_t g, uint8_t b) {
  uint32_t c = _strip.Color(r, g, b);
  for (uint16_t i = 0; i < _count; ++i) _strip.setPixelColor(i, c);
  _strip.show();
}

void WSBreath::update(unsigned long now) {
  if (!_inited) return;
  if (!_enabled) return;
  uint16_t dt = (uint16_t)(now - _last);
  if (dt < _intervalMs) return;
  _last = now;
  _lastDt = dt;
  float e = 0.5f * (1.0f - cosf(_phase));
  if (_gamma == 3.0f) e = e * e * e; else e = powf(e, _gamma);
  float v = _minVal + e * (_maxVal - _minVal);
  uint8_t br = clamp255((int)v);
  // 将颜色按亮度缩放
  uint8_t r = clamp255((_r * br) / 255);
  uint8_t g = clamp255((_g * br) / 255);
  uint8_t b = clamp255((_b * br) / 255);
  fill(r, g, b);
  _phase += (PI * _lastDt) / _halfMs;
  if (_phase >= 2 * PI) _phase -= 2 * PI;
}
