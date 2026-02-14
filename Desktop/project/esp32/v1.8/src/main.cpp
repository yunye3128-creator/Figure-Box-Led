#include "HalLed.h"
#include "HalInput.h"
#include "HalDisplay.h"
#include "Effects.h"
#include "StateManager.h"
#include "WebRouter.h"
#include "MenuUI.h"
#include <Arduino.h>

const uint8_t WS_PIN = 0;
const uint8_t WARM_PIN = 1;
const uint8_t COOL_PIN = 2;
const uint8_t BTN_LEFT_PIN = 4;
const uint8_t BTN_RIGHT_PIN = 5;
const uint8_t BTN_OK_PIN = 6;
const uint16_t NUM_PIXELS = 47;

HalLed led(NUM_PIXELS, WS_PIN, WARM_PIN, COOL_PIN);
Effects effects(led);
WebRouter web(led, effects);
HalInput input(BTN_LEFT_PIN, BTN_RIGHT_PIN, BTN_OK_PIN);
HalDisplay display;
MenuUI menu(led, input, display, effects, web);

static void applySystemStateToHardware(const SystemState& s) {
  led.setCCT(s.kelvin, s.cctBrightness);
  led.setCCTEnabled(s.cctOn);
  led.setBrightness(s.wsBrightness);
  led.setEnabled(s.wsOn);
  effects.setBreatheColor(s.breatheR, s.breatheG, s.breatheB);
  effects.setBreatheHalfMs(s.breatheHalfMs);
  effects.setSolidCycleEnabled(s.solidCycleOn);
  effects.setSolidCycleMs(s.solidCycleMs);
  effects.setEffect(s.effect);
}

#ifdef UNIT_TEST
void setup() __attribute__((weak));
void loop() __attribute__((weak));
void setup() {}
void loop() {}
#else
void setup() {
  led.begin();
  StateManager& sm = StateManager::instance();
  sm.begin();
  applySystemStateToHardware(sm.get());
  web.begin("ShowBox", "");
  input.begin();
  input.setTiming(25, 500, 450, 100);
  sm.setNumPixels(led.numPixels(), false);
  sm.setLight(led.getKelvin(), led.getCCTBrightness(), led.getCCTEnabled(), led.getBrightness(), led.getEnabled(), true);
  uint8_t br, bg, bb; effects.getBreatheColor(br, bg, bb);
  sm.setBreatheColor(br, bg, bb, false);
  sm.setBreatheHalfMs(effects.getBreatheHalfMs(), false);
  sm.setSolidCycle(effects.getSolidCycleEnabled(), false);
  sm.setSolidCycleMs(effects.getSolidCycleMs(), false);
  sm.setEffectMode(effects.getEffect(), true);
  menu.begin();
}

void loop() {
  web.handle();
  effects.update(millis());
  menu.update(millis());
  StateManager::instance().loop(millis());
}
#endif
