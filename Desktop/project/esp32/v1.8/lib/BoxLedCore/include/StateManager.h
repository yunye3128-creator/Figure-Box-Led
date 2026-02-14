#pragma once
#include <Arduino.h>
#include <Preferences.h>
#include <stdint.h>
#include "Effects.h"
enum Topic {
  TOPIC_LIGHT_CHANGE = 1,
  TOPIC_EFFECT_CHANGE = 2,
  TOPIC_WIFI_CHANGE = 3,
  TOPIC_CONFIG_CHANGE = 4,
  TOPIC_SCENE_CHANGE = 5
};
struct SystemState {
  int kelvin = 4000;
  int cctBrightness = 50;
  bool cctOn = true;
  uint8_t wsBrightness = 128;
  bool wsOn = true;
  EffectMode effect = EFFECT_RAINBOW;
  uint8_t breatheR = 255, breatheG = 255, breatheB = 255;
  uint16_t breatheHalfMs = 2000;
  bool solidCycleOn = false;
  uint16_t solidCycleMs = 4000;
  uint16_t numPixels = 0;
  //
  bool provisioning = true;
  bool staConnected = false;
  bool staConnecting = false;
  bool staFailed = false;
  int staFailCode = 0;
  char staIp[24] = {0};
  char apSsid[40] = {0};
  char apIp[24] = {0};
  //
  uint8_t screenTimeoutSec = 60;
  uint8_t screenSaverMode = 2;
  bool softOff = false;
};
typedef void (*EventCallback)(Topic topic, const SystemState& s, void* ctx);
class StateManager {
public:
  /**
   * @brief 获取状态管理器单例。
   * @return StateManager& 单例引用。
   * @note 首次调用会创建实例。
   * @throws 无异常抛出。
   */
  static StateManager& instance();
  /**
   * @brief 初始化状态管理器与持久化存储。
   * @return 无。
   * @note 会从 Preferences 载入持久化状态。
   * @throws 无异常抛出。
   */
  void begin();
  /**
   * @brief 状态管理器循环处理。
   * @param nowMs uint32_t 当前时间戳（毫秒）。
   * @return 无。
   * @note 用于延时保存配置。
   * @throws 无异常抛出。
   */
  void loop(uint32_t nowMs);
  /**
   * @brief 获取当前系统状态快照。
   * @return const SystemState& 系统状态引用。
   * @note 返回的是内部状态引用，不可修改。
   * @throws 无异常抛出。
   */
  const SystemState& get() const;
  /**
   * @brief 设置像素数量。
   * @param n uint16_t 像素数量。
   * @param doPublish bool 是否发布配置变更事件。
   * @return 无。
   * @note 不触发持久化保存。
   * @throws 无异常抛出。
   */
  void setNumPixels(uint16_t n, bool doPublish = false);
  /**
   * @brief 设置灯光相关状态。
   * @param kelvin int 色温（Kelvin）。
   * @param cctBrightness int 双色温亮度百分比（0-100）。
   * @param cctOn bool 双色温开关状态。
   * @param wsBrightness uint8_t 彩灯亮度（0-255）。
   * @param wsOn bool 彩灯开关状态。
   * @param doPublish bool 是否发布灯光变更事件。
   * @return 无。
   * @note 会标记为待保存。
   * @throws 无异常抛出。
   */
  void setLight(int kelvin, int cctBrightness, bool cctOn, uint8_t wsBrightness, bool wsOn, bool doPublish = true);
  /**
   * @brief 设置当前效果模式。
   * @param m EffectMode 效果模式枚举。
   * @param doPublish bool 是否发布效果变更事件。
   * @return 无。
   * @note 会标记为待保存。
   * @throws 无异常抛出。
   */
  void setEffectMode(EffectMode m, bool doPublish = true);
  /**
   * @brief 设置呼吸/常亮基准颜色。
   * @param r uint8_t 红色通道值（0-255）。
   * @param g uint8_t 绿色通道值（0-255）。
   * @param b uint8_t 蓝色通道值（0-255）。
   * @param doPublish bool 是否发布效果变更事件。
   * @return 无。
   * @note 会标记为待保存。
   * @throws 无异常抛出。
   */
  void setBreatheColor(uint8_t r, uint8_t g, uint8_t b, bool doPublish = true);
  /**
   * @brief 设置呼吸半周期时长。
   * @param ms uint16_t 半周期时间（毫秒）。
   * @param doPublish bool 是否发布效果变更事件。
   * @return 无。
   * @note 会标记为待保存。
   * @throws 无异常抛出。
   */
  void setBreatheHalfMs(uint16_t ms, bool doPublish = true);
  /**
   * @brief 设置常亮颜色自动轮换开关。
   * @param on bool true 开启，false 关闭。
   * @param doPublish bool 是否发布效果变更事件。
   * @return 无。
   * @note 会标记为待保存。
   * @throws 无异常抛出。
   */
  void setSolidCycle(bool on, bool doPublish = true);
  /**
   * @brief 设置常亮颜色轮换周期。
   * @param ms uint16_t 周期毫秒数。
   * @param doPublish bool 是否发布效果变更事件。
   * @return 无。
   * @note 会标记为待保存。
   * @throws 无异常抛出。
   */
  void setSolidCycleMs(uint16_t ms, bool doPublish = true);
  /**
   * @brief 设置配网状态。
   * @param prov bool true 表示处于配网中，false 表示已完成。
   * @param doPublish bool 是否发布 WiFi 变更事件。
   * @return 无。
   * @note 不触发持久化保存。
   * @throws 无异常抛出。
   */
  void setWifiProvisioning(bool prov, bool doPublish = true);
  /**
   * @brief 设置 STA 连接状态与 IP。
   * @param connected bool 是否已连接。
   * @param ip const char* IP 字符串，传入 nullptr 表示清空。
   * @param doPublish bool 是否发布 WiFi 变更事件。
   * @return 无。
   * @note 不触发持久化保存。
   * @throws 无异常抛出。
   */
  void setStaConnected(bool connected, const char* ip, bool doPublish = true);
  void setStaConnecting(bool connecting, bool doPublish = true);
  void setStaFail(int failCode, bool failed, bool doPublish = true);
  /**
   * @brief 设置 AP 端口信息。
   * @param ssid const char* AP SSID 字符串。
   * @param ip const char* AP IP 字符串。
   * @return 无。
   * @note 不触发事件或持久化保存。
   * @throws 无异常抛出。
   */
  void setApInfo(const char* ssid, const char* ip);
  /**
   * @brief 设置屏幕超时秒数。
   * @param sec uint8_t 超时时长（秒），255 表示常亮。
   * @param doPublish bool 是否发布配置变更事件。
   * @return 无。
   * @note 会标记为待保存。
   * @throws 无异常抛出。
   */
  void setScreenTimeout(uint8_t sec, bool doPublish = true);
  void setScreenSaverMode(uint8_t mode, bool doPublish = true);
  /**
   * @brief 设置软关灯状态。
   * @param off bool true 进入软关灯，false 退出软关灯。
   * @param doPublish bool 是否发布配置变更事件。
   * @return 无。
   * @note 会标记为待保存。
   * @throws 无异常抛出。
   */
  void setSoftOff(bool off, bool doPublish = true);
  /**
   * @brief 订阅系统状态变更事件。
   * @param topic Topic 事件主题枚举。
   * @param cb EventCallback 回调函数指针。
   * @param ctx void* 回调上下文指针。
   * @return bool 成功返回 true，订阅满时返回 false。
   * @note 调用方需保证回调函数有效。
   * @throws 无异常抛出。
   */
  bool subscribe(Topic topic, EventCallback cb, void* ctx);
  /**
   * @brief 取消指定回调的订阅。
   * @param cb EventCallback 回调函数指针。
   * @param ctx void* 回调上下文指针。
   * @return 无。
   * @note 会移除所有匹配的订阅项。
   * @throws 无异常抛出。
   */
  void unsubscribe(EventCallback cb, void* ctx);
private:
  /**
   * @brief 私有构造函数，防止外部创建实例。
   * @return 无。
   * @note 通过 instance() 获取单例。
   * @throws 无异常抛出。
   */
  StateManager();
  Preferences _prefs;
  SystemState _s;
  struct Sub { Topic topic; EventCallback cb; void* ctx; };
  static const uint8_t MAX_SUBS = 12;
  Sub _subs[MAX_SUBS];
  uint8_t _subCount = 0;
  //
  bool _dirty = false;
  uint32_t _lastChangeMs = 0;
  uint32_t _saveDelayMs = 5000;
  //
  /**
   * @brief 标记状态为脏并记录变更时间。
   * @return 无。
   * @note 用于延迟保存。
   * @throws 无异常抛出。
   */
  void markDirty();
  /**
   * @brief 从持久化存储加载状态。
   * @return 无。
   * @note 未保存的字段保持默认值。
   * @throws 无异常抛出。
   */
  void load();
  /**
   * @brief 将当前状态保存到持久化存储。
   * @return 无。
   * @note 会写入 Preferences。
   * @throws 无异常抛出。
   */
  void save();
  /**
   * @brief 发布状态变更事件。
   * @param topic Topic 事件主题枚举。
   * @return 无。
   * @note 仅通知已订阅的回调。
   * @throws 无异常抛出。
   */
  void publish(Topic topic);
};
