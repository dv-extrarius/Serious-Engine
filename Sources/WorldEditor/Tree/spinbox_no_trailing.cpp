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
#include "spinbox_no_trailing.h"

namespace
{
  const char* g_spin_style = R"(
QDoubleSpinBox {
  background-color: transparent;
  border: 0px;
}
)";
}

SpinBoxNoTrailing::SpinBoxNoTrailing(QWidget* parent)
  : QDoubleSpinBox(parent)
{
  setStyleSheet(g_spin_style);
}

QString SpinBoxNoTrailing::textFromValue(double value) const
{
  QString text = QDoubleSpinBox::textFromValue(value);
  int num_trailing_zeros = 0;
  for (int i = 0; i < text.length(); ++i)
  {
    auto c = text.at(text.length() - i - 1);
    if (c == '0')
    {
      ++num_trailing_zeros;
      continue;
    }
    if (c == locale().decimalPoint())
      ++num_trailing_zeros;
    break;
  }
  text.chop(num_trailing_zeros);
  if (text.isEmpty())
    text = "0";
  return text;
}
