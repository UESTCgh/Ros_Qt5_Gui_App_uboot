#include "widgets/dashboard.h"

#include <qapplication.h>
#include <qpainter.h>
#include <qpainterpath.h>

#include <QtGui/QFontDatabase>

#include "QDebug"
DashBoard::DashBoard(QWidget* parent)
    : QWidget(parent),
      _gear(kGear_4),
      _rpm(0),
      _speed(0),
      _temperature(0),
      _oil(0) {
  QFontDatabase::addApplicationFont(":/fonts/DejaVuSans.ttf");
  this->resize(parent->size());
  this->parent = parent;
}

void DashBoard::set_gear(const DashBoard::Gear gear) {
  _gear = gear;
  update();
}

void DashBoard::set_rpm(const int rpm) {
  _rpm = rpm;
  update();
}

void DashBoard::set_speed(const int speed) {
  _speed = speed;
  _rpm = speed;
  // printf("%2.0f",_speed);
  update();
}

void DashBoard::set_temperature(const double temperature) {
  _temperature = temperature;
  update();
}

void DashBoard::set_oil(const int oil) {
  _oil = oil;
  update();
}

void DashBoard::paintEvent(QPaintEvent* event) {
  this->resize(parent->size());
  QWidget::paintEvent(event);

  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  // ===================== 绘制顶部两个图片 =====================
  QPixmap pix1(QString::fromStdString("/home/xyc/ui/uboot_qt_pi/png/jcc.png"));
  // QPixmap pix1(QString::fromStdString("/home/xyc/ui/uboot_qt_pi/png/yds.png"));
  QPixmap pix2(QString::fromStdString("/home/xyc/ui/uboot_qt_pi/png/ft.png"));

  // 获取中心点
  int centerX = parent->width() / 2;
  int centerY = 100;  // 顶部区域显示两个 logo，可以调整位置

  // 缩放比例（控制显示大小）
  qreal ft_scaleFactor = 0.08;
  qreal jcc_scaleFactor = 0.2;
  qreal yds_scaleFactor = 0.15;

  // ================= 左侧图 =================
  painter.save();
  painter.translate(centerX - 120, centerY);  // 左移一点
  painter.scale(yds_scaleFactor, yds_scaleFactor);
  // painter.scale(jcc_scaleFactor, jcc_scaleFactor);
  painter.drawPixmap(-pix1.width() / 2, -pix1.height() / 2, pix1);
  painter.restore();



  // ================= 右侧图 =================
  painter.save();
  painter.translate(centerX + 120, centerY);  // 右移一点
  painter.scale(ft_scaleFactor, ft_scaleFactor);
  painter.drawPixmap(-pix2.width() / 2, -pix2.height() / 2, pix2);
  painter.restore();

  // ===================== 绘制下方表盘 =====================
  int side = qMin(int(parent->width() / 1.8), parent->height());

  painter.save(); // 保存原始状态
  painter.translate(parent->width() / 2, parent->height() * 0.65);  // 向下移动一点
  painter.scale(side / 280.0, side / 280.0);

  draw_tachometer(painter);
  // draw_speedometer(painter);
  // draw_gear(painter);
  // draw_thermometer(painter);
  // draw_oil_meter(painter);
  painter.restore(); // 恢复状态
}


void DashBoard::draw_tachometer(QPainter& painter) {
  static QColor normal_color(18, 11, 10, 245);
  static QColor overrun_color(245, 64, 64, 225);

  // 绘制表盘外檐
  painter.save();
  painter.setPen(QPen(normal_color, 1, Qt::SolidLine));
  QRect rect(-95, -95, 190, 190);
  painter.drawArc(rect, 0, 270 * 16);
  painter.restore();

  // 绘制红色区域
  painter.save();
  static QRectF rectangle_outer(-95, -95, 190, 190);
  static QRectF rectangle_inner(-87, -87, 174, 174);
  painter.setBrush(overrun_color);
  QPainterPath path;
  path.arcTo(rectangle_outer, 0.0, 108.0);
  path.arcTo(rectangle_inner, 108, -108);
  painter.drawPath(path);
  painter.restore();

  // 绘制大刻度
  painter.save();
  painter.setPen(QPen(normal_color, 1, Qt::SolidLine));
  painter.rotate(90);
  for (int i = 0; i < 21; ++i) {
    painter.drawLine(88, 0, 94, 0);
    painter.rotate(13.5);
  }
  painter.restore();

  // 绘制小刻度
  painter.save();
  painter.setPen(QPen(normal_color, 1, Qt::SolidLine));
  painter.rotate(90);
  for (int i = 0; i < 100; ++i) {
    painter.drawLine(91, 0, 94, 0);
    painter.rotate(2.7);
  }
  painter.restore();

  // 绘制表盘数字
  painter.save();
  painter.rotate(90);
  painter.setPen(normal_color);
  painter.setFont(QFont("Times", 14));
  for (int i = 0; i < 11; ++i) {
    painter.save();
    if (i > 6) {
      painter.setPen(overrun_color);
    }
    painter.rotate(27.0 * i);
    painter.translate(76, 0);
    painter.rotate(270 - 27.0 * i);
    painter.drawText(QRect(-20, -10, 40, 20), Qt::AlignCenter,
                     QString::number(i));
    painter.restore();
  }
  painter.restore();

  // 绘制指针
  static const QPoint hand[] = {QPoint(-4, 0), QPoint(0, 94), QPoint(4, 0),
                                QPoint(0, -6)};
  static QColor hand_color(0x88, 0x37, 0x4f, 176);
  painter.save();
  painter.setPen(Qt::NoPen);
  painter.setBrush(hand_color);
  painter.rotate(27.0 * (_rpm / 10.0));
  painter.drawConvexPolygon(hand, 4);
  painter.restore();

  // 绘制文字
  painter.save();
  painter.setPen(normal_color);
  painter.setFont(QFont("DejaVu Sans", 8));
  painter.drawText(QRect(-50, -70, 100, 50), Qt::AlignCenter, "×10");
  painter.setFont(QFont("DejaVu Sans", 8, 50, true));
  painter.drawText(QRect(-50, 34, 32, 16), Qt::AlignCenter, "CM/S");
  painter.restore();
}


