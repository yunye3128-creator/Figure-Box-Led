#include "Effects.h"
#include "StateManager.h"

static void effectsOnEvent(Topic topic, const SystemState& s, void* ctx) {
  if (topic != TOPIC_EFFECT_CHANGE || !ctx) return;
  Effects* self = (Effects*)ctx;
  self->setEffect(s.effect);
  self->setBreatheHalfMs(s.breatheHalfMs);
  self->setBreatheColor(s.breatheR, s.breatheG, s.breatheB);
  self->setSolidCycleEnabled(s.solidCycleOn);
  self->setSolidCycleMs(s.solidCycleMs);
}

Effects::Effects(HalLed& hal) : _hal(hal) {
  StateManager::instance().subscribe(TOPIC_EFFECT_CHANGE, effectsOnEvent, this);
}

void Effects::setEffect(EffectMode e) { 
  _mode = e; 
  if (e == EFFECT_RAINBOW) _curFx = &_rainbowFx;
  else if (e == EFFECT_BREATHE) _curFx = &_breatheFx;
  else _curFx = nullptr;
}
void Effects::setBreatheColor(uint8_t r, uint8_t g, uint8_t b) { _baseR = r; _baseG = g; _baseB = b; }
void Effects::getBreatheColor(uint8_t& r, uint8_t& g, uint8_t& b) const { r = _baseR; g = _baseG; b = _baseB; }
void Effects::setBreatheHalfMs(uint16_t ms) { if (ms < 200) ms = 200; if (ms > 10000) ms = 10000; _halfMs = ms; }
uint16_t Effects::getBreatheHalfMs() const { return _halfMs; }
void Effects::setSolidCycleEnabled(bool en) { _solidCycle = en; }
bool Effects::getSolidCycleEnabled() const { return _solidCycle; }
void Effects::setSolidCycleMs(uint16_t ms) { if (ms < 200) ms = 200; if (ms > 20000) ms = 20000; _solidMs = ms; }
uint16_t Effects::getSolidCycleMs() const { return _solidMs; }

void Effects::setRainbowSpeed(uint8_t s) { if (s < 1) s = 1; if (s > 255) s = 255; _rainbowSpeed = s; }
uint8_t Effects::getRainbowSpeed() const { return _rainbowSpeed; }
void Effects::setRainbowDirection(bool forward) { _rainbowForward = forward; }
bool Effects::getRainbowDirection() const { return _rainbowForward; }
void Effects::setWipeSpeed(uint16_t s) { if (s < 1) s = 1; if (s > 1000) s = 1000; _wipeSpeed = s; }
uint16_t Effects::getWipeSpeed() const { return _wipeSpeed; }
void Effects::setWipeDirection(bool forward) { _wipeForward = forward; }
bool Effects::getWipeDirection() const { return _wipeForward; }

EffectMode Effects::getEffect() const { return _mode; }
uint32_t Effects::wheel(uint8_t pos) {
  if (pos < 85) return _hal.Color(pos * 3, 255 - pos * 3, 0);
  if (pos < 170) { pos -= 85; return _hal.Color(255 - pos * 3, 0, pos * 3); }
  pos -= 170; return _hal.Color(0, pos * 3, 255 - pos * 3);
}

void Effects::ensurePixels() {
  uint16_t n = _hal.numPixels();
  if (_px && _pxCount == n) return;
  if (_px) { delete[] _px; _px = nullptr; }
  _px = new Px[n];
  _pxCount = n;
  for (uint16_t i = 0; i < n; ++i) { _px[i].r = 255; _px[i].g = 255; _px[i].b = 255; _px[i].br = 0; _px[i].on = false; }
}

uint16_t Effects::getPixelCount() const { return _hal.numPixels(); }

void Effects::setPixel(uint16_t i, uint8_t r, uint8_t g, uint8_t b, uint8_t br, bool on) {
  ensurePixels();
  if (i >= _pxCount) return;
  _px[i].r = r; _px[i].g = g; _px[i].b = b; _px[i].br = br; _px[i].on = on;
}

void Effects::setPixelColor(uint16_t i, uint8_t r, uint8_t g, uint8_t b) {
  ensurePixels();
  if (i >= _pxCount) return;
  _px[i].r = r; _px[i].g = g; _px[i].b = b;
}

void Effects::setPixelBrightness(uint16_t i, uint8_t br) {
  ensurePixels();
  if (i >= _pxCount) return;
  _px[i].br = br;
}

void Effects::setPixelOn(uint16_t i, bool on) {
  ensurePixels();
  if (i >= _pxCount) return;
  _px[i].on = on;
}

void Effects::getPixel(uint16_t i, uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& br, bool& on) const {
  if (!_px || i >= _pxCount) { r = 0; g = 0; b = 0; br = 0; on = false; return; }
  r = _px[i].r; g = _px[i].g; b = _px[i].b; br = _px[i].br; on = _px[i].on;
}

void Effects::stepRainbow() {
  uint16_t n = _hal.numPixels();
  uint8_t off = (uint8_t)((_rainbowOffsetQ4 >> 4) & 0xFF);
  for (uint16_t i = 0; i < n; i++) {
    uint8_t idx = (uint8_t)((i * 256 / n + off) & 255);
    _hal.setPixelColor(i, wheel(idx));
  }
  _hal.show();
  if (_rainbowForward) _rainbowOffsetQ4 = (uint16_t)((_rainbowOffsetQ4 + _rainbowSpeed) & 0x0FFF);
  else _rainbowOffsetQ4 = (uint16_t)((_rainbowOffsetQ4 + 4096 - _rainbowSpeed) & 0x0FFF);
}

