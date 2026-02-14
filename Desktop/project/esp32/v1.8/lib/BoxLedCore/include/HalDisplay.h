#pragma once
#include <Arduino.h>
#include <U8g2lib.h>

class HalDisplay {
public:
  enum DisplayFont {
    FONT_SMALL,
    FONT_LARGE
  };

  /**
   * @brief 构造显示屏控制对象。
   * @return 无。
   * @note 仅完成对象初始化，需调用 begin() 初始化硬件。
   * @throws 无异常抛出。
   */
  HalDisplay();
  /**
   * @brief 初始化 I2C 与显示屏驱动。
   * @return 无。
   * @note 使用固定 I2C 引脚并设置默认字体与绘制颜色。
   * @throws 无异常抛出。
   */
  void begin();
  /**
   * @brief 清空屏幕缓冲区。
   * @return 无。
   * @note 仅清缓冲，需调用 display() 刷新到屏幕。
   * @throws 无异常抛出。
   */
  void clear();
  /**
   * @brief 将缓冲区内容发送到屏幕显示。
   * @return 无。
   * @note 与 clear() 搭配使用。
   * @throws 无异常抛出。
   */
  void display();
  /**
   * @brief 设置屏幕省电模式。
   * @param enable bool true 开启省电，false 关闭省电。
   * @return 无。
   * @note 省电模式下屏幕可能不刷新。
   * @throws 无异常抛出。
   */
  void setPowerSave(bool enable);
  
  /**
   * @brief 设置当前字体大小。
   * @param font DisplayFont 字体枚举值。
   * @return 无。
   * @note 影响后续文本绘制与宽度计算。
   * @throws 无异常抛出。
   */
  void setFont(DisplayFont font);
  /**
   * @brief 设置绘制颜色或模式。
   * @param c uint8_t 绘制颜色值（由 U8g2 定义）。
   * @return 无。
   * @note 用于反白或覆盖等绘制效果。
   * @throws 无异常抛出。
   */
  void setDrawColor(uint8_t c);
  
  /**
   * @brief 绘制 ASCII 字符串。
   * @param x int 左上角 X 坐标。
   * @param y int 基线 Y 坐标。
   * @param s const char* 字符串指针。
   * @return 无。
   * @note 需确保字符串有效且以 '\0' 结尾。
   * @throws 无异常抛出。
   */
  void drawStr(int x, int y, const char* s);
  /**
   * @brief 绘制 UTF-8 字符串。
   * @param x int 左上角 X 坐标。
   * @param y int 基线 Y 坐标。
   * @param s const char* UTF-8 字符串指针。
   * @return 无。
   * @note 字体需支持所绘制字符集。
   * @throws 无异常抛出。
   */
  void drawUTF8(int x, int y, const char* s);
  /**
   * @brief 获取 ASCII 字符串在当前字体下的宽度。
   * @param s const char* 字符串指针。
   * @return int 字符串宽度（像素）。
   * @note 仅适用于 ASCII 字符串。
   * @throws 无异常抛出。
   */
  int getStrWidth(const char* s);
  /**
   * @brief 获取 UTF-8 字符串在当前字体下的宽度。
   * @param s const char* UTF-8 字符串指针。
   * @return int 字符串宽度（像素）。
   * @note 仅对支持的字体与字符集有效。
   * @throws 无异常抛出。
   */
  int getUTF8Width(const char* s);
  
  /**
   * @brief 绘制矩形边框。
   * @param x int 左上角 X 坐标。
   * @param y int 左上角 Y 坐标。
   * @param w int 宽度（像素）。
   * @param h int 高度（像素）。
   * @return 无。
   * @note 坐标与尺寸需在屏幕范围内。
   * @throws 无异常抛出。
   */
  void drawFrame(int x, int y, int w, int h);
  /**
   * @brief 绘制实心矩形。
   * @param x int 左上角 X 坐标。
   * @param y int 左上角 Y 坐标。
   * @param w int 宽度（像素）。
   * @param h int 高度（像素）。
   * @return 无。
   * @note 坐标与尺寸需在屏幕范围内。
   * @throws 无异常抛出。
   */
  void drawBox(int x, int y, int w, int h);
  /**
   * @brief 绘制三角形边框。
   * @param x1 int 顶点 1 的 X 坐标。
   * @param y1 int 顶点 1 的 Y 坐标。
   * @param x2 int 顶点 2 的 X 坐标。
   * @param y2 int 顶点 2 的 Y 坐标。
   * @param x3 int 顶点 3 的 X 坐标。
   * @param y3 int 顶点 3 的 Y 坐标。
   * @return 无。
   * @note 坐标需在屏幕范围内。
   * @throws 无异常抛出。
   */
  void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3);

private:
  U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C _u8g2;
};
