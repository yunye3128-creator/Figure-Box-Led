#pragma once
#include <Arduino.h>
#include "HalLed.h"
#include "HalInput.h"
#include "HalDisplay.h"
#include "Effects.h"
#include "WebRouter.h"

struct SystemState;

class MenuUI {
 public:
  /**
   * @brief 构造菜单界面控制器。
   * @param led HalLed& 灯光硬件控制对象。
   * @param input HalInput& 按键输入控制对象。
   * @param display HalDisplay& 显示屏控制对象。
   * @param effects Effects& 动效控制对象。
   * @param web WebRouter& Web 控制与配网对象。
   * @return 无。
   * @note 仅保存引用，需调用 begin() 初始化状态。
   * @throws 无异常抛出。
   */
  MenuUI(HalLed& led, HalInput& input, HalDisplay& display, Effects& effects, WebRouter& web);
  /**
   * @brief 初始化界面状态并订阅系统事件。
   * @return 无。
   * @note 会读取系统状态并设置初始界面。
   * @throws 无异常抛出。
   */
  void begin();
  /**
   * @brief 更新界面逻辑与渲染。
   * @param nowMs uint32_t 当前时间戳（毫秒），通常为 millis()。
   * @return 无。
   * @note 需要周期性调用以处理按键与刷新显示。
   * @throws 无异常抛出。
   */
  void update(uint32_t nowMs);

 private:
  enum ItemKind : uint8_t { KIND_MENU = 0, KIND_TOGGLE = 1, KIND_ADJUST = 2 };

  enum Action : uint8_t {
    ACT_NONE = 0,
    ACT_CCT_KELVIN,
    ACT_CCT_BRIGHTNESS,
    ACT_CCT_TOGGLE,
    ACT_WS_TOGGLE,
    ACT_SET_EFFECT_RAINBOW,
    ACT_SET_EFFECT_BREATHE,
    ACT_SET_EFFECT_SOLID,
    ACT_RAINBOW_SPEED,
    ACT_BREATHE_HUE,
    ACT_BREATHE_PERIOD,
    ACT_SOLID_HUE,
    ACT_SCREEN_TIMEOUT,
    ACT_WIFI_REPROVISION,
    ACT_INFO_STA_IP,
    ACT_SCENE_APPLY_1,
    ACT_SCENE_SAVE_1,
    ACT_SCENE_APPLY_2,
    ACT_SCENE_SAVE_2,
    ACT_SCENE_APPLY_3,
    ACT_SCENE_SAVE_3,
    ACT_SCENE_APPLY_4,
    ACT_SCENE_SAVE_4
  };

  struct MenuItem {
    const char* label;
    uint8_t parent;
    uint8_t firstChild;
    uint8_t childCount;
    ItemKind kind;
    Action action;
  };

  enum UiMode : uint8_t { MODE_BROWSE = 0, MODE_ADJUST = 1 };
  enum ScreenSaverMode : uint8_t { SCREEN_STATUS = 0, SCREEN_TIME = 1, SCREEN_ROTATE = 2, SCREEN_OFF = 3 };

  HalLed& _led;
  HalInput& _input;
  HalDisplay& _display;
  Effects& _effects;
  WebRouter& _web;

  static const MenuItem _items[];
  static constexpr uint8_t _rootId = 0;

  UiMode _mode = MODE_BROWSE;
  uint8_t _stack[5]{_rootId};
  uint8_t _sel[5]{0};
  uint8_t _depth = 1;

  Action _adjustAction = ACT_NONE;
  const char* _adjustTitle = nullptr;
  uint8_t _adjustDepth = 0;

  uint16_t _breatheHue = 0;
  uint16_t _solidHue = 0;
  uint8_t _screenTimeoutSec = 255;
  uint8_t _screenSaverMode = SCREEN_ROTATE;
  uint32_t _lastInteractionMs = 0;
  bool _asleep = false;
  bool _softOff = false;
  bool _powerPressHandled = false;
  uint16_t _lastMinuteShown = 65535;
  bool _dirty = true;
  uint32_t _lastSyncMs = 0;
  int _lastCt = -1;
  int _lastCctB = -1;
  bool _lastCctOn = false;
  uint8_t _lastWsB = 0xFF;
  bool _lastWsOn = false;
  uint8_t _lastFx = 0xFF;
  uint8_t _lastRainbowSpeed = 0xFF;
  uint16_t _lastBreatheHalfMs = 0xFFFF;
  uint8_t _lastScreenTimeoutSec = 0xFF;
  uint8_t _lastScreenSaverMode = 0xFF;
  bool _lastProvisioning = true;
  bool _lastStaConnected = false;
  bool _lastStaConnecting = false;
  bool _lastStaFailed = false;
  int _lastStaFailCode = 0;
  char _lastStaIp[24]{0};
  uint32_t _wifiStatusUntilMs = 0;

