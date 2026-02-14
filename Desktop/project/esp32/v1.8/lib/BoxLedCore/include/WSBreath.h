#pragma once
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
/*
  WS2812 呼吸光效控制类
  - 支持可调参数：呼吸周期（默认2秒，指半周期：最低->最高）、亮度范围（0-255）、颜色值（默认白色）
  - 非线性亮度曲线：余弦缓动 + Gamma 校正，视觉更自然
  - 10ms 固定更新节拍，保证平滑
  - 简单安全的接口与边界检查
*/
class WSBreath {
 public:
  /**
   * @brief 构造 WS2812 呼吸效果控制器。
   * @param count uint16_t 像素数量。
   * @param pin uint8_t WS2812 数据引脚。
   * @param type neoPixelType 像素类型与时序配置。
   * @return 无。
   * @note 仅保存配置，需调用 begin() 初始化硬件。
   * @throws 无异常抛出。
   */
  WSBreath(uint16_t count, uint8_t pin, neoPixelType type = NEO_GRB + NEO_KHZ800);
  /**
   * @brief 初始化灯带并设置初始亮度。
   * @param initialBrightness uint8_t 初始亮度（0-255）。
   * @return bool 初始化成功返回 true，像素数为 0 时返回 false。
   * @note 调用后才会允许 update() 输出。
   * @throws 无异常抛出。
   */
  bool begin(uint8_t initialBrightness = 128);
  /**
   * @brief 更新呼吸效果并输出到灯带。
   * @param now unsigned long 当前时间戳（毫秒），通常为 millis()。
   * @return 无。
   * @note 需周期性调用，未初始化或禁用时会直接返回。
   * @throws 无异常抛出。
   */
  void update(unsigned long now);
  /**
   * @brief 设置呼吸半周期时长。
   * @param halfPeriodMs uint16_t 半周期时间（毫秒），范围会被限制在 200~20000。
   * @return bool 总是返回 true。
   * @note 输入超出范围会被自动夹紧。
   * @throws 无异常抛出。
   */
  bool setPeriodMs(uint16_t halfPeriodMs);
  /**
   * @brief 设置呼吸亮度范围。
   * @param minVal uint8_t 最小亮度（0-255）。
   * @param maxVal uint8_t 最大亮度（0-255）。
   * @return bool 总是返回 true。
   * @note 若 minVal > maxVal 将自动交换。
   * @throws 无异常抛出。
   */
  bool setRange(uint8_t minVal, uint8_t maxVal);
  /**
   * @brief 设置呼吸颜色。
   * @param r uint8_t 红色通道值（0-255）。
   * @param g uint8_t 绿色通道值（0-255）。
   * @param b uint8_t 蓝色通道值（0-255）。
   * @return bool 总是返回 true。
   * @note 颜色会与亮度曲线叠加。
   * @throws 无异常抛出。
   */
  bool setColor(uint8_t r, uint8_t g, uint8_t b);
  /**
   * @brief 设置呼吸效果启用状态。
   * @param en bool true 启用，false 关闭。
   * @return 无。
   * @note 关闭时会输出全黑并停止更新。
   * @throws 无异常抛出。
   */
  void setEnabled(bool en);
  /**
   * @brief 获取当前呼吸半周期时长。
   * @return uint16_t 半周期时间（毫秒）。
   * @note 返回值为内部保存值。
   * @throws 无异常抛出。
   */
  uint16_t getPeriodMs() const;
  /**
   * @brief 获取当前亮度范围。
   * @param minVal uint8_t& 输出最小亮度。
   * @param maxVal uint8_t& 输出最大亮度。
   * @return 无。
   * @note 输出参数必须为可写引用。
   * @throws 无异常抛出。
   */
  void getRange(uint8_t& minVal, uint8_t& maxVal) const;
  /**
   * @brief 获取当前颜色设置。
   * @param r uint8_t& 输出红色通道值（0-255）。
   * @param g uint8_t& 输出绿色通道值（0-255）。
   * @param b uint8_t& 输出蓝色通道值（0-255）。
   * @return 无。
   * @note 输出参数必须为可写引用。
   * @throws 无异常抛出。
   */
  void getColor(uint8_t& r, uint8_t& g, uint8_t& b) const;
  /**
   * @brief 获取呼吸效果启用状态。
   * @return bool 当前启用状态。
   * @note 无。
   * @throws 无异常抛出。
   */
  bool getEnabled() const;
 private:
  Adafruit_NeoPixel _strip;
  bool _enabled = true;
  bool _inited = false;
  uint16_t _count;
  uint8_t _r = 255, _g = 255, _b = 255;
  uint8_t _minVal = 0, _maxVal = 255;
  uint16_t _halfMs = 2000;
  const uint8_t _intervalMs = 10;
  uint16_t _lastDt = 10;
  float _gamma = 3.0f;
  float _phase = 0.0f;
  unsigned long _last = 0;
  /**
   * @brief 将指定颜色填充到所有像素。
   * @param r uint8_t 红色通道值（0-255）。
   * @param g uint8_t 绿色通道值（0-255）。
   * @param b uint8_t 蓝色通道值（0-255）。
   * @return 无。
   * @note 会立即调用 show() 输出。
   * @throws 无异常抛出。
   */
  void fill(uint8_t r, uint8_t g, uint8_t b);
  /**
   * @brief 将整数夹紧到 0~255。
   * @param v int 输入值。
   * @return uint8_t 夹紧后的值。
   * @note 用于亮度与颜色计算。
   * @throws 无异常抛出。
   */
  static uint8_t clamp255(int v);
};
