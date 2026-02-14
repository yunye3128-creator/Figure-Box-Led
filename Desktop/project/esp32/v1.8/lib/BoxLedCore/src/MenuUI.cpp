#include "MenuUI.h"
#include "StateManager.h"
#include <WiFi.h>
#include <Wire.h>
#include <time.h>

static const int8_t _textDy = -2;

static const char STR_BU_GUANG_DENG[] = "补光灯";
static const char STR_CAI_DENG[] = "氛围灯";
static const char STR_LIANG_PING_SHI_JIAN[] = "息屏时间";
static const char STR_IP_DI_ZHI[] = "IP地址";
static const char STR_CHONG_XIN_PEI_WANG[] = "重新配网";
static const char STR_CHANG_JING_YU_SHE[] = "场景预设";
static const char STR_YING_YONG_1[] = "应用1";
static const char STR_BAO_CUN_1[] = "保存1";
static const char STR_YING_YONG_2[] = "应用2";
static const char STR_BAO_CUN_2[] = "保存2";
static const char STR_YING_YONG_3[] = "应用3";
static const char STR_BAO_CUN_3[] = "保存3";
static const char STR_YING_YONG_4[] = "应用4";
static const char STR_BAO_CUN_4[] = "保存4";

static const char STR_SE_WEN_TIAO_JIE[] = "色温";
static const char STR_LIANG_DU_TIAO_JIE[] = "亮度";
static const char STR_KAI_GUAN_KONG_ZHI[] = "开关";

static const char STR_XIAO_GUO_XUAN_ZE[] = "效果";
static const char STR_CAI_HONG_XIAO_GUO[] = "彩虹";
static const char STR_HU_XI_XIAO_GUO[] = "呼吸";
static const char STR_CHANG_LIANG_XIAO_GUO[] = "常亮";
static const char STR_SU_DU_TIAO_JIE[] = "速度";
static const char STR_YAN_SE_TIAO_JIE[] = "颜色";
static const char STR_ZHOU_QI_SHI_JIAN[] = "周期";

const MenuUI::MenuItem MenuUI::_items[] = {
    {"", 0, 1, 6, KIND_MENU, ACT_NONE},
    {STR_BU_GUANG_DENG, 0, 7, 3, KIND_MENU, ACT_NONE},
    {STR_CAI_DENG, 0, 10, 2, KIND_MENU, ACT_NONE},
    {STR_LIANG_PING_SHI_JIAN, 0, 0, 0, KIND_ADJUST, ACT_SCREEN_TIMEOUT},
    {STR_IP_DI_ZHI, 0, 0, 0, KIND_MENU, ACT_INFO_STA_IP},
    {STR_CHONG_XIN_PEI_WANG, 0, 0, 0, KIND_MENU, ACT_WIFI_REPROVISION},
    {STR_CHANG_JING_YU_SHE, 0, 19, 8, KIND_MENU, ACT_NONE},
    {STR_SE_WEN_TIAO_JIE, 1, 0, 0, KIND_ADJUST, ACT_CCT_KELVIN},
    {STR_LIANG_DU_TIAO_JIE, 1, 0, 0, KIND_ADJUST, ACT_CCT_BRIGHTNESS},
    {STR_KAI_GUAN_KONG_ZHI, 1, 0, 0, KIND_TOGGLE, ACT_CCT_TOGGLE},
    {STR_KAI_GUAN_KONG_ZHI, 2, 0, 0, KIND_TOGGLE, ACT_WS_TOGGLE},
    {STR_XIAO_GUO_XUAN_ZE, 2, 12, 3, KIND_MENU, ACT_NONE},
    {STR_CAI_HONG_XIAO_GUO, 11, 15, 1, KIND_MENU, ACT_SET_EFFECT_RAINBOW},
    {STR_HU_XI_XIAO_GUO, 11, 16, 2, KIND_MENU, ACT_SET_EFFECT_BREATHE},
    {STR_CHANG_LIANG_XIAO_GUO, 11, 18, 1, KIND_MENU, ACT_SET_EFFECT_SOLID},
    {STR_SU_DU_TIAO_JIE, 12, 0, 0, KIND_ADJUST, ACT_RAINBOW_SPEED},
    {STR_YAN_SE_TIAO_JIE, 13, 0, 0, KIND_ADJUST, ACT_BREATHE_HUE},
    {STR_ZHOU_QI_SHI_JIAN, 13, 0, 0, KIND_ADJUST, ACT_BREATHE_PERIOD},
    {STR_YAN_SE_TIAO_JIE, 14, 0, 0, KIND_ADJUST, ACT_SOLID_HUE},
    {STR_YING_YONG_1, 6, 0, 0, KIND_MENU, ACT_SCENE_APPLY_1},
    {STR_BAO_CUN_1, 6, 0, 0, KIND_MENU, ACT_SCENE_SAVE_1},
    {STR_YING_YONG_2, 6, 0, 0, KIND_MENU, ACT_SCENE_APPLY_2},
    {STR_BAO_CUN_2, 6, 0, 0, KIND_MENU, ACT_SCENE_SAVE_2},
    {STR_YING_YONG_3, 6, 0, 0, KIND_MENU, ACT_SCENE_APPLY_3},
    {STR_BAO_CUN_3, 6, 0, 0, KIND_MENU, ACT_SCENE_SAVE_3},
    {STR_YING_YONG_4, 6, 0, 0, KIND_MENU, ACT_SCENE_APPLY_4},
    {STR_BAO_CUN_4, 6, 0, 0, KIND_MENU, ACT_SCENE_SAVE_4},
};

