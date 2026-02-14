# Figure Box Studio（ESP32-C3 手办盒灯光控制）

一套基于 ESP32-C3 的手办盒灯光控制方案，支持在局域网内通过浏览器跨端远程控制。系统使用双色温 LED 为手办补光，可调色温与亮度；同时使用 WS2812 灯珠实现底部氛围灯效果，适配不同展示场景。

## 功能特性

- 双色温补光：色温与亮度可调
- WS2812 氛围灯：多种动效与颜色控制
- 多端浏览器远程控制：手机、平板、PC 访问
- 局域网内实时状态刷新与控制

## 目录结构

- lib/BoxLedCore：核心固件与控制逻辑
- platformio.ini：PlatformIO 配置

## 硬件说明

- 开发板：ESP32-C3
- 补光灯：双色温 LED
- 氛围灯：WS2812 灯珠

## 快速开始

### 环境要求

- VS Code + PlatformIO 插件
- 支持 ESP32-C3 的 USB 驱动

### 编译

```bash
pio run
```

### 上传

上传口通过环境变量配置：

```powershell
$env:UPLOAD_PORT="COM3"
```

然后执行：

```bash
pio run -t upload
```

### 串口监视

```bash
pio device monitor
```

### 调试

```bash
pio debug
```

### 单元测试

```bash
pio test -e esp32c3 --without-uploading
```

## 使用说明

- 设备接入局域网后，使用浏览器访问设备 IP
- 控制面板中可调节补光色温/亮度与氛围灯效果
- 支持实时状态刷新与场景保存/应用

## 许可证

请根据需要在此处补充许可证信息。
