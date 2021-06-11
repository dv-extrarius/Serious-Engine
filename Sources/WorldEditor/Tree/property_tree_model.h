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

#ifndef PROPERTY_TREE_MODEL_H
#define PROPERTY_TREE_MODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

#include <set>

class CEntity;
class BasePropertyTreeItem;

class PropertyTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
  explicit PropertyTreeModel(QObject *parent = nullptr);

  void          Clear();
  void          Fill(const std::set<CEntity*>& curr_selection);
  QWidget*      CreateEditor(const QModelIndex& index, QWidget* parent);
  CPropertyID*  GetSelectedProperty(const QModelIndexList& model_indices) const;

  int           rowCount(const QModelIndex& parent = QModelIndex()) const override;
  int           columnCount(const QModelIndex& parent = QModelIndex()) const override;

  QVariant      data(const QModelIndex& index, int role) const override;
  Qt::ItemFlags flags(const QModelIndex& index) const override;
  QVariant      headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  QModelIndex   index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
  QModelIndex   parent(const QModelIndex& index) const override;

private:
  void          _AddEntityProperties(const QModelIndex& parent, const std::set<CEntity*>& entities);
  void          _FillSubProperties(const QModelIndex& parent, const std::set<CEntity*>& entities);
  void          _AppendItem(std::unique_ptr<BasePropertyTreeItem>&& item, BasePropertyTreeItem& parent);

private:
  std::unique_ptr<BasePropertyTreeItem> mp_header_item;
};

#endif