  /**
   * @brief 处理左键触发的导航或调整操作。
   * @return 无。
   * @note 依据当前模式执行不同动作。
   * @throws 无异常抛出。
   */
  void onLeft();
  /**
   * @brief 处理右键触发的导航或调整操作。
   * @return 无。
   * @note 依据当前模式执行不同动作。
   * @throws 无异常抛出。
   */
  void onRight();
  /**
   * @brief 处理确认键短按事件。
   * @return 无。
   * @note 用于进入菜单或退出调节。
   * @throws 无异常抛出。
   */
  void onOkShort();
  /**
   * @brief 处理确认键长按事件。
   * @return 无。
   * @note 用于返回上级或退出调节。
   * @throws 无异常抛出。
   */
  void onOkLong();

  /**
   * @brief 在当前菜单中移动选中项。
   * @param delta int 移动步进，正数向下，负数向上。
   * @return 无。
   * @note 会循环选择子项。
   * @throws 无异常抛出。
   */
  void navigate(int delta);
  /**
   * @brief 执行当前选中项的进入或动作。
   * @return 无。
   * @note 菜单项类型不同会触发不同逻辑。
   * @throws 无异常抛出。
   */
  void enterSelected();
  /**
   * @brief 返回上一级菜单。
   * @return 无。
   * @note 已在根菜单时不产生作用。
   * @throws 无异常抛出。
   */
  void goBack();
  /**
   * @brief 如果需要则唤醒屏幕并记录交互时间。
   * @param nowMs uint32_t 当前时间戳（毫秒）。
   * @return 无。
   * @note 软关灯状态下不会唤醒。
   * @throws 无异常抛出。
   */
  void wakeIfNeeded(uint32_t nowMs);
  void renderScreenSaver(uint32_t nowMs);
  void renderScreenSaverStatus(const SystemState& s);
  void renderScreenSaverTime(uint32_t nowMs);
  void drawScreenStatusLine(const char* label, const char* percent, const char* rightText, uint8_t yBaseline);

  /**
   * @brief 获取当前菜单 ID。
   * @return uint8_t 当前菜单索引。
   * @note 基于栈顶菜单。
   * @throws 无异常抛出。
   */
  uint8_t currentMenuId() const;
  /**
   * @brief 获取当前菜单项结构。
   * @return const MenuItem& 当前菜单项引用。
   * @note 返回静态菜单表中的引用。
   * @throws 无异常抛出。
   */
  const MenuItem& currentMenu() const;
  /**
   * @brief 获取当前选中菜单项。
   * @return const MenuItem& 选中菜单项引用。
   * @note 返回静态菜单表中的引用。
   * @throws 无异常抛出。
   */
  const MenuItem& selectedItem() const;
  /**
   * @brief 获取当前菜单下选中子项索引。
   * @return uint8_t 子项索引。
   * @note 仅在有子项时有效。
   * @throws 无异常抛出。
   */
  uint8_t selectedChildIndex() const;
  /**
   * @brief 获取当前选中子项 ID。
   * @return uint8_t 子项 ID。
   * @note 无子项时返回根 ID。
   * @throws 无异常抛出。
   */
  uint8_t selectedChildId() const;

