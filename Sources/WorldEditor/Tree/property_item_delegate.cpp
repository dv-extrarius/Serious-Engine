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
#include "property_item_delegate.h"
#include "property_item_delegate.h.moc"
#include "property_tree_model.h"

#include <QTreeView>
#include <QPainter>
#include <QPen>

PropertyItemDelegate::PropertyItemDelegate(const QTreeView* tree_view, QObject* parent)
  : QStyledItemDelegate(parent)
  , mp_tree_view(tree_view)
{
}

void PropertyItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  if (mp_tree_view)
  {
    painter->save();

    QPen pen;
    pen.setColor(QColor(0xE5E5E5));
    pen.setWidth(1);
    painter->setPen(pen);
    painter->drawLine(0, option.rect.bottom(), mp_tree_view->width(), option.rect.bottom());
    painter->drawLine(option.rect.right(), 0, option.rect.right(), mp_tree_view->height());

    painter->restore();
  }
  QStyledItemDelegate::paint(painter, option, index);
}
