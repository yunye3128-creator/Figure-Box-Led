 #pragma once
#include <Arduino.h>
#include "HalLed.h"

enum EffectMode { EFFECT_OFF = 0, EFFECT_RAINBOW = 1, EFFECT_BREATHE = 2, EFFECT_WIPE = 3, EFFECT_BREATHE_COLOR = 4, EFFECT_SOLID = 5, EFFECT_MANUAL = 6 };

 class Effects {
  public:
  /**
   * @brief 构造效果控制器并订阅系统效果状态。
   * @param m HalLed& 底层灯带硬件控制对象。
   * @return 无。
   * @note 构造时会注册回调以接收效果状态变化。
   * @throws 无异常抛出。
   */
  explicit Effects(HalLed& m);
  /**
   * @brief 设置当前效果模式。
   * @param e EffectMode 枚举值，表示要切换的效果。
   * @return 无。
   * @note 切换后内部会选择对应的更新策略。
   * @throws 无异常抛出。
   */
  void setEffect(EffectMode e);
  /**
   * @brief 设置呼吸/常亮等模式的基准颜色。
   * @param r uint8_t 红色通道值（0-255）。
   * @param g uint8_t 绿色通道值（0-255）。
   * @param b uint8_t 蓝色通道值（0-255）。
   * @return 无。
   * @note 仅影响依赖基色的效果模式。
   * @throws 无异常抛出。
   */
  void setBreatheColor(uint8_t r, uint8_t g, uint8_t b);
  /**
   * @brief 获取呼吸/常亮等模式的基准颜色。
   * @param r uint8_t& 输出红色通道值（0-255）。
   * @param g uint8_t& 输出绿色通道值（0-255）。
   * @param b uint8_t& 输出蓝色通道值（0-255）。
   * @return 无。
   * @note 输出参数必须为可写引用。
   * @throws 无异常抛出。
   */
  void getBreatheColor(uint8_t& r, uint8_t& g, uint8_t& b) const;
  /**
   * @brief 设置呼吸半周期时长。
   * @param ms uint16_t 半周期时间（毫秒），范围会被限制在 200~10000。
   * @return 无。
   * @note 输入超出范围会被自动夹紧。
   * @throws 无异常抛出。
   */
  void setBreatheHalfMs(uint16_t ms);
  /**
   * @brief 获取当前呼吸半周期时长。
   * @return uint16_t 当前半周期时间（毫秒）。
   * @note 返回值已是内部夹紧后的结果。
   * @throws 无异常抛出。
   */
  uint16_t getBreatheHalfMs() const;
  /**
   * @brief 开关常亮颜色自动轮换。
   * @param en bool true 开启，false 关闭。
   * @return 无。
   * @note 仅在常亮模式下生效。
   * @throws 无异常抛出。
   */
  void setSolidCycleEnabled(bool en);
  /**
   * @brief 获取常亮颜色自动轮换开关状态。
   * @return bool 当前开关状态。
   * @note 无。
   * @throws 无异常抛出。
   */
  bool getSolidCycleEnabled() const;
  /**
   * @brief 设置常亮颜色轮换周期。
   * @param ms uint16_t 周期毫秒数，范围会被限制在 200~20000。
   * @return 无。
   * @note 输入超出范围会被自动夹紧。
   * @throws 无异常抛出。
   */
  void setSolidCycleMs(uint16_t ms);
  /**
   * @brief 获取常亮颜色轮换周期。
   * @return uint16_t 周期毫秒数。
   * @note 返回值为内部保存值。
   * @throws 无异常抛出。
   */
  uint16_t getSolidCycleMs() const;
  /**
   * @brief 设置彩虹效果速度。
   * @param s uint8_t 速度值，范围会被限制在 1~255。
   * @return 无。
   * @note 值越大，彩虹变化越快。
   * @throws 无异常抛出。
   */
  void setRainbowSpeed(uint8_t s);
  /**
   * @brief 获取彩虹效果速度。
   * @return uint8_t 当前速度值。
   * @note 返回值为内部保存值。
   * @throws 无异常抛出。
   */
  uint8_t getRainbowSpeed() const;
  /**
   * @brief 设置彩虹效果方向。
   * @param forward bool true 表示正向，false 表示反向。
   * @return 无。
   * @note 方向仅影响彩虹效果。
   * @throws 无异常抛出。
   */
  void setRainbowDirection(bool forward);
  /**
   * @brief 获取彩虹效果方向。
   * @return bool 当前方向标志。
   * @note 无。
   * @throws 无异常抛出。
   */
  bool getRainbowDirection() const;
  /**
   * @brief 设置跑点效果速度。
   * @param s uint16_t 速度值，范围会被限制在 1~1000。
   * @return 无。
   * @note 值越大步进越快。
   * @throws 无异常抛出。
   */
  void setWipeSpeed(uint16_t s);
  /**
   * @brief 获取跑点效果速度。
   * @return uint16_t 当前速度值。
   * @note 返回值为内部保存值。
   * @throws 无异常抛出。
   */
  uint16_t getWipeSpeed() const;
  /**
   * @brief 设置跑点效果方向。
   * @param forward bool true 表示正向，false 表示反向。
   * @return 无。
   * @note 方向仅影响跑点效果。
   * @throws 无异常抛出。
   */
  void setWipeDirection(bool forward);
  /**
   * @brief 获取跑点效果方向。
   * @return bool 当前方向标志。
   * @note 无。
   * @throws 无异常抛出。
   */
  bool getWipeDirection() const;
  /**
   * @brief 获取当前效果模式。
   * @return EffectMode 当前模式枚举值。
   * @note 无。
   * @throws 无异常抛出。
   */
  EffectMode getEffect() const;
  /**
   * @brief 更新效果帧并输出到灯带。
   * @param now unsigned long 当前时间戳（毫秒），通常为 millis()。
   * @return 无。
   * @note 需周期性调用以产生动画效果；当间隔不足时会跳过更新。
   * @throws 无异常抛出。
   */
  void update(unsigned long now);
  /**
   * @brief 设置单个像素的颜色、亮度与开关状态。
   * @param i uint16_t 像素索引（从 0 开始）。
   * @param r uint8_t 红色通道值（0-255）。
   * @param g uint8_t 绿色通道值（0-255）。
   * @param b uint8_t 蓝色通道值（0-255）。
   * @param br uint8_t 像素亮度（0-255）。
   * @param on bool 像素开关状态。
   * @return 无。
   * @note 仅在手动模式中使用；索引越界会被忽略。
   * @throws 无异常抛出。
   */
  void setPixel(uint16_t i, uint8_t r, uint8_t g, uint8_t b, uint8_t br, bool on);
  /**
   * @brief 设置单个像素的颜色。
   * @param i uint16_t 像素索引（从 0 开始）。
   * @param r uint8_t 红色通道值（0-255）。
   * @param g uint8_t 绿色通道值（0-255）。
   * @param b uint8_t 蓝色通道值（0-255）。
   * @return 无。
   * @note 索引越界会被忽略。
   * @throws 无异常抛出。
   */
  void setPixelColor(uint16_t i, uint8_t r, uint8_t g, uint8_t b);
  /**
   * @brief 设置单个像素的亮度。
   * @param i uint16_t 像素索引（从 0 开始）。
   * @param br uint8_t 亮度值（0-255）。
   * @return 无。
   * @note 索引越界会被忽略。
   * @throws 无异常抛出。
   */
  void setPixelBrightness(uint16_t i, uint8_t br);
  /**
   * @brief 设置单个像素的开关状态。
   * @param i uint16_t 像素索引（从 0 开始）。
   * @param on bool 开关状态。
   * @return 无。
   * @note 索引越界会被忽略。
   * @throws 无异常抛出。
   */
  void setPixelOn(uint16_t i, bool on);
  /**
   * @brief 获取当前像素数量。
   * @return uint16_t 像素总数。
   * @note 数量来自底层 HalLed。
   * @throws 无异常抛出。
   */
  uint16_t getPixelCount() const;
  /**
   * @brief 获取单个像素的颜色、亮度与开关状态。
   * @param i uint16_t 像素索引（从 0 开始）。
   * @param r uint8_t& 输出红色通道值（0-255）。
   * @param g uint8_t& 输出绿色通道值（0-255）。
   * @param b uint8_t& 输出蓝色通道值（0-255）。
   * @param br uint8_t& 输出亮度值（0-255）。
   * @param on bool& 输出开关状态。
   * @return 无。
   * @note 若像素数据尚未初始化或索引越界，返回全零并关闭。
   * @throws 无异常抛出。
   */
  void getPixel(uint16_t i, uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& br, bool& on) const;
  private:
  struct IEffect {
    /**
     * @brief 更新对应效果的单帧输出。
     * @param fx Effects& 外部效果控制器引用。
     * @param dt uint16_t 本次更新时间间隔（毫秒）。
     * @return 无。
     * @note 由外层根据当前模式调用。
     * @throws 无异常抛出。
     */
    virtual void update(Effects& fx, uint16_t dt) = 0;
    /**
     * @brief 虚析构函数，保证派生类安全释放。
     * @return 无。
     * @note 无。
     * @throws 无异常抛出。
     */
    virtual ~IEffect() {}
  };
  struct RainbowFx : IEffect {
    /**
     * @brief 彩虹效果单帧更新。
     * @param fx Effects& 外部效果控制器引用。
     * @param dt uint16_t 本次更新时间间隔（毫秒）。
     * @return 无。
     * @note 仅更新彩虹效果。
     * @throws 无异常抛出。
     */
    void update(Effects& fx, uint16_t dt) override;
  };
  struct BreatheFx : IEffect {
    /**
     * @brief 呼吸效果单帧更新。
     * @param fx Effects& 外部效果控制器引用。
     * @param dt uint16_t 本次更新时间间隔（毫秒）。
     * @return 无。
     * @note 仅更新呼吸效果。
     * @throws 无异常抛出。
     */
    void update(Effects& fx, uint16_t dt) override;
  };
  HalLed& _hal;
  EffectMode _mode = EFFECT_RAINBOW;
  unsigned long _last = 0;
  uint16_t _rainbowOffsetQ4 = 0;
  uint8_t _rainbowSpeed = 16;
  uint8_t _hueOffset = 0;
  bool _rainbowForward = true;
  int _breatheVal = 0;
  int _breatheDir = 1;
  uint16_t _wipePos = 0;
  uint16_t _wipeSpeed = 1;
  bool _wipeForward = true;
  uint8_t _baseR = 255;
  uint8_t _baseG = 255;
  uint8_t _baseB = 255;
  float _phase = 0.0f;
  uint16_t _halfMs = 2000;
  const uint8_t _intervalMs = 10;
  uint16_t _lastDt = 10;
  uint8_t _min = 10;
  float _gamma = 3.0f;
  /**
   * @brief 颜色轮盘算法，将色相位置映射为 RGB 颜色。
   * @param pos uint8_t 色相位置（0-255）。
   * @return uint32_t 编码后的 RGB 颜色值。
   * @note 颜色编码来自 HalLed::Color。
   * @throws 无异常抛出。
   */
  uint32_t wheel(uint8_t pos);
  /**
   * @brief 生成彩虹效果单帧输出。
   * @return 无。
   * @note 使用彩虹速度与方向计算偏移。
   * @throws 无异常抛出。
   */
  void stepRainbow();
  /**
   * @brief 生成呼吸效果单帧输出。
   * @return 无。
   * @note 采用余弦缓动与 Gamma 校正。
   * @throws 无异常抛出。
   */
  void stepBreathe();
  /**
   * @brief 生成跑点效果单帧输出。
   * @return 无。
   * @note 跑点步进由速度与方向决定。
   * @throws 无异常抛出。
   */
  void stepWipe();
  /**
   * @brief 生成呼吸变色效果单帧输出。
   * @return 无。
   * @note 色相不断前进并叠加呼吸亮度。
   * @throws 无异常抛出。
   */
  void stepBreatheColor();
  /**
   * @brief 生成常亮效果单帧输出。
   * @return 无。
   * @note 使用当前基准颜色填充全灯带。
   * @throws 无异常抛出。
   */
  void stepSolid();
  bool _solidCycle = false;
  uint16_t _solidMs = 4000;
  float _solidPhase = 0.0f;
  /**
   * @brief 生成常亮颜色轮换效果单帧输出。
   * @return 无。
   * @note 基于相位缓慢变换色相。
   * @throws 无异常抛出。
   */
  void stepSolidCycle();
  struct Px { uint8_t r; uint8_t g; uint8_t b; uint8_t br; bool on; };
  Px* _px = nullptr;
  uint16_t _pxCount = 0;
  /**
   * @brief 确保手动像素缓存与灯带数量一致。
   * @return 无。
   * @note 当像素数量变化时会重新分配内存。
   * @throws 无异常抛出。
   */
  void ensurePixels();
  /**
   * @brief 生成手动像素模式单帧输出。
   * @return 无。
   * @note 使用 setPixel 写入的缓存数据。
   * @throws 无异常抛出。
   */
  void stepManual();
  IEffect* _curFx = nullptr;
  RainbowFx _rainbowFx;
  BreatheFx _breatheFx;
 };