MenuUI::MenuUI(HalLed& led, HalInput& input, HalDisplay& display, Effects& effects, WebRouter& web)
    : _led(led),
      _input(input),
      _display(display),
      _effects(effects),
      _web(web) {}

uint8_t MenuUI::currentMenuId() const { return _stack[_depth - 1]; }
const MenuUI::MenuItem& MenuUI::currentMenu() const { return _items[currentMenuId()]; }
uint8_t MenuUI::selectedChildIndex() const { return _sel[_depth - 1]; }

uint8_t MenuUI::selectedChildId() const {
  const MenuItem& m = currentMenu();
  if (m.childCount == 0) return _rootId;
  uint8_t idx = selectedChildIndex();
  if (idx >= m.childCount) idx = 0;
  return (uint8_t)(m.firstChild + idx);
}

const MenuUI::MenuItem& MenuUI::selectedItem() const { return _items[selectedChildId()]; }

void MenuUI::begin() {
  _display.begin();
  
  _lastInteractionMs = millis();
  _lastSyncMs = 0;
  const SystemState& s = StateManager::instance().get();
  _softOff = s.softOff;
  _powerPressHandled = false;
  _lastCt = s.kelvin;
  _lastCctB = s.cctBrightness;
  _lastCctOn = s.cctOn;
  _lastWsB = s.wsBrightness;
  _lastWsOn = s.wsOn;
  
  _lastFx = (uint8_t)s.effect;
  _lastRainbowSpeed = _effects.getRainbowSpeed();
  _lastBreatheHalfMs = s.breatheHalfMs;
  _screenTimeoutSec = s.screenTimeoutSec;
  _screenSaverMode = s.screenSaverMode;
  _lastScreenTimeoutSec = s.screenTimeoutSec;
  _lastScreenSaverMode = s.screenSaverMode;
  
  _lastProvisioning = s.provisioning;
  _lastStaConnected = s.staConnected;
  _lastStaConnecting = s.staConnecting;
  _lastStaFailed = s.staFailed;
  _lastStaFailCode = s.staFailCode;
  strncpy(_lastStaIp, s.staIp, sizeof(_lastStaIp) - 1);
  _lastStaIp[sizeof(_lastStaIp) - 1] = 0;
  _wifiStatusUntilMs = 0;
  
  _dirty = true;
  _asleep = false;
  _display.setPowerSave(false);
  StateManager::instance().subscribe(TOPIC_LIGHT_CHANGE, [](Topic, const SystemState&, void* ctx){ ((MenuUI*)ctx)->_dirty = true; }, this);
  StateManager::instance().subscribe(TOPIC_EFFECT_CHANGE, [](Topic, const SystemState&, void* ctx){ ((MenuUI*)ctx)->_dirty = true; }, this);
  StateManager::instance().subscribe(TOPIC_WIFI_CHANGE, [](Topic, const SystemState&, void* ctx){ ((MenuUI*)ctx)->_dirty = true; }, this);
  StateManager::instance().subscribe(TOPIC_CONFIG_CHANGE, [](Topic, const SystemState&, void* ctx){ ((MenuUI*)ctx)->_dirty = true; }, this);
}

