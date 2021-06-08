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
#include "property_tree_model.h"
#include "property_tree_model.h.moc"
#include "base_property_tree_item.h"
#include "properties_entity_root.h"
#include "ui_property_factory.h"

class _HeaderTreeItem : public BasePropertyTreeItem
{
public:
  _HeaderTreeItem()
    : BasePropertyTreeItem(nullptr)
  {
  }

  QVariant data(int column, int role) const override
  {
    if (column < 0 || column > 2)
      return QVariant();

    if (column == 0)
      return "Name";
    if (column == 1)
      return "Value";
    return "Type";
  }
};


PropertyTreeModel::PropertyTreeModel(QObject* parent)
  : QAbstractItemModel(parent)
{
  mp_header_item = std::make_unique<_HeaderTreeItem>();
}

int PropertyTreeModel::columnCount(const QModelIndex&) const
{
  return 3;
}

int PropertyTreeModel::rowCount(const QModelIndex& parent) const
{
  BasePropertyTreeItem* parentItem;
  if (parent.column() > 0)
    return 0;

  if (!parent.isValid())
    parentItem = mp_header_item.get();
  else
    parentItem = static_cast<BasePropertyTreeItem*>(parent.internalPointer());

  return parentItem->childCount();
}

QWidget* PropertyTreeModel::CreateEditor(const QModelIndex& index, QWidget* parent)
{
  if (!index.isValid() || index.column() != 1)
    return nullptr;

  auto* item = static_cast<BasePropertyTreeItem*>(index.internalPointer());
  if (!item->editable())
    return nullptr;
  return item->CreateEditor(parent);
}

void PropertyTreeModel::Clear()
{
  beginResetModel();
  mp_header_item->Clear();
  endResetModel();
}

void PropertyTreeModel::_AppendItem(std::unique_ptr<BasePropertyTreeItem>&& item, BasePropertyTreeItem& parent)
{
  auto* item_raw = item.get();
  QObject::connect(item_raw, &BasePropertyTreeItem::Changed,
    [this, item_raw]
    {
      int row = item_raw->row();
      dataChanged(createIndex(row, 0, item_raw->parentItem()), createIndex(row, 2, item_raw->parentItem()));
    });
  parent.appendChild(std::move(item));
}

void PropertyTreeModel::Fill(const std::vector<CEntity*>& curr_selection)
{
  beginResetModel();
  Clear();
  if (curr_selection.empty())
    return;

  QModelIndex root_index;
  auto selection_sorted = curr_selection;
  std::sort(selection_sorted.begin(), selection_sorted.end());
  _AddEntityProperties(root_index, selection_sorted);
}

void PropertyTreeModel::_AddEntityProperties(const QModelIndex& parent, const std::vector<CEntity*>& entities)
{
  BasePropertyTreeItem* parent_item = mp_header_item.get();
  if (parent.isValid())
    parent_item = static_cast<BasePropertyTreeItem*>(parent.internalPointer());

  int starting_row = parent_item->childCount();
  _AppendItem(std::make_unique<EntityRootProperties>(parent_item, entities), *parent_item);
  beginInsertRows(parent, starting_row, starting_row);
  endInsertRows();

  _FillSubProperties(index(starting_row, 0, parent), entities);
}