  /**
   * @brief 渲染当前界面到显示屏。
   * @param nowMs uint32_t 当前时间戳（毫秒）。
   * @return 无。
   * @note 根据状态选择渲染不同页面。
   * @throws 无异常抛出。
   */
  void render(uint32_t nowMs);
  /**
   * @brief 渲染主页界面。
   * @param nowMs uint32_t 当前时间戳（毫秒）。
   * @return 无。
   * @note 主页包含时间与当前选项。
   * @throws 无异常抛出。
   */
  void renderHome(uint32_t nowMs);
  /**
   * @brief 渲染菜单浏览界面。
   * @param menuId uint8_t 要渲染的菜单 ID。
   * @return 无。
   * @note 用于显示子菜单与状态信息。
   * @throws 无异常抛出。
   */
  void renderMenu(uint8_t menuId);
  /**
   * @brief 渲染参数调节界面。
   * @return 无。
   * @note 显示滑条与当前值。
   * @throws 无异常抛出。
   */
  void renderAdjust();
  /**
   * @brief 渲染配网提示界面。
   * @return 无。
   * @note 显示 AP SSID 与 IP。
   * @throws 无异常抛出。
   */
  void renderProvisioning(uint32_t nowMs);

  /**
   * @brief 以大字体居中绘制 UTF-8 字符串。
   * @param s const char* UTF-8 字符串指针。
   * @param yBaseline uint8_t 基线 Y 坐标。
   * @return 无。
   * @note 仅影响当前一行输出。
   * @throws 无异常抛出。
   */
  void drawCentered16(const char* s, uint8_t yBaseline);
  /**
   * @brief 以小字体靠右绘制字符串。
   * @param s const char* 字符串指针。
   * @param yBaseline uint8_t 基线 Y 坐标。
   * @return 无。
   * @note X 坐标自动右对齐。
   * @throws 无异常抛出。
   */
  void drawRightSmall(const char* s, uint8_t yBaseline);
  /**
   * @brief 以小字体靠左绘制字符串。
   * @param s const char* 字符串指针。
   * @param yBaseline uint8_t 基线 Y 坐标。
   * @return 无。
   * @note X 坐标固定在左侧。
   * @throws 无异常抛出。
   */
  void drawLeftSmall(const char* s, uint8_t yBaseline);
  /**
   * @brief 绘制左右方向提示三角形。
   * @return 无。
   * @note 用于提示可左右切换。
   * @throws 无异常抛出。
   */
  void drawTriangles();

  /**
   * @brief 将 HSV 色相值转换为 RGB。
   * @param h uint16_t 色相角度（0-359）。
   * @param r uint8_t& 输出红色通道值（0-255）。
   * @param g uint8_t& 输出绿色通道值（0-255）。
   * @param b uint8_t& 输出蓝色通道值（0-255）。
   * @return 无。
   * @note 饱和度与亮度固定为最大值。
   * @throws 无异常抛出。
   */
  static void hsvToRgb(uint16_t h, uint8_t& r, uint8_t& g, uint8_t& b);

  /**
   * @brief 执行菜单项进入动作。
   * @param a Action 动作类型枚举。
   * @return 无。
   * @note 部分动作会直接触发场景或配网流程。
   * @throws 无异常抛出。
   */
  void applyEnterAction(Action a);
  /**
   * @brief 执行开关类动作。
   * @param a Action 动作类型枚举。
   * @return 无。
   * @note 仅处理开关类菜单项。
   * @throws 无异常抛出。
   */
  void applyToggle(Action a);
  /**
   * @brief 调节当前参数的步进值。
   * @param delta int 步进方向，正数增加，负数减少。
   * @return 无。
   * @note 会根据参数范围自动夹紧。
   * @throws 无异常抛出。
   */
  void adjustStep(int delta);
  /**
   * @brief 获取指定动作当前参数值。
   * @param a Action 动作类型枚举。
   * @return int 当前参数值。
   * @note 用于显示与调节计算。
   * @throws 无异常抛出。
   */
  int getAdjustValue(Action a) const;
  /**
   * @brief 设置指定动作的参数值并同步到系统。
   * @param a Action 动作类型枚举。
   * @param v int 目标参数值。
   * @return 无。
   * @note 可能触发灯效与状态变更。
   * @throws 无异常抛出。
   */
  void setAdjustValue(Action a, int v);
  /**
   * @brief 获取指定动作的参数范围与步进。
   * @param a Action 动作类型枚举。
   * @param minV int& 输出最小值。
   * @param maxV int& 输出最大值。
   * @param stepV int& 输出步进值。
   * @return 无。
   * @note 用于调节与进度条计算。
   * @throws 无异常抛出。
   */
  void getAdjustRange(Action a, int& minV, int& maxV, int& stepV) const;
};