void MenuUI::update(uint32_t nowMs) {
  _input.update(nowMs);
  
  const SystemState& stateNow = StateManager::instance().get();
  bool softOffNow = stateNow.softOff;
  if (softOffNow != _softOff) {
    _softOff = softOffNow;
    if (_softOff) {
      _asleep = true;
      _display.setPowerSave(true);
      _led.setCCTEnabled(false);
      _led.setEnabled(false);
    } else {
      _asleep = false;
      _led.setCCT(stateNow.kelvin, stateNow.cctBrightness);
      _led.setCCTEnabled(stateNow.cctOn);
      _led.setBrightness(stateNow.wsBrightness);
      _led.setEnabled(stateNow.wsOn);
      _display.setPowerSave(false);
      _dirty = true;
    }
  }
  
  bool disableInput = _web.isProvisioning() && _mode == MODE_BROWSE && _depth == 1;
  if (disableInput && !_softOff) {
    _lastInteractionMs = nowMs;
    if (_asleep) {
      _asleep = false;
      _display.setPowerSave(false);
      _dirty = true;
    }
  }
  if (disableInput) {
    _powerPressHandled = false;
  } else {
    bool ignoreInputActions = _asleep && !softOffNow;
    uint32_t okDur = _input.getOkPressDuration(nowMs);
    bool okPressed = _input.isOkPressed();
    if (!ignoreInputActions) {
      if (okPressed) {
        if (!_powerPressHandled) {
          if (softOffNow) {
            if (okDur >= 3000U) {
              StateManager::instance().setSoftOff(false);
              _powerPressHandled = true;
            }
          } else {
            if (okDur >= 4000U) {
              StateManager::instance().setSoftOff(true);
              _powerPressHandled = true;
            }
          }
        }
      } else {
        _powerPressHandled = false;
      }
    } else {
      _powerPressHandled = false;
    }

    InputEvent evt = _input.getEvent();
    if (ignoreInputActions) {
      if (okPressed || evt != EVT_NONE) wakeIfNeeded(nowMs);
      while (evt != EVT_NONE) evt = _input.getEvent();
    } else {
      while (evt != EVT_NONE) {
        if (evt == EVT_BTN_LEFT_SHORT || evt == EVT_BTN_LEFT_LONG || evt == EVT_BTN_LEFT_REPEAT) onLeft();
        else if (evt == EVT_BTN_RIGHT_SHORT || evt == EVT_BTN_RIGHT_LONG || evt == EVT_BTN_RIGHT_REPEAT) onRight();
        else if (evt == EVT_BTN_OK_SHORT) onOkShort();
        else if (evt == EVT_BTN_OK_LONG) onOkLong();
        
        evt = _input.getEvent();
      }
    }
  }

  if (_screenTimeoutSec != 255 && !disableInput) {
    uint32_t toMs = (uint32_t)_screenTimeoutSec * 1000U;
    if (!_asleep && toMs > 0 && (nowMs - _lastInteractionMs) >= toMs) {
      _asleep = true;
      if (_screenSaverMode == SCREEN_OFF || _softOff) {
        _display.setPowerSave(true);
      } else {
        _display.setPowerSave(false);
        _dirty = true;
      }
    }
  }

  if (_depth == 1 && _mode == MODE_BROWSE) {
    uint16_t minuteNow = (uint16_t)((nowMs / 60000UL) % 1440UL);
    if (minuteNow != _lastMinuteShown) {
      _lastMinuteShown = minuteNow;
      _dirty = true;
    }
  }
  if (_asleep && !_softOff && _screenSaverMode != SCREEN_OFF) {
    uint16_t minuteNow = (uint16_t)((nowMs / 60000UL) % 1440UL);
    if (minuteNow != _lastMinuteShown) {
      _lastMinuteShown = minuteNow;
      _dirty = true;
    }
  }

  uint32_t syncEveryMs = (_mode == MODE_ADJUST) ? 120 : 250;
  if ((nowMs - _lastSyncMs) >= syncEveryMs) {
    _lastSyncMs = nowMs;
    bool changed = false;

    const SystemState& s = StateManager::instance().get();
    int ct = s.kelvin;
    int cb = s.cctBrightness;
    bool co = s.cctOn;
    uint8_t wb = s.wsBrightness;
    bool wo = s.wsOn;
    uint8_t fx = (uint8_t)s.effect;
    uint8_t rs = _effects.getRainbowSpeed();
    uint16_t bh = s.breatheHalfMs;

    bool prov = s.provisioning;
    bool sta = s.staConnected;
    bool staConn = s.staConnecting;
    bool staFailed = s.staFailed;
    int staFailCode = s.staFailCode;
    bool staPrev = _lastStaConnected;
    char ip[24]; strncpy(ip, s.staIp, sizeof(ip)); ip[sizeof(ip) - 1] = 0;
    uint8_t st = s.screenTimeoutSec;
    uint8_t sm = s.screenSaverMode;

    if (ct != _lastCt) { _lastCt = ct; changed = true; }
    if (cb != _lastCctB) { _lastCctB = cb; changed = true; }
    if (co != _lastCctOn) { _lastCctOn = co; changed = true; }
    if (wb != _lastWsB) { _lastWsB = wb; changed = true; }
    if (wo != _lastWsOn) { _lastWsOn = wo; changed = true; }
    if (fx != _lastFx) { _lastFx = fx; changed = true; }
    if (rs != _lastRainbowSpeed) { _lastRainbowSpeed = rs; changed = true; }
    if (bh != _lastBreatheHalfMs) { _lastBreatheHalfMs = bh; changed = true; }
    if (st != _lastScreenTimeoutSec) { _lastScreenTimeoutSec = st; _screenTimeoutSec = st; changed = true; }
    if (sm != _lastScreenSaverMode) { _lastScreenSaverMode = sm; _screenSaverMode = sm; changed = true; }
    if (prov != _lastProvisioning) { _lastProvisioning = prov; changed = true; }
    if (sta != _lastStaConnected) { _lastStaConnected = sta; changed = true; }
    if (staConn != _lastStaConnecting) { _lastStaConnecting = staConn; changed = true; }
    if (staFailed != _lastStaFailed) {
      _lastStaFailed = staFailed;
      if (staFailed) _wifiStatusUntilMs = nowMs + 2500U;
      changed = true;
    }
    if (staFailCode != _lastStaFailCode) {
      _lastStaFailCode = staFailCode;
      if (staFailed) _wifiStatusUntilMs = nowMs + 2500U;
      changed = true;
    }
    if (strncmp(ip, _lastStaIp, sizeof(_lastStaIp)) != 0) {
      strncpy(_lastStaIp, ip, sizeof(_lastStaIp) - 1);
      _lastStaIp[sizeof(_lastStaIp) - 1] = 0;
      changed = true;
    }

    if (!staPrev && sta) {
      _mode = MODE_BROWSE;
      _adjustAction = ACT_NONE;
      _adjustTitle = nullptr;
      _adjustDepth = 0;
      _depth = 1;
      _stack[0] = _rootId;
      _sel[0] = 0;
      changed = true;
    }
    if (changed && !_asleep) _dirty = true;
    if (changed && _asleep && !_softOff) {
      if (_screenSaverMode == SCREEN_OFF) {
        _display.setPowerSave(true);
      } else {
        _display.setPowerSave(false);
        _dirty = true;
      }
    }
  }

  if (_dirty) {
    if (_asleep) {
      if (!_softOff && _screenSaverMode != SCREEN_OFF) {
        renderScreenSaver(nowMs);
        _dirty = false;
      }
    } else {
      render(nowMs);
      _dirty = false;
    }
  }
}