void DashBoard::draw_speedometer(QPainter& painter) {
  painter.save();

  painter.translate(-60, 80);  // 左移60像素，竖直位置也略上移

  // SPEED 标签
  painter.setPen(QColor(64, 64, 245));
  painter.setFont(QFont("DejaVu Sans", 5, 50, true));
  QRect labelRect(-30, 0, 60, 16);  // 更小
  painter.drawText(labelRect, Qt::AlignCenter, "SPEED");

  // 速度值
  painter.setPen(QColor(26, 245, 245));
  painter.setFont(QFont("DejaVu Sans", 16, 60, true));  // 字体缩小
  QRect valueRect(-30, 18, 60, 26);  // 更小
  painter.drawText(valueRect, Qt::AlignCenter,
                  QString("%1").arg(_speed, 3, '0'));

  // 单位
  painter.setPen(QColor(26, 245, 245));
  painter.setFont(QFont("DejaVu Sans", 7, 63, true));
  QRect unitRect(28, 38, 30, 15);  // 与数字相对靠右下
  painter.drawText(unitRect, Qt::AlignLeft | Qt::AlignBottom,
                  "cm/s");

  painter.restore();

}


void DashBoard::draw_gear(QPainter& painter) {
  static QRect gear_rect(0, 0, 80, 80);
  static QRect suffix_rect(48, 48, 32, 32);
  static QFont suffix_font("DejaVu Sans", 16, 63, true);

  painter.save();
  painter.setPen(QPen(QColor(26, 245, 245), 1, Qt::SolidLine));
  painter.setFont(QFont("DejaVu Sans", 48, 63, true));

  switch (_gear) {
    case kGear_1:
      painter.drawText(gear_rect, Qt::AlignCenter, QString::number(_gear));
      painter.setFont(suffix_font);
      painter.drawText(suffix_rect, Qt::AlignCenter, "st");
      break;
    case kGear_2:
      painter.drawText(gear_rect, Qt::AlignCenter, QString::number(_gear));
      painter.setFont(suffix_font);
      painter.drawText(suffix_rect, Qt::AlignCenter, "nd");
      break;
    case kGear_3:
      painter.drawText(gear_rect, Qt::AlignCenter, QString::number(_gear));
      painter.setFont(suffix_font);
      painter.drawText(suffix_rect, Qt::AlignCenter, "rd");
      break;
    case kGear_4:
    case kGear_5:
    case kGear_6:
    case kGear_7:
    case kGear_8:
      painter.drawText(gear_rect, Qt::AlignCenter, QString::number(_gear));
      painter.setFont(suffix_font);
      painter.drawText(suffix_rect, Qt::AlignCenter, "th");
      break;
    case kGear_D:
      painter.drawText(gear_rect, Qt::AlignCenter, "D");
      break;
    case kGear_N:
      painter.drawText(gear_rect, Qt::AlignCenter, "N");
      break;
    case kGear_P:
      painter.drawText(gear_rect, Qt::AlignCenter, "P");
      break;
    case kGear_R:
      painter.drawText(gear_rect, Qt::AlignCenter, "R");
      break;
    default:
      break;
  }

  painter.restore();
}

