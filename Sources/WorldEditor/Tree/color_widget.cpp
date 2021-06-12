/* Copyright (c) 2021 SeriousAlexej (Oleksii Sierov).
This program is free software; you can redistribute it and/or modify
it under the terms of version 2 of the GNU General Public License as published by
the Free Software Foundation


This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. */

#include "StdAfx.h"
#include "color_widget.h"
#include "color_widget.h.moc"

#include <QMouseEvent>
#include <QPainter>
#include <QToolTip>

ColorWidget::ColorWidget(QColor col, QWidget* parent)
  : QWidget(parent)
  , m_col(col)
  , m_mode(EMode::Idle)
{
  setAttribute(Qt::WA_Hover);
  setMouseTracking(true);
}

void ColorWidget::SetColor(QColor col)
{
  if (m_col != col)
  {
    m_col = col;
    update();
  }
}

QColor ColorWidget::GetColor() const
{
  return m_col;
}

void ColorWidget::mousePressEvent(QMouseEvent* event)
{
  if (event->buttons() == Qt::LeftButton)
  {
    if (rectColor().contains(event->pos()))
    {
      clicked();
    }
    else
    {
      m_edit_start_pos = QCursor::pos();
      if (rectHue().contains(event->pos()))
        m_mode = EMode::H;
      else if (rectSaturation().contains(event->pos()))
        m_mode = EMode::S;
      else if (rectValue().contains(event->pos()))
        m_mode = EMode::V;
      else
        m_mode = EMode::A;
      setCursor(Qt::BlankCursor);
      editingStarted();
    }
  }
}

void ColorWidget::mouseReleaseEvent(QMouseEvent* event)
{
  m_mode = EMode::Idle;
  setCursor(Qt::ArrowCursor);
  editingFinished();
}

void ColorWidget::mouseMoveEvent(QMouseEvent* event)
{
  if (m_mode == EMode::Idle)
    return;

  const auto delta = QCursor::pos().x() - m_edit_start_pos.x();
  QCursor::setPos(m_edit_start_pos);

  int hue = qBound(0, m_col.hue(), 359);
  int saturation = m_col.saturation();
  int value = m_col.value();
  int alpha = m_col.alpha();
  switch (m_mode)
  {
  case EMode::H:
    hue += delta;
    break;
  case EMode::S:
    saturation += delta;
    break;
  case EMode::V:
    value += delta;
    break;
  case EMode::A:
    alpha += delta;
    break;
  default:
    break;
  }

  QColor prev_col = m_col;
  m_col = QColor::fromHsv(qBound(0, hue, 359), qBound(0, saturation, 255), qBound(0, value, 255), qBound(0, alpha, 255));
  if (m_col != prev_col)
    colorChanged();

  update();
  showTooltip();
}

void ColorWidget::paintEvent(QPaintEvent*)
{
  QPainter painter(this);

  painter.setPen(Qt::black);
  const int hue = qBound(0, m_col.hue(), 359);
  for (const auto& [rect, col] : {
    std::make_pair(rectHue(), QColor::fromHsv(hue, 255, 255)),
    std::make_pair(rectSaturation(), QColor::fromHsv(hue, m_col.saturation(), 255)),
    std::make_pair(rectValue(), QColor::fromHsv(0, 0, m_col.value())),
    std::make_pair(rectAlpha(), QColor::fromHsv(0, 0, m_col.alpha(), 255)),
    std::make_pair(rectColor(), QColor::fromRgb(m_col.rgb())) })
  {
    painter.fillRect(rect, col);
    painter.drawRect(rect);
  }
}

bool ColorWidget::event(QEvent* event)
{
  if (event->type() == QEvent::ToolTip)
  {
    showTooltip();
    return true;
  }
  return QWidget::event(event);
}

void ColorWidget::showTooltip() const
{
  QString color_string = QString("RGBA: %1,%2,%3,%4   ").arg(m_col.red()).arg(m_col.green()).arg(m_col.blue()).arg(m_col.alpha());
  color_string += QString("HSV: %1,%2,%3").arg(m_col.hue() >= 0 ? m_col.hue() : 0).arg(m_col.saturation()).arg(m_col.value());
  QToolTip::showText(QCursor::pos(), color_string);
}

QRect ColorWidget::rectHue() const
{
  return QRect(0, 0, width() / 6, height()-1);
}

QRect ColorWidget::rectSaturation() const
{
  auto rect_base = rectHue();
  return rect_base.translated(rect_base.width(), 0);
}

QRect ColorWidget::rectValue() const
{
  auto rect_base = rectSaturation();
  return rect_base.translated(rect_base.width(), 0);
}

QRect ColorWidget::rectAlpha() const
{
  auto rect_base = rectValue();
  return rect_base.translated(rect_base.width(), 0);
}

QRect ColorWidget::rectColor() const
{
  auto rect_base = rectAlpha();
  rect_base.translate(rect_base.width(), 0);
  rect_base.setRight(rect().right()-1);
  return rect_base;
}