void MenuUI::wakeIfNeeded(uint32_t nowMs) {
  _lastInteractionMs = nowMs;
  if (_asleep && !StateManager::instance().get().softOff) {
    _asleep = false;
    _display.setPowerSave(false);
    _dirty = true;
  }
}

void MenuUI::onLeft() {
  wakeIfNeeded(millis());
  if (_mode == MODE_ADJUST) adjustStep(-1);
  else navigate(-1);
  _dirty = true;
}

void MenuUI::onRight() {
  wakeIfNeeded(millis());
  if (_mode == MODE_ADJUST) adjustStep(1);
  else navigate(1);
  _dirty = true;
}

void MenuUI::onOkShort() {
  if (StateManager::instance().get().softOff) return;
  if (_powerPressHandled) return;
  wakeIfNeeded(millis());
  if (_mode == MODE_ADJUST) {
    _mode = MODE_BROWSE;
    _adjustAction = ACT_NONE;
    _adjustTitle = nullptr;
    _adjustDepth = 0;
  } else {
    if (_depth > 1 && currentMenu().childCount == 0) goBack();
    else enterSelected();
  }
  _dirty = true;
}

void MenuUI::onOkLong() {
  if (StateManager::instance().get().softOff) return;
  if (_powerPressHandled) return;
  wakeIfNeeded(millis());
  if (_mode == MODE_ADJUST) {
    _mode = MODE_BROWSE;
    _adjustAction = ACT_NONE;
    _adjustTitle = nullptr;
    _adjustDepth = 0;
  } else {
    goBack();
  }
  _dirty = true;
}

void MenuUI::navigate(int delta) {
  const MenuItem& m = currentMenu();
  if (m.childCount == 0) return;
  int v = (int)selectedChildIndex();
  v += delta;
  while (v < 0) v += m.childCount;
  while (v >= (int)m.childCount) v -= m.childCount;
  _sel[_depth - 1] = (uint8_t)v;
}

void MenuUI::enterSelected() {
  const MenuItem& it = selectedItem();
  if (it.kind == KIND_MENU) {
    applyEnterAction(it.action);
    if (it.action == ACT_WIFI_REPROVISION) return;
    if (it.action == ACT_SCENE_APPLY_1 || it.action == ACT_SCENE_SAVE_1 ||
        it.action == ACT_SCENE_APPLY_2 || it.action == ACT_SCENE_SAVE_2 ||
        it.action == ACT_SCENE_APPLY_3 || it.action == ACT_SCENE_SAVE_3 ||
        it.action == ACT_SCENE_APPLY_4 || it.action == ACT_SCENE_SAVE_4) return;
    if (_depth < 5) {
      _stack[_depth] = selectedChildId();
      _sel[_depth] = 0;
      _depth++;
    }
    return;
  }
  if (it.kind == KIND_TOGGLE) {
    applyToggle(it.action);
    return;
  }
  if (it.kind == KIND_ADJUST) {
    _mode = MODE_ADJUST;
    _adjustAction = it.action;
    _adjustTitle = it.label;
    _adjustDepth = (uint8_t)(_depth + 1);
    return;
  }
}

void MenuUI::goBack() {
  if (_depth > 1) {
    _depth--;
    return;
  }
}

void MenuUI::applyEnterAction(Action a) {
  if (a == ACT_SET_EFFECT_RAINBOW) {
    _led.setEnabled(true);
    StateManager::instance().setEffectMode(EFFECT_RAINBOW);
    _effects.setEffect(EFFECT_RAINBOW);
  } else if (a == ACT_SET_EFFECT_BREATHE) {
    _led.setEnabled(true);
    uint8_t r, g, b;
    hsvToRgb(_breatheHue, r, g, b);
    StateManager::instance().setBreatheColor(r, g, b);
    StateManager::instance().setEffectMode(EFFECT_BREATHE);
    _effects.setBreatheColor(r, g, b);
    _effects.setEffect(EFFECT_BREATHE);
  } else if (a == ACT_SET_EFFECT_SOLID) {
    _led.setEnabled(true);
    uint8_t r, g, b;
    hsvToRgb(_solidHue, r, g, b);
    StateManager::instance().setBreatheColor(r, g, b);
    StateManager::instance().setSolidCycle(false);
    StateManager::instance().setEffectMode(EFFECT_SOLID);
    _effects.setBreatheColor(r, g, b);
    _effects.setSolidCycleEnabled(false);
    _effects.setEffect(EFFECT_SOLID);
  } else if (a == ACT_WIFI_REPROVISION) {
    _web.reprovision();
    _mode = MODE_BROWSE;
    _adjustAction = ACT_NONE;
    _adjustTitle = nullptr;
    _adjustDepth = 0;
    _depth = 1;
    _stack[0] = _rootId;
    _sel[0] = 0;
    _dirty = true;
  } else if (a == ACT_SCENE_APPLY_1) {
    _web.applyScene(1);
    _dirty = true;
  } else if (a == ACT_SCENE_SAVE_1) {
    _web.saveScene(1);
    _dirty = true;
  } else if (a == ACT_SCENE_APPLY_2) {
    _web.applyScene(2);
    _dirty = true;
  } else if (a == ACT_SCENE_SAVE_2) {
    _web.saveScene(2);
    _dirty = true;
  } else if (a == ACT_SCENE_APPLY_3) {
    _web.applyScene(3);
    _dirty = true;
  } else if (a == ACT_SCENE_SAVE_3) {
    _web.saveScene(3);
    _dirty = true;
  } else if (a == ACT_SCENE_APPLY_4) {
    _web.applyScene(4);
    _dirty = true;
  } else if (a == ACT_SCENE_SAVE_4) {
    _web.saveScene(4);
    _dirty = true;
  }
}

