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
#ifndef PROPERTY_ITEM_DELEGATE_H
#define PROPERTY_ITEM_DELEGATE_H

#include <QStyledItemDelegate>
#include <QPointer>

class QTreeView;

class PropertyItemDelegate : public QStyledItemDelegate
{
  Q_OBJECT
public:
  explicit PropertyItemDelegate(const QTreeView* tree_view, QObject* parent = nullptr);
  void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;

private:
  QPointer<const QTreeView> mp_tree_view;
};

#endif
