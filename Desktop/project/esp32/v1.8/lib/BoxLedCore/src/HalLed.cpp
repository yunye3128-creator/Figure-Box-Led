#include "HalLed.h"

HalLed::HalLed(uint16_t numPixels, uint8_t wsPin, uint8_t warmPin, uint8_t coolPin)
  : _strip(numPixels, wsPin, NEO_GRB + NEO_KHZ800),
    _wsPin(wsPin), _warmPin(warmPin), _coolPin(coolPin) {
}

void HalLed::begin() {
  // RGB Init
  _strip.begin();
  _strip.show(); // Initialize all pixels to 'off'
  _strip.setBrightness(128);

  // CCT Init
  ledcSetup(_chWarm, _freq, _resBits);
  ledcSetup(_chCool, _freq, _resBits);
  ledcAttachPin(_warmPin, _chWarm);
  ledcAttachPin(_coolPin, _chCool);
  updateCCT();
}

void HalLed::setRGB(uint16_t index, uint8_t r, uint8_t g, uint8_t b) {
  _strip.setPixelColor(index, r, g, b);
}

void HalLed::setPixelColor(uint16_t index, uint32_t color) {
  _strip.setPixelColor(index, color);
}

void HalLed::fillScreen(uint8_t r, uint8_t g, uint8_t b) {
  _strip.fill(Color(r, g, b));
}

void HalLed::show() {
  if (_rgbEnabled) {
    _strip.show();
  } else {
    // If disabled, we might want to clear, but usually we just don't show new data 
    // or we show black. The original LedMatrix clear() and show() on disable.
    // Here we assume logic layer handles clearing if needed, or we clear on disable.
    // Original code: setEnabled(false) -> clear(), show().
  }
}

void HalLed::clear() {
  _strip.clear();
}

void HalLed::setBrightness(uint8_t b) {
  _strip.setBrightness(b);
}

uint8_t HalLed::getBrightness() const {
  return _strip.getBrightness();
}

void HalLed::setEnabled(bool en) {
  _rgbEnabled = en;
  if (!_rgbEnabled) {
    _strip.clear();
    _strip.show();
  }
}

bool HalLed::getEnabled() const {
  return _rgbEnabled;
}

uint16_t HalLed::numPixels() const {
  return _strip.numPixels();
}

uint32_t HalLed::Color(uint8_t r, uint8_t g, uint8_t b) {
  return _strip.Color(r, g, b);
}

// CCT Implementation

void HalLed::setCCT(int kelvin, int brightnessPercent) {
  if (kelvin < _minK) kelvin = _minK;
  if (kelvin > _maxK) kelvin = _maxK;
  if (brightnessPercent < 0) brightnessPercent = 0;
  if (brightnessPercent > 100) brightnessPercent = 100;

  _kelvin = kelvin;
  _cctBrightness = brightnessPercent;
  updateCCT();
}

void HalLed::setCCTEnabled(bool enabled) {
  _cctEnabled = enabled;
  updateCCT();
}

int HalLed::getKelvin() const {
  return _kelvin;
}

int HalLed::getCCTBrightness() const {
  return _cctBrightness;
}

bool HalLed::getCCTEnabled() const {
  return _cctEnabled;
}

void HalLed::updateCCT() {
  int dutyWarm = 0;
  int dutyCool = 0;
  
  if (_cctEnabled) {
    float B = _cctBrightness / 100.0f;
    float t = (float)(_kelvin - _minK) / (float)(_maxK - _minK);
    if (t < 0) t = 0;
    if (t > 1) t = 1;
    
    // Simple linear mix
    float wWarm = 1.0f - t;
    float wCool = t;
    
    // Normalize so max channel is at B
    // Wait, original logic:
    // float wMax = (wWarm > wCool) ? wWarm : wCool;
    // float warm = (wMax > 0.0f) ? (B * (wWarm / wMax)) : 0.0f;
    // float cool = (wMax > 0.0f) ? (B * (wCool / wMax)) : 0.0f;
    // This logic maintains constant brightness by boosting the weaker channel? 
    // No, wWarm/wMax means one channel is always 1.0 (before B scaling).
    // This maintains constant LUX? Maybe. I'll copy exact logic.
    
    float wMax = (wWarm > wCool) ? wWarm : wCool;
    float warm = 0.0f;
    float cool = 0.0f;
    if (wMax > 0.0001f) {
       warm = B * (wWarm / wMax);
       cool = B * (wCool / wMax);
    }
    
    int maxDuty = (1 << _resBits) - 1;
    dutyWarm = (int)(warm * maxDuty + 0.5f);
    dutyCool = (int)(cool * maxDuty + 0.5f);
  }
  
  ledcWrite(_chWarm, dutyWarm);
  ledcWrite(_chCool, dutyCool);
}