void DashBoard::draw_thermometer(QPainter& painter) {
  painter.save();

  painter.drawImage(QRect(115, -60, 8, 16),
                    QImage("://images/temperature-icon.png"));

  painter.translate(-160, 100);

  static QColor normal_color(26, 245, 245, 245);
  static QColor overrun_color(245, 64, 64, 225);

  // 绘制表盘外檐
  painter.save();
  painter.setPen(QPen(normal_color, 1, Qt::SolidLine));
  QRect rect(-300, -300, 600, 600);
  painter.drawArc(rect, 12 * 16, 20 * 16);
  painter.restore();

  // 绘制刻度
  painter.save();
  painter.setPen(QPen(normal_color, 1, Qt::SolidLine));
  painter.rotate(-12);
  painter.drawLine(300, 0, 306, 0);
  painter.rotate(-10);
  painter.drawLine(300, 0, 304, 0);
  painter.rotate(-10);
  painter.drawLine(300, 0, 306, 0);
  painter.restore();

  // 绘制刻度值
  painter.save();
  painter.setPen(normal_color);
  painter.setFont(QFont("DejaVu Sans", 6));

  painter.rotate(-12);
  painter.save();
  painter.translate(316, 0);
  painter.rotate(12);
  painter.drawText(QRect(-20, -10, 40, 20), Qt::AlignCenter,
                   QString::number(0) + "°C");
  painter.restore();

  painter.rotate(-10);
  painter.save();
  painter.translate(317, 0);
  painter.rotate(22);
  painter.drawText(QRect(-20, -10, 40, 20), Qt::AlignCenter,
                   QString::number(50) + "°C");
  painter.restore();

  painter.rotate(-10);
  painter.save();
  painter.translate(320, 0);
  painter.rotate(32);
  painter.drawText(QRect(-20, -10, 40, 20), Qt::AlignCenter,
                   QString::number(100) + "°C");
  painter.restore();

  painter.restore();

  // 绘制红色区域
  painter.save();
  static QRectF rectangle_outer(-304, -304, 608, 608);
  static QRectF rectangle_inner(-300.5, -300.5, 601, 601);
  painter.setBrush(overrun_color);
  QPainterPath path;
  path.arcTo(rectangle_outer, 28, 3.9);
  path.arcTo(rectangle_inner, 31.9, -3.9);
  painter.drawPath(path);
  painter.restore();

  // 绘制指针
  painter.save();
  painter.setPen(QPen(overrun_color, 1, Qt::SolidLine));
  painter.rotate(-12 - 0.2 * _temperature);
  painter.drawLine(298, 0, 306, 0);
  painter.restore();

  painter.restore();
}

void DashBoard::draw_oil_meter(QPainter& painter) {
  painter.save();

  painter.drawImage(QRect(-130, -60, 16, 16),
                    QImage("://images/fuel-icon.png"));

  painter.translate(160, 100);
  painter.rotate(180);

  static QColor normal_color(26, 245, 245, 245);
  static QColor overrun_color(245, 64, 64, 225);

  // 绘制表盘外檐
  painter.save();
  painter.setPen(QPen(normal_color, 1, Qt::SolidLine));
  QRect rect(-300, -300, 600, 600);
  painter.drawArc(rect, -12 * 16, -20 * 16);
  painter.restore();

  // 绘制刻度
  painter.save();
  painter.setPen(QPen(normal_color, 1, Qt::SolidLine));
  painter.rotate(12);
  painter.drawLine(300, 0, 306, 0);
  painter.rotate(10);
  painter.drawLine(300, 0, 304, 0);
  painter.rotate(10);
  painter.drawLine(300, 0, 306, 0);
  painter.restore();

  // 绘制刻度值
  painter.save();
  painter.setPen(normal_color);
  painter.setFont(QFont("DejaVu Sans", 6));

  painter.rotate(12);
  painter.save();
  painter.translate(316, 0);
  painter.rotate(168);
  painter.drawText(QRect(-20, -10, 40, 20), Qt::AlignCenter,
                   QString::number(0) + "%");
  painter.restore();

  painter.rotate(10);
  painter.save();
  painter.translate(317, 0);
  painter.rotate(158);
  painter.drawText(QRect(-20, -10, 40, 20), Qt::AlignCenter,
                   QString::number(50) + "%");
  painter.restore();

  painter.rotate(10);
  painter.save();
  painter.translate(320, 0);
  painter.rotate(148);
  painter.drawText(QRect(-20, -10, 40, 20), Qt::AlignCenter,
                   QString::number(100) + "%");
  painter.restore();

  painter.restore();

  // 绘制红色区域
  painter.save();
  static QRectF rectangle_outer(-304, -304, 608, 608);
  static QRectF rectangle_inner(-300.5, -300.5, 601, 601);
  painter.setBrush(overrun_color);
  QPainterPath path;
  path.arcTo(rectangle_outer, -12.1, -3.9);
  path.arcTo(rectangle_inner, -16, 3.9);
  painter.drawPath(path);
  painter.restore();

  // 绘制指针
  painter.save();
  painter.setPen(QPen(overrun_color, 1, Qt::SolidLine));
  painter.rotate(12 + 0.2 * _oil);
  painter.drawLine(298, 0, 306, 0);
  painter.restore();

  painter.restore();
}
