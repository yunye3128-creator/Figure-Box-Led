#include "HalInput.h"

HalInput::HalInput(uint8_t pinLeft, uint8_t pinRight, uint8_t pinOk) {
  _btns[0] = {pinLeft, true, true, 0, 0, 0, false, false};
  _btns[1] = {pinRight, true, true, 0, 0, 0, false, false};
  _btns[2] = {pinOk, true, true, 0, 0, 0, false, false};
}

void HalInput::begin() {
  for (int i = 0; i < 3; i++) {
    pinMode(_btns[i].pin, INPUT_PULLUP);
  }
}

void HalInput::pushEvent(InputEvent e) {
  uint8_t next = (_head + 1) % Q_SIZE;
  if (next != _tail) {
    _queue[_head] = e;
    _head = next;
  }
}

InputEvent HalInput::getEvent() {
  if (_head == _tail) return EVT_NONE;
  InputEvent e = _queue[_tail];
  _tail = (_tail + 1) % Q_SIZE;
  return e;
}

bool HalInput::isOkPressed() const {
  return _btns[2].pressed;
}

uint32_t HalInput::getOkPressDuration(uint32_t nowMs) const {
  if (!_btns[2].pressed) return 0;
  return nowMs - _btns[2].pressedAtMs;
}

void HalInput::update(uint32_t nowMs) {
  updateButton(_btns[0], EVT_BTN_LEFT_SHORT, EVT_BTN_LEFT_LONG, EVT_BTN_LEFT_REPEAT, nowMs);
  updateButton(_btns[1], EVT_BTN_RIGHT_SHORT, EVT_BTN_RIGHT_LONG, EVT_BTN_RIGHT_REPEAT, nowMs);
  updateButton(_btns[2], EVT_BTN_OK_SHORT, EVT_BTN_OK_LONG, EVT_NONE, nowMs);
}

void HalInput::updateButton(ButtonState& b, InputEvent shortEvt, InputEvent longEvt, InputEvent repeatEvt, uint32_t nowMs) {
  bool raw = (digitalRead(b.pin) == LOW); // Active LOW

  if (raw != b.lastStable) {
    b.lastStable = raw;
    b.lastChangeMs = nowMs;
  }

  if ((nowMs - b.lastChangeMs) > _debounceMs) {
    if (b.stable != raw) {
      b.stable = raw;
      if (b.stable) {
        b.pressed = true;
        b.pressedAtMs = nowMs;
        b.lastRepeatMs = nowMs;
        b.longFired = false;
      } else {
        if (b.pressed && !b.longFired) {
          pushEvent(shortEvt);
        }
        b.pressed = false;
      }
    }
  }

  if (b.pressed && !b.longFired && (nowMs - b.pressedAtMs > _longPressMs)) {
    b.longFired = true;
    pushEvent(longEvt);
  }

  if (repeatEvt != EVT_NONE && b.pressed) {
    uint32_t sincePress = nowMs - b.pressedAtMs;
    if (sincePress >= _repeatStartMs) {
      if ((nowMs - b.lastRepeatMs) >= _repeatMs) {
        b.lastRepeatMs = nowMs;
        pushEvent(repeatEvt);
      }
    }
  }
}

void HalInput::setTiming(uint32_t debounceMs, uint32_t longPressMs, uint32_t repeatStartMs, uint32_t repeatMs) {
  _debounceMs = debounceMs;
  _longPressMs = longPressMs;
  _repeatStartMs = repeatStartMs;
  _repeatMs = repeatMs;
}
