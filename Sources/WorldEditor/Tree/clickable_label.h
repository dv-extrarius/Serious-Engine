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
#ifndef CLICKABLE_LABEL_H
#define CLICKABLE_LABEL_H

#include <QLabel>

class ClickableLabel : public QLabel
{
  Q_OBJECT

public:
  explicit ClickableLabel(const QString& text, QWidget* parent = nullptr);

  Q_SIGNAL void clicked();

protected:
  void mousePressEvent(QMouseEvent* event) override;
};

#endif