void MenuUI::applyToggle(Action a) {
  if (a == ACT_CCT_TOGGLE) {
    _led.setCCTEnabled(!_led.getCCTEnabled());
  } else if (a == ACT_WS_TOGGLE) {
    _led.setEnabled(!_led.getEnabled());
  }
  StateManager::instance().setLight(_led.getKelvin(), _led.getCCTBrightness(), _led.getCCTEnabled(), _led.getBrightness(), _led.getEnabled());
}

int MenuUI::getAdjustValue(Action a) const {
  if (a == ACT_CCT_KELVIN) return _led.getKelvin();
  if (a == ACT_CCT_BRIGHTNESS) return _led.getCCTBrightness();
  if (a == ACT_RAINBOW_SPEED) return (int)_effects.getRainbowSpeed();
  if (a == ACT_BREATHE_HUE) return (int)_breatheHue;
  if (a == ACT_SOLID_HUE) return (int)_solidHue;
  if (a == ACT_BREATHE_PERIOD) return (int)(2 * (int)_effects.getBreatheHalfMs());
  if (a == ACT_SCREEN_TIMEOUT) return (int)_screenTimeoutSec;
  return 0;
}

void MenuUI::setAdjustValue(Action a, int v) {
  if (a == ACT_CCT_KELVIN) {
    _led.setCCT(v, _led.getCCTBrightness());
    StateManager::instance().setLight(_led.getKelvin(), _led.getCCTBrightness(), _led.getCCTEnabled(), _led.getBrightness(), _led.getEnabled());
  }
  else if (a == ACT_CCT_BRIGHTNESS) {
    _led.setCCT(_led.getKelvin(), v);
    StateManager::instance().setLight(_led.getKelvin(), _led.getCCTBrightness(), _led.getCCTEnabled(), _led.getBrightness(), _led.getEnabled());
  }
  else if (a == ACT_RAINBOW_SPEED) {
    _led.setEnabled(true);
    _effects.setEffect(EFFECT_RAINBOW);
    _effects.setRainbowSpeed((uint8_t)v);
  }
  else if (a == ACT_BREATHE_HUE) {
    _led.setEnabled(true);
    _effects.setEffect(EFFECT_BREATHE);
    if (v < 0) v = 0;
    if (v > 359) v = 359;
    _breatheHue = (uint16_t)v;
    uint8_t r, g, b;
    hsvToRgb(_breatheHue, r, g, b);
    StateManager::instance().setBreatheColor(r, g, b);
    _effects.setBreatheColor(r, g, b);
  } else if (a == ACT_SOLID_HUE) {
    if (v < 0) v = 0;
    if (v > 359) v = 359;
    _solidHue = (uint16_t)v;
    uint8_t r, g, b;
    hsvToRgb(_solidHue, r, g, b);
    StateManager::instance().setBreatheColor(r, g, b);
    _effects.setBreatheColor(r, g, b);
    _effects.setSolidCycleEnabled(false);
    _effects.setEffect(EFFECT_SOLID);
  } else if (a == ACT_BREATHE_PERIOD) {
    _led.setEnabled(true);
    _effects.setEffect(EFFECT_BREATHE);
    if (v < 400) v = 400;
    if (v > 20000) v = 20000;
    uint16_t half = (uint16_t)(v / 2);
    StateManager::instance().setBreatheHalfMs(half);
    _effects.setBreatheHalfMs(half);
  } else if (a == ACT_SCREEN_TIMEOUT) {
    if (v < 0) v = 0;
    if (v > 255) v = 255;
    if (v != 255 && v < 5) v = 5;
    _screenTimeoutSec = (uint8_t)v;
    StateManager::instance().setScreenTimeout(_screenTimeoutSec);
  }
}

void MenuUI::getAdjustRange(Action a, int& minV, int& maxV, int& stepV) const {
  if (a == ACT_CCT_KELVIN) { minV = 2700; maxV = 6500; stepV = 100; return; }
  if (a == ACT_CCT_BRIGHTNESS) { minV = 0; maxV = 100; stepV = 1; return; }
  if (a == ACT_RAINBOW_SPEED) { minV = 1; maxV = 80; stepV = 1; return; }
  if (a == ACT_BREATHE_HUE) { minV = 0; maxV = 359; stepV = 5; return; }
  if (a == ACT_SOLID_HUE) { minV = 0; maxV = 359; stepV = 5; return; }
  if (a == ACT_BREATHE_PERIOD) { minV = 400; maxV = 20000; stepV = 100; return; }
  if (a == ACT_SCREEN_TIMEOUT) { minV = 5; maxV = 255; stepV = 5; return; }
  minV = 0; maxV = 100; stepV = 1;
}

