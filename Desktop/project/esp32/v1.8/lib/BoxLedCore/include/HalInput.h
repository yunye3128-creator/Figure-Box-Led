#pragma once
#include <Arduino.h>

enum InputEvent {
  EVT_NONE = 0,
  EVT_BTN_LEFT_SHORT,
  EVT_BTN_LEFT_LONG,
  EVT_BTN_LEFT_REPEAT,
  EVT_BTN_RIGHT_SHORT,
  EVT_BTN_RIGHT_LONG,
  EVT_BTN_RIGHT_REPEAT,
  EVT_BTN_OK_SHORT,
  EVT_BTN_OK_LONG
};

class HalInput {
public:
  /**
   * @brief 构造按键输入处理对象。
   * @param pinLeft uint8_t 左键 GPIO 引脚号。
   * @param pinRight uint8_t 右键 GPIO 引脚号。
   * @param pinOk uint8_t 确认键 GPIO 引脚号。
   * @return 无。
   * @note 仅保存引脚配置，需调用 begin() 初始化。
   * @throws 无异常抛出。
   */
  HalInput(uint8_t pinLeft, uint8_t pinRight, uint8_t pinOk);
  /**
   * @brief 初始化按键引脚为上拉输入。
   * @return 无。
   * @note 采用 INPUT_PULLUP，按键为低电平有效。
   * @throws 无异常抛出。
   */
  void begin();
  /**
   * @brief 更新按键状态并生成事件。
   * @param nowMs uint32_t 当前时间戳（毫秒），通常为 millis()。
   * @return 无。
   * @note 需要周期性调用以产生短按、长按与连发事件。
   * @throws 无异常抛出。
   */
  void update(uint32_t nowMs);
  /**
   * @brief 读取队列中的一个按键事件。
   * @return InputEvent 返回事件类型，无事件时返回 EVT_NONE。
   * @note 每次调用会出队一个事件。
   * @throws 无异常抛出。
   */
  InputEvent getEvent();
  /**
   * @brief 设置按键消抖与长按/连发时序参数。
   * @param debounceMs uint32_t 消抖时间（毫秒）。
   * @param longPressMs uint32_t 长按判定时间（毫秒）。
   * @param repeatStartMs uint32_t 连发起始延迟（毫秒）。
   * @param repeatMs uint32_t 连发间隔（毫秒）。
   * @return 无。
   * @note 参数用于 update() 的按键判定逻辑。
   * @throws 无异常抛出。
   */
  void setTiming(uint32_t debounceMs, uint32_t longPressMs, uint32_t repeatStartMs, uint32_t repeatMs);
  /**
   * @brief 判断确认键当前是否处于按下状态。
   * @return bool true 表示按下，false 表示未按下。
   * @note 基于内部稳定状态计算。
   * @throws 无异常抛出。
   */
  bool isOkPressed() const;
  /**
   * @brief 获取确认键按下持续时长。
   * @param nowMs uint32_t 当前时间戳（毫秒），通常为 millis()。
   * @return uint32_t 按下持续毫秒数，未按下时返回 0。
   * @note 按下时间基于最近一次按下时刻。
   * @throws 无异常抛出。
   */
  uint32_t getOkPressDuration(uint32_t nowMs) const;

private:
  struct ButtonState {
    uint8_t pin;
    bool stable;
    bool lastStable;
    uint32_t lastChangeMs;
    uint32_t pressedAtMs;
    uint32_t lastRepeatMs;
    bool pressed;
    bool longFired;
  };

  ButtonState _btns[3];
  
  /**
   * @brief 事件队列长度常量。
   * @note 使用环形缓冲区保存按键事件。
   */
  static const uint8_t Q_SIZE = 16;
  InputEvent _queue[Q_SIZE];
  uint8_t _head = 0;
  uint8_t _tail = 0;

  uint32_t _debounceMs = 20;
  uint32_t _longPressMs = 800;
  uint32_t _repeatStartMs = 500;
  uint32_t _repeatMs = 120;

  /**
   * @brief 将事件压入队列。
   * @param e InputEvent 需要入队的事件类型。
   * @return 无。
   * @note 队列满时将丢弃新事件。
   * @throws 无异常抛出。
   */
  void pushEvent(InputEvent e);
  /**
   * @brief 更新单个按键的状态机并产生事件。
   * @param b ButtonState& 按键状态结构体引用。
   * @param shortEvt InputEvent 短按事件类型。
   * @param longEvt InputEvent 长按事件类型。
   * @param repeatEvt InputEvent 连发事件类型，若不需要连发请传 EVT_NONE。
   * @param nowMs uint32_t 当前时间戳（毫秒）。
   * @return 无。
   * @note 结合消抖与长按时序参数产生事件。
   * @throws 无异常抛出。
   */
  void updateButton(ButtonState& b, InputEvent shortEvt, InputEvent longEvt, InputEvent repeatEvt, uint32_t nowMs);
};