void PropertyTreeModel::_FillSubProperties(const QModelIndex& parent, const std::vector<CEntity*>& entities)
{
  BasePropertyTreeItem* parent_item = static_cast<BasePropertyTreeItem*>(parent.internalPointer());

  /*
  * collect common properties from entities here
  * 
  * 
    // obtain entity class ptr
    CDLLEntityClass *pdecDLLClass = penEntity->GetClass()->ec_pdecDLLClass;
    // for all classes in hierarchy of this entity
    for(;pdecDLLClass!=NULL; pdecDLLClass = pdecDLLClass->dec_pdecBase)
    {
      // for all properties
      for(INDEX iProperty=0; iProperty<pdecDLLClass->dec_ctProperties; iProperty++)
      {
        CEntityProperty &epProperty = pdecDLLClass->dec_aepProperties[iProperty];
        // don't add properties with no name
        if( epProperty.ep_strName != CTString("") )
        {
          CAnimData *pAD = NULL;
          // remember anim data
          if( epProperty.ep_eptType == CEntityProperty::EPT_ANIMATION)
          {
            pAD = penEntity->GetAnimData( epProperty.ep_slOffset);
          }
          // create current CPropertyID
          CPropertyID *pPropertyID = new CPropertyID( epProperty.ep_strName,
            epProperty.ep_eptType, &epProperty, pAD);
          // if we should add all of this entity's properties (if this is first entity)
          // and add it into list
          m_lhProperties.AddTail( pPropertyID->pid_lnNode);
        }
      }
    }
  * 
  * 
  */
  int starting_row = parent_item->childCount();
  int inserted_rows = 0;

  CEntity* penEntity = entities.front();
  CDLLEntityClass* pdecDLLClass = penEntity->GetClass()->ec_pdecDLLClass;
  for (; pdecDLLClass; pdecDLLClass = pdecDLLClass->dec_pdecBase)
  {
    for (INDEX iProperty = 0; iProperty < pdecDLLClass->dec_ctProperties; iProperty++)
    {
      CEntityProperty& epProperty = pdecDLLClass->dec_aepProperties[iProperty];
      // don't add properties with no name
      if (!(epProperty.ep_strName != CTString("")))
        continue;

      if (UIPropertyFactory::Instance().HasFactoryFor(epProperty.ep_eptType))
      {
        std::unique_ptr<BasePropertyTreeItem> new_item(UIPropertyFactory::Instance().GetFactoryFor(epProperty.ep_eptType)(penEntity, &epProperty, parent_item));
        _AppendItem(std::move(new_item), *parent_item);
        ++inserted_rows;
      }
    }
  }
  if (inserted_rows > 0)
  {
    beginInsertRows(parent, starting_row, starting_row + inserted_rows - 1);
    endInsertRows();
  }
}

QVariant PropertyTreeModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (role != Qt::DisplayRole && role != Qt::EditRole)
    return QVariant();

  BasePropertyTreeItem* item = static_cast<BasePropertyTreeItem*>(index.internalPointer());
  if (role == Qt::EditRole && !item->editable())
    return QVariant();
  return item->data(index.column(), role);
}

Qt::ItemFlags PropertyTreeModel::flags(const QModelIndex &index) const
{
  if (!index.isValid())
    return Qt::NoItemFlags;

  auto item_flags = QAbstractItemModel::flags(index);
  if (static_cast<BasePropertyTreeItem*>(index.internalPointer())->editable())
    item_flags |= Qt::ItemIsEditable;
  return item_flags;
}

QVariant PropertyTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    return mp_header_item->data(section, role);

  return QVariant();
}

QModelIndex PropertyTreeModel::index(int row, int column, const QModelIndex &parent) const
{
  if (!hasIndex(row, column, parent))
    return QModelIndex();

  BasePropertyTreeItem *parentItem;

  if (!parent.isValid())
    parentItem = mp_header_item.get();
  else
    parentItem = static_cast<BasePropertyTreeItem*>(parent.internalPointer());

  BasePropertyTreeItem *childItem = parentItem->child(row);
  if (childItem)
    return createIndex(row, column, childItem);
  return QModelIndex();
}

QModelIndex PropertyTreeModel::parent(const QModelIndex &index) const
{
  if (!index.isValid())
    return QModelIndex();

  BasePropertyTreeItem *childItem = static_cast<BasePropertyTreeItem*>(index.internalPointer());
  BasePropertyTreeItem *parentItem = childItem->parentItem();

  if (!parentItem || parentItem == mp_header_item.get())
    return QModelIndex();

  return createIndex(parentItem->row(), 0, parentItem);
}