void MenuUI::adjustStep(int delta) {
  if (_adjustAction == ACT_SCREEN_TIMEOUT) {
    static const uint8_t opts[] = {5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 255};
    uint8_t cur = _screenTimeoutSec;
    int idx = 0;
    for (int i = 0; i < (int)(sizeof(opts) / sizeof(opts[0])); ++i) {
      if (opts[i] == cur) { idx = i; break; }
    }
    idx += delta;
    int n = (int)(sizeof(opts) / sizeof(opts[0]));
    while (idx < 0) idx += n;
    while (idx >= n) idx -= n;
    setAdjustValue(ACT_SCREEN_TIMEOUT, opts[idx]);
    return;
  }

  int minV, maxV, stepV;
  getAdjustRange(_adjustAction, minV, maxV, stepV);
  int v = getAdjustValue(_adjustAction);
  v += delta * stepV;
  if (v < minV) v = minV;
  if (v > maxV) v = maxV;
  setAdjustValue(_adjustAction, v);
}

void MenuUI::render(uint32_t nowMs) {
  _display.clear();
  if (_web.isProvisioning() && _mode == MODE_BROWSE && _depth == 1) renderProvisioning(nowMs);
  else if (_mode == MODE_ADJUST) renderAdjust();
  else { if (_depth == 1) renderHome(nowMs); else renderMenu(currentMenuId()); }
  _display.display();
}

void MenuUI::renderScreenSaver(uint32_t nowMs) {
  _display.clear();
  uint8_t mode = _screenSaverMode;
  if (mode == SCREEN_ROTATE) {
    uint16_t minuteNow = (uint16_t)((nowMs / 60000UL) % 1440UL);
    mode = (minuteNow % 2 == 0) ? SCREEN_STATUS : SCREEN_TIME;
  }
  if (mode == SCREEN_TIME) {
    renderScreenSaverTime(nowMs);
  } else {
    renderScreenSaverStatus(StateManager::instance().get());
  }
  _display.display();
}

void MenuUI::drawScreenStatusLine(const char* label, const char* percent, const char* rightText, uint8_t yBaseline) {
  _display.setFont(HalDisplay::FONT_LARGE);
  int leftW = _display.getUTF8Width(label);
  int rightW = _display.getUTF8Width(rightText);
  int percentW = _display.getStrWidth(percent);
  int rightX = 128 - rightW;
  if (rightX < 0) rightX = 0;
  int percentX = (128 - percentW) / 2;
  int minX = leftW + 2;
  int maxX = rightX - percentW - 2;
  if (percentX < minX) percentX = minX;
  if (percentX > maxX) percentX = maxX;
  if (percentX < 0) percentX = 0;
  int y = (int)yBaseline + _textDy + 1;
  if (y < 0) y = 0;
  _display.drawUTF8(0, (uint8_t)y, label);
  _display.drawStr((uint8_t)percentX, (uint8_t)y, percent);
  _display.drawUTF8((uint8_t)rightX, (uint8_t)y, rightText);
}

void MenuUI::renderScreenSaverStatus(const SystemState& s) {
  int cctPercent = s.cctOn ? s.cctBrightness : 0;
  if (cctPercent < 0) cctPercent = 0;
  if (cctPercent > 100) cctPercent = 100;
  int wsPercent = s.wsOn ? (int)((s.wsBrightness * 100 + 127) / 255) : 0;
  if (wsPercent < 0) wsPercent = 0;
  if (wsPercent > 100) wsPercent = 100;
  int k = s.kelvin / 100;
  if (k < 0) k = 0;
  if (k > 99) k = 99;
  char p1[8];
  char p2[8];
  char right1[8];
  snprintf(p1, sizeof(p1), "%d%%", cctPercent);
  snprintf(p2, sizeof(p2), "%d%%", wsPercent);
  snprintf(right1, sizeof(right1), "%02dk", k);
  const char* fxText = "关闭";
  if (s.effect == EFFECT_RAINBOW) fxText = "彩虹";
  else if (s.effect == EFFECT_BREATHE) fxText = "呼吸";
  else if (s.effect == EFFECT_BREATHE_COLOR) fxText = "变色";
  else if (s.effect == EFFECT_WIPE) fxText = "跑马";
  else if (s.effect == EFFECT_SOLID) fxText = "常亮";
  else if (s.effect == EFFECT_MANUAL) fxText = "手动";
  drawScreenStatusLine("补光", p1, right1, 15);
  drawScreenStatusLine("氛围", p2, fxText, 31);
}

