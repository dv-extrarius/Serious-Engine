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
#include "clickable_label.h"

namespace
{ 
  // copied from PropertyComboBox.cpp
  void _JoinProperties(std::list<std::unique_ptr<CPropertyID>>& properties, CEntity* penEntity, BOOL bIntersect)
  {
    // if we should add all of this entity's properties (if this is first entity)
    if (!bIntersect)
    {
      // obtain entity class ptr
      CDLLEntityClass* pdecDLLClass = penEntity->GetClass()->ec_pdecDLLClass;
      // for all classes in hierarchy of this entity
      for (; pdecDLLClass != NULL; pdecDLLClass = pdecDLLClass->dec_pdecBase)
      {
        // for all properties
        for (INDEX iProperty = 0; iProperty < pdecDLLClass->dec_ctProperties; iProperty++)
        {
          CEntityProperty& epProperty = pdecDLLClass->dec_aepProperties[iProperty];
          // dont add properties with no name
          if (epProperty.ep_strName != CTString(""))
          {
            CAnimData* pAD = NULL;
            if (epProperty.ep_eptType == CEntityProperty::EPT_ANIMATION)
              pAD = penEntity->GetAnimData(epProperty.ep_slOffset);

            properties.push_back(std::make_unique<CPropertyID>(epProperty.ep_strName, epProperty.ep_eptType, &epProperty, pAD));
          }
        }
      }
    }
    // in case of intersecting properties we should take one of existing properties in list
    // and see if investigating entity has property with same descriptive name
    // If not, remove it that existing property.
    else
    {
      for (auto itProp = properties.begin(); itProp != properties.end();)
      {
        CTString strCurrentName = (*itProp)->pid_strName;
        CEntityProperty::PropertyType eptCurrentType = (*itProp)->pid_eptType;
        // mark that property with same name is not found
        BOOL bSameFound = FALSE;

        // obtain entity class ptr
        CDLLEntityClass* pdecDLLClass = penEntity->GetClass()->ec_pdecDLLClass;
        // for all classes in hierarchy of this entity
        for (; pdecDLLClass != NULL; pdecDLLClass = pdecDLLClass->dec_pdecBase)
        {
          // for all properties
          for (INDEX iProperty = 0; iProperty < pdecDLLClass->dec_ctProperties; iProperty++)
          {
            CEntityProperty& epProperty = pdecDLLClass->dec_aepProperties[iProperty];
            CAnimData* pAD = NULL;
            // remember anim data
            if (epProperty.ep_eptType == CEntityProperty::EPT_ANIMATION)
            {
              pAD = penEntity->GetAnimData(epProperty.ep_slOffset);
            }

            // create current CPropertyID
            CPropertyID PropertyID = CPropertyID(epProperty.ep_strName, epProperty.ep_eptType,
              &epProperty, pAD);

            // is this property same as one we are investigating
            if ((strCurrentName == PropertyID.pid_strName) &&
              (eptCurrentType == PropertyID.pid_eptType))
            {
              // if propperty is enum, enum ptr must also be the same
              if ((*itProp)->pid_eptType == CEntityProperty::EPT_ENUM)
              {
                // only then,
                if ((*itProp)->pid_penpProperty->ep_pepetEnumType ==
                  PropertyID.pid_penpProperty->ep_pepetEnumType)
                {
                  // same property is found
                  bSameFound = TRUE;
                }
                else
                {
                  bSameFound = FALSE;
                }
                goto pcb_OutLoop_JoinProperties;
              }
              // if propperty is animation, anim data ptr must be the same
              else if ((*itProp)->pid_eptType == CEntityProperty::EPT_ANIMATION)
              {
                if ((*itProp)->pid_padAnimData == PropertyID.pid_padAnimData)
                {
                  // same property is found
                  bSameFound = TRUE;
                }
                else
                {
                  bSameFound = FALSE;
                }
                goto pcb_OutLoop_JoinProperties;
              }
              else
              {
                // same property is found
                bSameFound = TRUE;
                goto pcb_OutLoop_JoinProperties;
              }
            }
          }
        }
      pcb_OutLoop_JoinProperties:;
        // if property with same name is not found
        if (!bSameFound)
          // remove our investigating property from list
          // and delete it
          itProp = properties.erase(itProp);
        else
          ++itProp;
      }
    }
  }

  std::list<std::unique_ptr<CPropertyID>> _CollectProperties(const std::set<CEntity*>& entities)
  {
    std::list<std::unique_ptr<CPropertyID>> properties;
    if (!entities.empty())
    {
      auto beg_it = entities.begin();
      auto cur_it = beg_it;
      _JoinProperties(properties, *beg_it, FALSE);
      for (++cur_it; cur_it != entities.end(); ++cur_it)
        _JoinProperties(properties, *cur_it, TRUE);

      properties.push_back(std::make_unique<CPropertyID>("Spawn flags", CEntityProperty::EPT_SPAWNFLAGS, nullptr, nullptr));
      properties.push_back(std::make_unique<CPropertyID>("Parent", CEntityProperty::EPT_PARENT, nullptr, nullptr));
    }
    return properties;
  }

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
} // anonymous namespace


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
  auto* entity_item = dynamic_cast<BaseEntityPropertyTreeItem*>(item);
  if (!entity_item)
    return nullptr;

  if (entity_item->ValueIsCommonForAllEntities())
  {
    return entity_item->CreateEditor(parent);
  } else {
    auto* mixed_editor = new ClickableLabel("(mixed)", parent);
    QObject::connect(mixed_editor, &ClickableLabel::clicked, entity_item,
      [this, entity_item]
      {
        entity_item->SetFirstValueToAllEntities();
      });
    return mixed_editor;
  }
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
      dataChanged(createIndex(row, 0, item_raw), createIndex(row, 2, item_raw));
    });
  parent.appendChild(std::move(item));
}

void PropertyTreeModel::Fill(const std::set<CEntity*>& curr_selection)
{
  beginResetModel();
  Clear();
  if (curr_selection.empty())
    return;

  QModelIndex root_index;
  _AddEntityProperties(root_index, curr_selection);
}

void PropertyTreeModel::_AddEntityProperties(const QModelIndex& parent, const std::set<CEntity*>& entities)
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

void PropertyTreeModel::_FillSubProperties(const QModelIndex& parent, const std::set<CEntity*>& entities)
{
  BasePropertyTreeItem* parent_item = static_cast<BasePropertyTreeItem*>(parent.internalPointer());

  int starting_row = parent_item->childCount();
  int inserted_rows = 0;

  auto common_properties = _CollectProperties(entities);

  for (auto it = common_properties.begin(); it != common_properties.end(); ++it)
  {
    std::unique_ptr<CPropertyID>& property = *it;
    if (UIPropertyFactory::Instance().HasFactoryFor(property->pid_eptType))
    {
      std::unique_ptr<BaseEntityPropertyTreeItem> new_item(UIPropertyFactory::Instance().GetFactoryFor(property->pid_eptType)(parent_item));
      new_item->_SetEntitiesAndProperty(entities, std::move(property));
      _AppendItem(std::move(new_item), *parent_item);
      ++inserted_rows;
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
  return item->data(index.column(), role);
}

Qt::ItemFlags PropertyTreeModel::flags(const QModelIndex &index) const
{
  if (!index.isValid())
    return Qt::NoItemFlags;

  return QAbstractItemModel::flags(index);
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