void Effects::stepBreathe() {
  float e = 0.5f * (1.0f - cosf(_phase));
  if (_gamma == 3.0f) e = e * e * e; else e = powf(e, _gamma);
  float v = _min + e * (255.0f - _min);
  _breatheVal = (int)v;
  uint8_t r = (uint8_t)((_baseR * _breatheVal) / 255);
  uint8_t g = (uint8_t)((_baseG * _breatheVal) / 255);
  uint8_t b = (uint8_t)((_baseB * _breatheVal) / 255);
  _hal.fillScreen(r, g, b);
  _hal.show();
  _phase += (PI * _lastDt) / _halfMs;
  if (_phase >= 2 * PI) _phase -= 2 * PI;
}

void Effects::stepWipe() {
  uint16_t n = _hal.numPixels();
  _hal.fillScreen(0, 0, 0);
  _hal.setPixelColor(_wipePos, _hal.Color(255, 255, 255));
  _hal.show();
  uint16_t step = _wipeSpeed % n;
  if (step == 0) step = 1;
  if (_wipeForward) _wipePos = (uint16_t)((_wipePos + step) % n);
  else _wipePos = (uint16_t)((_wipePos + n - step) % n);
}

void Effects::stepBreatheColor() {
  float e = 0.5f * (1.0f - cosf(_phase));
  if (_gamma == 3.0f) e = e * e * e; else e = powf(e, _gamma);
  float v = _min + e * (255.0f - _min);
  _breatheVal = (int)v;
  uint8_t idx = _hueOffset;
  _hueOffset++;
  uint32_t c = wheel(idx);
  uint8_t r = (uint8_t)((c >> 16) & 0xFF);
  uint8_t g = (uint8_t)((c >> 8) & 0xFF);
  uint8_t b = (uint8_t)(c & 0xFF);
  r = (uint8_t)((r * _breatheVal) / 255);
  g = (uint8_t)((g * _breatheVal) / 255);
  b = (uint8_t)((b * _breatheVal) / 255);
  _hal.fillScreen(r, g, b);
  _hal.show();
  _phase += (PI * _lastDt) / _halfMs;
  if (_phase >= 2 * PI) _phase -= 2 * PI;
}

void Effects::stepSolid() {
  _hal.fillScreen(_baseR, _baseG, _baseB);
  _hal.show();
}

void Effects::stepSolidCycle() {
  uint8_t idx = (uint8_t)((_solidPhase / (2 * PI)) * 255.0f);
  uint32_t c = wheel(idx);
  uint8_t r = (uint8_t)((c >> 16) & 0xFF);
  uint8_t g = (uint8_t)((c >> 8) & 0xFF);
  uint8_t b = (uint8_t)(c & 0xFF);
  r = (uint8_t)((r + _baseR) / 2);
  g = (uint8_t)((g + _baseG) / 2);
  b = (uint8_t)((b + _baseB) / 2);
  _hal.fillScreen(r, g, b);
  _hal.show();
  _solidPhase += (2 * PI * _lastDt) / _solidMs;
  if (_solidPhase >= 2 * PI) _solidPhase -= 2 * PI;
}

void Effects::stepManual() {
  ensurePixels();
  uint16_t n = _pxCount;
  for (uint16_t i = 0; i < n; ++i) {
    Px& p = _px[i];
    if (!p.on || p.br == 0) { _hal.setPixelColor(i, 0); }
    else {
      uint8_t r = (uint8_t)((p.r * p.br) / 255);
      uint8_t g = (uint8_t)((p.g * p.br) / 255);
      uint8_t b = (uint8_t)((p.b * p.br) / 255);
      _hal.setPixelColor(i, _hal.Color(r, g, b));
    }
  }
  _hal.show();
}

void Effects::update(unsigned long now) {
  if (!_hal.getEnabled()) { _hal.clear(); _hal.show(); return; }
  uint16_t dt = (uint16_t)(now - _last);
  if (dt < _intervalMs) return;
  _last = now;
  _lastDt = dt;
  if (_curFx) { _curFx->update(*this, dt); return; }
  if (_mode == EFFECT_OFF) { _hal.clear(); _hal.show(); }
  else if (_mode == EFFECT_RAINBOW) stepRainbow();
  else if (_mode == EFFECT_BREATHE) stepBreathe();
  else if (_mode == EFFECT_WIPE) stepWipe();
  else if (_mode == EFFECT_BREATHE_COLOR) stepBreatheColor();
  else if (_mode == EFFECT_SOLID) { if (_solidCycle) stepSolidCycle(); else stepSolid(); }
  else if (_mode == EFFECT_MANUAL) stepManual();
}

void Effects::RainbowFx::update(Effects& fx, uint16_t dt) {
  fx._lastDt = dt;
  fx.stepRainbow();
}

void Effects::BreatheFx::update(Effects& fx, uint16_t dt) {
  fx._lastDt = dt;
  fx.stepBreathe();
}
