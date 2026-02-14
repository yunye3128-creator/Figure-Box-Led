#include "StateManager.h"
static StateManager* gSM = nullptr;
StateManager& StateManager::instance() {
  if (!gSM) gSM = new StateManager();
  return *gSM;
}
#
StateManager::StateManager() {}
void StateManager::begin() {
  _prefs.begin("sysstate", false);
  load();
}
#
const SystemState& StateManager::get() const { return _s; }
void StateManager::markDirty() {
  _dirty = true;
  _lastChangeMs = millis();
}
#
void StateManager::loop(uint32_t nowMs) {
  if (_dirty && (nowMs - _lastChangeMs) >= _saveDelayMs) {
    save();
    _dirty = false;
  }
}
void StateManager::load() {
  _s.kelvin = _prefs.getInt("kelvin", _s.kelvin);
  _s.cctBrightness = _prefs.getInt("cctB", _s.cctBrightness);
  _s.cctOn = _prefs.getBool("cctOn", _s.cctOn);
  _s.wsBrightness = _prefs.getUChar("wsB", _s.wsBrightness);
  _s.wsOn = _prefs.getBool("wsOn", _s.wsOn);
  _s.effect = (EffectMode)_prefs.getUChar("fx", (uint8_t)_s.effect);
  _s.breatheR = _prefs.getUChar("br", _s.breatheR);
  _s.breatheG = _prefs.getUChar("bg", _s.breatheG);
  _s.breatheB = _prefs.getUChar("bb", _s.breatheB);
  _s.breatheHalfMs = _prefs.getUShort("bh", _s.breatheHalfMs);
  _s.solidCycleOn = _prefs.getBool("scOn", _s.solidCycleOn);
  _s.solidCycleMs = _prefs.getUShort("scMs", _s.solidCycleMs);
  _s.screenTimeoutSec = _prefs.getUChar("scrTo", _s.screenTimeoutSec);
  _s.screenSaverMode = _prefs.getUChar("ssm", _s.screenSaverMode);
}
void StateManager::save() {
  _prefs.putInt("kelvin", _s.kelvin);
  _prefs.putInt("cctB", _s.cctBrightness);
  _prefs.putBool("cctOn", _s.cctOn);
  _prefs.putUChar("wsB", _s.wsBrightness);
  _prefs.putBool("wsOn", _s.wsOn);
  _prefs.putUChar("fx", (uint8_t)_s.effect);
  _prefs.putUChar("br", _s.breatheR);
  _prefs.putUChar("bg", _s.breatheG);
  _prefs.putUChar("bb", _s.breatheB);
  _prefs.putUShort("bh", _s.breatheHalfMs);
  _prefs.putBool("scOn", _s.solidCycleOn);
  _prefs.putUShort("scMs", _s.solidCycleMs);
  _prefs.putUChar("scrTo", _s.screenTimeoutSec);
  _prefs.putUChar("ssm", _s.screenSaverMode);
}
void StateManager::publish(Topic topic) {
  for (uint8_t i = 0; i < _subCount; ++i) {
    const Sub& s = _subs[i];
    if (s.cb && s.topic == topic) s.cb(topic, _s, s.ctx);
  }
}
bool StateManager::subscribe(Topic topic, EventCallback cb, void* ctx) {
  if (_subCount >= MAX_SUBS) return false;
  _subs[_subCount++] = Sub{topic, cb, ctx};
  return true;
}
void StateManager::unsubscribe(EventCallback cb, void* ctx) {
  for (uint8_t i = 0; i < _subCount; ++i) {
    if (_subs[i].cb == cb && _subs[i].ctx == ctx) {
      for (uint8_t j = i + 1; j < _subCount; ++j) _subs[j - 1] = _subs[j];
      _subCount--;
      return;
    }
  }
}
void StateManager::setNumPixels(uint16_t n, bool doPublish) {
  _s.numPixels = n;
  if (doPublish) this->publish(TOPIC_CONFIG_CHANGE);
}
void StateManager::setLight(int kelvin, int cctBrightness, bool cctOn, uint8_t wsBrightness, bool wsOn, bool doPublish) {
  _s.kelvin = kelvin;
  _s.cctBrightness = cctBrightness;
  _s.cctOn = cctOn;
  _s.wsBrightness = wsBrightness;
  _s.wsOn = wsOn;
  markDirty();
  if (doPublish) this->publish(TOPIC_LIGHT_CHANGE);
}
void StateManager::setEffectMode(EffectMode m, bool doPublish) {
  _s.effect = m;
  markDirty();
  if (doPublish) this->publish(TOPIC_EFFECT_CHANGE);
}
void StateManager::setBreatheColor(uint8_t r, uint8_t g, uint8_t b, bool doPublish) {
  _s.breatheR = r; _s.breatheG = g; _s.breatheB = b;
  markDirty();
  if (doPublish) this->publish(TOPIC_EFFECT_CHANGE);
}
void StateManager::setBreatheHalfMs(uint16_t ms, bool doPublish) {
  _s.breatheHalfMs = ms;
  markDirty();
  if (doPublish) this->publish(TOPIC_EFFECT_CHANGE);
}
void StateManager::setSolidCycle(bool on, bool doPublish) {
  _s.solidCycleOn = on;
  markDirty();
  if (doPublish) this->publish(TOPIC_EFFECT_CHANGE);
}
void StateManager::setSolidCycleMs(uint16_t ms, bool doPublish) {
  _s.solidCycleMs = ms;
  markDirty();
  if (doPublish) this->publish(TOPIC_EFFECT_CHANGE);
}
void StateManager::setWifiProvisioning(bool prov, bool doPublish) {
  _s.provisioning = prov;
  if (doPublish) this->publish(TOPIC_WIFI_CHANGE);
}
void StateManager::setStaConnected(bool connected, const char* ip, bool doPublish) {
  _s.staConnected = connected;
  if (ip) {
    strncpy(_s.staIp, ip, sizeof(_s.staIp) - 1);
    _s.staIp[sizeof(_s.staIp) - 1] = 0;
  } else {
    _s.staIp[0] = 0;
  }
  if (doPublish) this->publish(TOPIC_WIFI_CHANGE);
}
void StateManager::setStaConnecting(bool connecting, bool doPublish) {
  _s.staConnecting = connecting;
  if (doPublish) this->publish(TOPIC_WIFI_CHANGE);
}
void StateManager::setStaFail(int failCode, bool failed, bool doPublish) {
  _s.staFailCode = failCode;
  _s.staFailed = failed;
  if (doPublish) this->publish(TOPIC_WIFI_CHANGE);
}
void StateManager::setApInfo(const char* ssid, const char* ip) {
  if (ssid) { strncpy(_s.apSsid, ssid, sizeof(_s.apSsid) - 1); _s.apSsid[sizeof(_s.apSsid) - 1] = 0; }
  if (ip) { strncpy(_s.apIp, ip, sizeof(_s.apIp) - 1); _s.apIp[sizeof(_s.apIp) - 1] = 0; }
}
void StateManager::setScreenTimeout(uint8_t sec, bool doPublish) {
  _s.screenTimeoutSec = sec;
  markDirty();
  if (doPublish) this->publish(TOPIC_CONFIG_CHANGE);
}
void StateManager::setScreenSaverMode(uint8_t mode, bool doPublish) {
  _s.screenSaverMode = mode;
  markDirty();
  if (doPublish) this->publish(TOPIC_CONFIG_CHANGE);
}

void StateManager::setSoftOff(bool off, bool doPublish) {
  _s.softOff = off;
  markDirty();
  if (doPublish) this->publish(TOPIC_CONFIG_CHANGE);
}
