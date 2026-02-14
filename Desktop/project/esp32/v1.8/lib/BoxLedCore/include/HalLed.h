#pragma once
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

class HalLed {
public:
  /**
   * @brief 构造灯光硬件控制对象。
   * @param numPixels uint16_t WS2812 像素数量。
   * @param wsPin uint8_t WS2812 数据引脚。
   * @param warmPin uint8_t 暖光通道 PWM 引脚。
   * @param coolPin uint8_t 冷光通道 PWM 引脚。
   * @return 无。
   * @note 仅保存配置，需调用 begin() 完成初始化。
   * @throws 无异常抛出。
   */
  HalLed(uint16_t numPixels, uint8_t wsPin, uint8_t warmPin, uint8_t coolPin);
  /**
   * @brief 初始化 RGB 灯带与双色温 PWM。
   * @return 无。
   * @note 会设置默认亮度并同步当前 CCT 状态。
   * @throws 无异常抛出。
   */
  void begin();

  /**
   * @brief 设置单个像素的 RGB 值。
   * @param index uint16_t 像素索引（从 0 开始）。
   * @param r uint8_t 红色通道值（0-255）。
   * @param g uint8_t 绿色通道值（0-255）。
   * @param b uint8_t 蓝色通道值（0-255）。
   * @return 无。
   * @note 仅写入缓存，需调用 show() 输出。
   * @throws 无异常抛出。
   */
  void setRGB(uint16_t index, uint8_t r, uint8_t g, uint8_t b);
  /**
   * @brief 设置单个像素的打包颜色值。
   * @param index uint16_t 像素索引（从 0 开始）。
   * @param color uint32_t 由 Color() 生成的颜色值。
   * @return 无。
   * @note 仅写入缓存，需调用 show() 输出。
   * @throws 无异常抛出。
   */
  void setPixelColor(uint16_t index, uint32_t color);
  /**
   * @brief 用指定颜色填充整个灯带缓冲区。
   * @param r uint8_t 红色通道值（0-255）。
   * @param g uint8_t 绿色通道值（0-255）。
   * @param b uint8_t 蓝色通道值（0-255）。
   * @return 无。
   * @note 仅写入缓存，需调用 show() 输出。
   * @throws 无异常抛出。
   */
  void fillScreen(uint8_t r, uint8_t g, uint8_t b);
  /**
   * @brief 将缓冲区内容输出到 RGB 灯带。
   * @return 无。
   * @note 若 RGB 被禁用，将不会输出新数据。
   * @throws 无异常抛出。
   */
  void show();
  /**
   * @brief 清空 RGB 灯带缓冲区。
   * @return 无。
   * @note 仅清空缓冲，不立即输出。
   * @throws 无异常抛出。
   */
  void clear();
  /**
   * @brief 设置 RGB 灯带全局亮度。
   * @param b uint8_t 亮度值（0-255）。
   * @return 无。
   * @note 亮度由底层库应用。
   * @throws 无异常抛出。
   */
  void setBrightness(uint8_t b);
  /**
   * @brief 获取 RGB 灯带全局亮度。
   * @return uint8_t 当前亮度值（0-255）。
   * @note 直接来自底层库。
   * @throws 无异常抛出。
   */
  uint8_t getBrightness() const;
  /**
   * @brief 设置 RGB 灯带启用状态。
   * @param en bool true 启用输出，false 关闭输出。
   * @return 无。
   * @note 关闭时会清屏并立即显示。
   * @throws 无异常抛出。
   */
  void setEnabled(bool en);
  /**
   * @brief 获取 RGB 灯带启用状态。
   * @return bool 当前启用状态。
   * @note 无。
   * @throws 无异常抛出。
   */
  bool getEnabled() const;
  /**
   * @brief 获取灯带像素数量。
   * @return uint16_t 像素数量。
   * @note 来自底层灯带对象。
   * @throws 无异常抛出。
   */
  uint16_t numPixels() const;
  /**
   * @brief 生成打包颜色值。
   * @param r uint8_t 红色通道值（0-255）。
   * @param g uint8_t 绿色通道值（0-255）。
   * @param b uint8_t 蓝色通道值（0-255）。
   * @return uint32_t 打包后的颜色值。
   * @note 颜色格式与底层库一致。
   * @throws 无异常抛出。
   */
  uint32_t Color(uint8_t r, uint8_t g, uint8_t b);

  /**
   * @brief 设置双色温灯的色温与亮度。
   * @param kelvin int 目标色温（Kelvin），范围将被限制在 2700~6500。
   * @param brightnessPercent int 亮度百分比（0-100）。
   * @return 无。
   * @note 参数越界会被自动夹紧并立即刷新 PWM。
   * @throws 无异常抛出。
   */
  void setCCT(int kelvin, int brightnessPercent);
  /**
   * @brief 设置双色温灯开关状态。
   * @param enabled bool true 开启，false 关闭。
   * @return 无。
   * @note 关闭时输出占空比为 0。
   * @throws 无异常抛出。
   */
  void setCCTEnabled(bool enabled);
  /**
   * @brief 获取当前色温值。
   * @return int 当前色温（Kelvin）。
   * @note 返回值为内部保存值。
   * @throws 无异常抛出。
   */
  int getKelvin() const;
  /**
   * @brief 获取当前双色温亮度百分比。
   * @return int 亮度百分比（0-100）。
   * @note 返回值为内部保存值。
   * @throws 无异常抛出。
   */
  int getCCTBrightness() const;
  /**
   * @brief 获取双色温灯开关状态。
   * @return bool 当前开关状态。
   * @note 无。
   * @throws 无异常抛出。
   */
  bool getCCTEnabled() const;

private:
  Adafruit_NeoPixel _strip;
  uint8_t _wsPin;
  uint8_t _warmPin;
  uint8_t _coolPin;

  // RGB State
  bool _rgbEnabled = true;

  // CCT State
  bool _cctEnabled = true;
  int _kelvin = 4000;
  int _cctBrightness = 50;
  
  // CCT PWM Config
  const uint8_t _chWarm = 0;
  const uint8_t _chCool = 1;
  const uint32_t _freq = 1000;
  const uint8_t _resBits = 12;
  const int _minK = 2700;
  const int _maxK = 6500;

  /**
   * @brief 根据当前 CCT 参数更新 PWM 输出。
   * @return 无。
   * @note 内部会进行线性混合并归一化亮度。
   * @throws 无异常抛出。
   */
  void updateCCT();
};