void MenuUI::renderScreenSaverTime(uint32_t nowMs) {
  uint8_t hh = 0;
  uint8_t mm = 0;
  uint8_t ss = 0;
  uint8_t mon = 1;
  uint8_t day = 1;
  uint8_t wday = 1;
  time_t tnow = time(nullptr);
  if (tnow > 1609459200) {
    struct tm ti;
    localtime_r(&tnow, &ti);
    hh = (uint8_t)ti.tm_hour;
    mm = (uint8_t)ti.tm_min;
    ss = (uint8_t)ti.tm_sec;
    mon = (uint8_t)(ti.tm_mon + 1);
    day = (uint8_t)ti.tm_mday;
    wday = (uint8_t)ti.tm_wday;
  } else {
    uint32_t secs = nowMs / 1000UL;
    hh = (uint8_t)((secs / 3600UL) % 24);
    mm = (uint8_t)((secs / 60UL) % 60);
    ss = (uint8_t)(secs % 60);
    uint32_t days = secs / 86400UL;
    wday = (uint8_t)((days + 1) % 7);
  }
  const char* weekNames[7] = {"周日","周一","周二","周三","周四","周五","周六"};
  const char* week = weekNames[wday % 7];
  char date[6];
  snprintf(date, sizeof(date), "%02u/%02u", mon, day);
  _display.setFont(HalDisplay::FONT_LARGE);
  int y1 = 15 + _textDy;
  if (y1 < 0) y1 = 0;
  _display.drawStr(0, (uint8_t)y1, date);
  int weekW = _display.getUTF8Width(week);
  int weekX = 128 - weekW;
  if (weekX < 0) weekX = 0;
  _display.drawUTF8((uint8_t)weekX, (uint8_t)y1, week);
  char timeLarge[6];
  char timeSmall[4];
  snprintf(timeLarge, sizeof(timeLarge), "%02u:%02u", hh, mm);
  snprintf(timeSmall, sizeof(timeSmall), ":%02u", ss);
  _display.setFont(HalDisplay::FONT_LARGE);
  int largeW = _display.getStrWidth(timeLarge);
  _display.setFont(HalDisplay::FONT_SMALL);
  int smallW = _display.getStrWidth(timeSmall);
  int totalW = largeW + smallW;
  int x = (128 - totalW) / 2;
  if (x < 0) x = 0;
  int y2 = 31 + _textDy;
  if (y2 < 0) y2 = 0;
  _display.setFont(HalDisplay::FONT_LARGE);
  _display.drawStr((uint8_t)x, (uint8_t)y2, timeLarge);
  _display.setFont(HalDisplay::FONT_SMALL);
  _display.drawStr((uint8_t)(x + largeW), (uint8_t)y2, timeSmall);
}

void MenuUI::renderHome(uint32_t nowMs) {
  uint8_t hh = 0;
  uint8_t mm = 0;
  time_t tnow = time(nullptr);
  if (tnow > 1609459200) {
    struct tm ti;
    localtime_r(&tnow, &ti);
    hh = (uint8_t)ti.tm_hour;
    mm = (uint8_t)ti.tm_min;
  } else {
    uint32_t mins = (nowMs / 60000UL) % 1440UL;
    hh = (uint8_t)(mins / 60);
    mm = (uint8_t)(mins % 60);
  }
  char t[6];
  snprintf(t, sizeof(t), "%02u:%02u", hh, mm);

  const MenuItem& m = currentMenu();
  uint8_t idx = selectedChildIndex();
  uint8_t n = m.childCount ? m.childCount : 1;
  char pos[8];
  snprintf(pos, sizeof(pos), "%u/%u", (unsigned)(idx + 1), (unsigned)n);

  _display.setFont(HalDisplay::FONT_SMALL);
  _display.drawStr(0, 8, t);
  _display.drawStr(40, 8, "L1");
  drawRightSmall(pos, 10);

  _display.setFont(HalDisplay::FONT_LARGE);
  const MenuItem& it = selectedItem();
  drawCentered16(it.label, 28);
}

void MenuUI::renderMenu(uint8_t menuId) {
  const MenuItem& menu = _items[menuId];
  char pos[8];
  uint8_t idx = selectedChildIndex();
  uint8_t n = menu.childCount ? menu.childCount : 1;
  snprintf(pos, sizeof(pos), "%u/%u", (unsigned)(idx + 1), (unsigned)n);
  int secondLineOffset = (_depth == 2) ? 1 : 0;

  _display.setFont(HalDisplay::FONT_LARGE);
  drawCentered16(menu.label, 16);

  _display.setFont(HalDisplay::FONT_SMALL);
  char lvl[4];
  snprintf(lvl, sizeof(lvl), "L%u", (unsigned)_depth);
  drawLeftSmall(lvl, 10);
  drawRightSmall(pos, 10);

  if (menu.childCount == 0 && menu.action == ACT_INFO_STA_IP) {
    char ip[24];
    if (_web.isStaConnected()) _web.getStaIp(ip, sizeof(ip));
    else strncpy(ip, "未连接", sizeof(ip));
    ip[sizeof(ip) - 1] = 0;
    _display.setFont(HalDisplay::FONT_SMALL);
    int w2 = _display.getStrWidth(ip);
    int x2 = (128 - w2) / 2;
    if (x2 < 0) x2 = 0;
    int y2 = 30 + _textDy + secondLineOffset;
    if (y2 < 0) y2 = 0;
    _display.drawStr((uint8_t)x2, (uint8_t)y2, ip);
    return;
  }

  const MenuItem& it = selectedItem();
  _display.setFont(HalDisplay::FONT_LARGE);
  drawCentered16(it.label, (uint8_t)(30 + secondLineOffset));
  if (menu.childCount > 1) drawTriangles();
}

