#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPUpdateServer.h>
#include <Preferences.h>
#include <DNSServer.h>
#include <ESPmDNS.h>
#include <time.h>
#include "HalLed.h"
#include "Effects.h"

class WebRouter {
 public:
  /**
   * @brief 构造 Web 路由与配网控制器。
   * @param led HalLed& 灯光硬件控制对象。
   * @param effects Effects& 动效控制对象。
   * @return 无。
   * @note 仅保存引用，需调用 begin() 启动服务。
   * @throws 无异常抛出。
   */
  WebRouter(HalLed& led, Effects& effects);
  /**
   * @brief 启动 Web 服务与配网流程。
   * @param apNameBase const char* AP 名称前缀。
   * @param apPwd const char* AP 密码，空字符串表示无密码。
   * @return 无。
   * @note 会初始化 WiFi、WebServer 与 OTA 服务。
   * @throws 无异常抛出。
   */
  void begin(const char* apNameBase, const char* apPwd);
  /**
   * @brief 处理 Web 请求与后台状态更新。
   * @return 无。
   * @note 需在主循环中高频调用。
   * @throws 无异常抛出。
   */
  void handle();
  /**
   * @brief 查询是否处于配网模式。
   * @return bool true 表示配网中，false 表示已完成。
   * @note 该状态会影响 UI 展示。
   * @throws 无异常抛出。
   */
  bool isProvisioning() const;
  /**
   * @brief 查询 STA 是否已连接。
   * @return bool true 表示已连接。
   * @note 仅反映内部连接状态。
   * @throws 无异常抛出。
   */
  bool isStaConnected() const;
  /**
   * @brief 获取当前 AP SSID。
   * @param out char* 输出缓冲区。
   * @param n size_t 缓冲区长度。
   * @return 无。
   * @note 结果以 '\0' 结尾。
   * @throws 无异常抛出。
   */
  void getApSsid(char* out, size_t n) const;
  /**
   * @brief 获取当前 AP IP。
   * @param out char* 输出缓冲区。
   * @param n size_t 缓冲区长度。
   * @return 无。
   * @note 结果以 '\0' 结尾。
   * @throws 无异常抛出。
   */
  void getApIp(char* out, size_t n) const;
  /**
   * @brief 获取当前 STA IP。
   * @param out char* 输出缓冲区。
   * @param n size_t 缓冲区长度。
   * @return 无。
   * @note 未连接时输出为空字符串。
   * @throws 无异常抛出。
   */
  void getStaIp(char* out, size_t n) const;
  /**
   * @brief 触发重新配网流程。
   * @return 无。
   * @note 会切换到 AP 配网模式。
   * @throws 无异常抛出。
   */
  void reprovision();
  /**
   * @brief 保存当前状态到指定场景槽。
   * @param slot uint8_t 场景槽位（从 1 开始）。
   * @return bool 成功返回 true，失败返回 false。
   * @note 失败可能因为槽位无效或存储错误。
   * @throws 无异常抛出。
   */
  bool saveScene(uint8_t slot);
  /**
   * @brief 应用指定场景槽的状态。
   * @param slot uint8_t 场景槽位（从 1 开始）。
   * @return bool 成功返回 true，失败返回 false。
   * @note 失败可能因为槽位无效或读取错误。
   * @throws 无异常抛出。
   */
  bool applyScene(uint8_t slot);
 private:
  HalLed& _led;
  Effects& _effects;
  WebServer _server;
  HTTPUpdateServer _otaServer;
  Preferences _prefs;
  DNSServer _dns;
  bool _provisioning = true;
  String _apSsid;
  bool _staConnecting = false;
  uint32_t _staStartMs = 0;
  uint32_t _staTimeoutMs = 60000;
  bool _staFinalizePending = false;
  uint32_t _staFinalizeAtMs = 0;
  uint32_t _staLastFailMs = 0;
  int _staLastFailCode = 0;
  bool _timeSyncPending = false;
  bool _timeSynced = false;
  bool _mdnsStarted = false;
  bool _dnsRunning = false;
  WiFiClient _eventsClient;
  bool _eventsConnected = false;
  uint32_t _eventsLastSendMs = 0;
  uint32_t _eventsLastPingMs = 0;
  String _eventsLastJson;
  const uint16_t _lightMinIntervalMs = 16;
  uint32_t _lastLightUpdateMs = 0;
  String _lastLightResponse;
  /**
   * @brief 同步一次网络时间。
   * @param timeoutMs uint32_t 超时时间（毫秒）。
   * @return bool 成功返回 true，超时返回 false。
   * @note 仅用于首次时间同步。
   * @throws 无异常抛出。
   */
  bool syncTimeOnce(uint32_t timeoutMs);
  /**
   * @brief 处理根页面请求。
   * @return 无。
   * @note 返回控制页面 HTML。
   * @throws 无异常抛出。
   */
  void handleRoot();
  /**
   * @brief 处理 WiFi 状态查询接口。
   * @return 无。
   * @note 返回 JSON 数据。
   * @throws 无异常抛出。
   */
  void handleApiWifiStatus();
  void handleApiWifiScan();
  /**
   * @brief 处理 WiFi 配置接口。
   * @return 无。
   * @note 读取请求参数并尝试连接。
   * @throws 无异常抛出。
   */
  void handleApiWifiConfig();
  /**
   * @brief 处理忘记 WiFi 的接口。
   * @return 无。
   * @note 清除已保存的 WiFi 信息。
   * @throws 无异常抛出。
   */
  void handleApiWifiForget();
  /**
   * @brief 处理灯光总体状态查询接口。
   * @return 无。
   * @note 返回包含灯光与效果的 JSON。
   * @throws 无异常抛出。
   */
  void handleApiStatus();
  /**
   * @brief 处理灯光基础参数设置接口。
   * @return 无。
   * @note 包括色温与亮度等。
   * @throws 无异常抛出。
   */
  void handleApiLight();
  /**
   * @brief 处理效果参数设置接口。
   * @return 无。
   * @note 包括模式、速度、颜色等。
   * @throws 无异常抛出。
   */
  void handleApiEffect();
  void handleApiConfig();
  /**
   * @brief 处理电源开关控制接口。
   * @return 无。
   * @note 可能影响 RGB 与 CCT 开关。
   * @throws 无异常抛出。
   */
  void handleApiPower();
  /**
   * @brief 处理事件流推送接口。
   * @return 无。
   * @note 用于 SSE 或长连接推送。
   * @throws 无异常抛出。
   */
  void handleEvents();
  /**
   * @brief 处理手动像素控制页面请求。
   * @return 无。
   * @note 返回手动控制页面内容。
   * @throws 无异常抛出。
   */
  void handleManual();
  /**
   * @brief 处理像素状态查询接口。
   * @return 无。
   * @note 返回像素缓存 JSON。
   * @throws 无异常抛出。
   */
  void handlePixelsState();
  /**
   * @brief 处理单像素设置接口。
   * @return 无。
   * @note 从请求参数读取像素信息。
   * @throws 无异常抛出。
   */
  void handlePixel();
  /**
   * @brief 处理保存场景的接口。
   * @return 无。
   * @note 读取槽位参数并保存。
   * @throws 无异常抛出。
   */
  void handleApiSceneSave();
  /**
   * @brief 处理应用场景的接口。
   * @return 无。
   * @note 读取槽位参数并应用。
   * @throws 无异常抛出。
   */
  void handleApiSceneApply();
  /**
   * @brief 启动 AP 配网模式。
   * @param apPwd const char* AP 密码。
   * @return 无。
   * @note 会启动 DNS 以及 WebServer。
   * @throws 无异常抛出。
   */
  void startAp(const char* apPwd);
  /**
   * @brief 发送通用 CORS 响应头。
   * @return 无。
   * @note 用于跨域 API 请求。
   * @throws 无异常抛出。
   */
  void sendApiCors();
  /**
   * @brief 构建当前系统状态 JSON。
   * @return String JSON 字符串。
   * @note 用于前端状态刷新。
   * @throws 无异常抛出。
   */
  String buildStateJson() const;
  /**
   * @brief 构建 WiFi 状态 JSON。
   * @return String JSON 字符串。
   * @note 包含配网与连接信息。
   * @throws 无异常抛出。
   */
  String buildWifiStatusJson() const;
  /**
   * @brief 从请求参数更新灯光状态。
   * @return bool 成功返回 true，参数非法返回 false。
   * @note 仅处理灯光基础参数。
   * @throws 无异常抛出。
   */
  bool updateLightFromArgs();
  /**
   * @brief 从请求参数应用效果设置。
   * @return 无。
   * @note 需要先通过 updateLightFromArgs 完成基础同步。
   * @throws 无异常抛出。
   */
  void applyEffectFromArgs();
};