void MenuUI::renderProvisioning(uint32_t nowMs) {
  const SystemState& s = StateManager::instance().get();
  if (s.staConnecting) {
    _display.setFont(HalDisplay::FONT_LARGE);
    uint8_t dots = (uint8_t)((nowMs / 400U) % 4U);
    const char* tail = "";
    if (dots == 1) tail = ".";
    else if (dots == 2) tail = "..";
    else if (dots == 3) tail = "...";
    char msg[24];
    snprintf(msg, sizeof(msg), "%s%s", "连接中", tail);
    drawCentered16(msg, 32);
    return;
  }
  if (s.staFailed && nowMs < _wifiStatusUntilMs) {
    _display.setFont(HalDisplay::FONT_LARGE);
#ifdef WL_WRONG_PASSWORD
    const char* msg = (s.staFailCode == WL_WRONG_PASSWORD) ? "密码错误" : "连接失败";
#else
    const char* msg = "连接失败";
#endif
    drawCentered16(msg, 32);
    return;
  }
  char ssid[40];
  char ip[24];
  _web.getApSsid(ssid, sizeof(ssid));
  _web.getApIp(ip, sizeof(ip));
  _display.setFont(HalDisplay::FONT_LARGE);
  int y1 = 14 + _textDy;
  int y2 = 31 + _textDy;
  if (y1 < 0) y1 = 0;
  if (y2 < 0) y2 = 0;
  _display.drawStr(0, (uint8_t)y1, ssid);
  _display.drawStr(0, (uint8_t)y2, ip);
}

void MenuUI::renderAdjust() {
  const char* title = _adjustTitle ? _adjustTitle : "";
  _display.setFont(HalDisplay::FONT_LARGE);
  drawCentered16(title, 16);

  _display.setFont(HalDisplay::FONT_SMALL);
  char lvl[4];
  snprintf(lvl, sizeof(lvl), "L%u", (unsigned)_adjustDepth);
  drawLeftSmall(lvl, 10);

  int v = getAdjustValue(_adjustAction);
  int minV, maxV, stepV;
  getAdjustRange(_adjustAction, minV, maxV, stepV);

  int fill = 0;
  if (_adjustAction == ACT_SCREEN_TIMEOUT) {
    static const uint8_t opts[] = {5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 255};
    int idx = 0;
    int n = (int)(sizeof(opts) / sizeof(opts[0]));
    for (int i = 0; i < n; ++i) { if ((int)opts[i] == v) { idx = i; break; } }
    fill = (int)((idx * 126) / (n - 1));
  } else if (maxV > minV) {
    fill = (int)(((int64_t)(v - minV) * 126) / (maxV - minV));
    if (fill < 0) fill = 0;
    if (fill > 126) fill = 126;
  }

  _display.drawFrame(0, 18, 128, 14);
  if (fill > 0) _display.drawBox(1, 19, (uint8_t)fill, 12);

  char s[24];
  if (_adjustAction == ACT_CCT_KELVIN) snprintf(s, sizeof(s), "%dK", v);
  else if (_adjustAction == ACT_CCT_BRIGHTNESS) snprintf(s, sizeof(s), "%d%%", v);
  else if (_adjustAction == ACT_RAINBOW_SPEED) snprintf(s, sizeof(s), "%d", v);
  else if (_adjustAction == ACT_BREATHE_HUE || _adjustAction == ACT_SOLID_HUE) snprintf(s, sizeof(s), "H:%d", v);
  else if (_adjustAction == ACT_BREATHE_PERIOD) snprintf(s, sizeof(s), "%.1fs", v / 1000.0f);
  else if (_adjustAction == ACT_SCREEN_TIMEOUT) {
    if (v == 255) snprintf(s, sizeof(s), "常亮");
    else snprintf(s, sizeof(s), "%ds", v);
  } else snprintf(s, sizeof(s), "%d", v);

  int tw = _display.getStrWidth(s);
  int tx = (128 - tw) / 2;
  _display.setDrawColor(2);
  _display.drawStr((uint8_t)tx, 30, s);
  _display.setDrawColor(1);
}

void MenuUI::drawCentered16(const char* s, uint8_t yBaseline) {
  int w = _display.getUTF8Width(s);
  int x = (128 - w) / 2;
  if (x < 0) x = 0;
  int y = (int)yBaseline + _textDy;
  if (y < 0) y = 0;
  _display.drawUTF8((uint8_t)x, (uint8_t)y, s);
}

void MenuUI::drawRightSmall(const char* s, uint8_t yBaseline) {
  int w = _display.getStrWidth(s);
  int x = 128 - w;
  if (x < 0) x = 0;
  int y = (int)yBaseline + _textDy;
  if (y < 0) y = 0;
  _display.drawStr((uint8_t)x, (uint8_t)y, s);
}

void MenuUI::drawLeftSmall(const char* s, uint8_t yBaseline) {
  int y = (int)yBaseline + _textDy;
  if (y < 0) y = 0;
  _display.drawStr(0, (uint8_t)y, s);
}

void MenuUI::drawTriangles() {
  _display.drawTriangle(2, 24, 10, 20, 10, 28);
  _display.drawTriangle(125, 24, 117, 20, 117, 28);
}

void MenuUI::hsvToRgb(uint16_t h, uint8_t& r, uint8_t& g, uint8_t& b) {
  uint8_t s = 255;
  uint8_t v = 255;
  uint8_t region, remainder, p, q, t;

  if (s == 0) { r = v; g = v; b = v; return; }

  region = h / 60;
  remainder = (h - (region * 60)) * 255 / 60;

  p = (v * (255 - s)) >> 8;
  q = (v * (255 - ((s * remainder) >> 8))) >> 8;
  t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

  switch (region) {
    case 0: r = v; g = t; b = p; break;
    case 1: r = q; g = v; b = p; break;
    case 2: r = p; g = v; b = t; break;
    case 3: r = p; g = q; b = v; break;
    case 4: r = t; g = p; b = v; break;
    default: r = v; g = p; b = q; break;
  }
}
