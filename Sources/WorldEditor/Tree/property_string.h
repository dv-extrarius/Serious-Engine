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
#ifndef PROPERTY_STRING_H
#define PROPERTY_STRING_H

#include "base_property_tree_item.h"

#include <QPointer>
#include <QCheckBox>

class Property_String : public BasePropertyTreeItem
{
public:
  Property_String(CEntity* entity, CEntityProperty* prop, BasePropertyTreeItem* parent);

  QVariant data(int column, int role) const override;
  bool     editable() const override;
  QWidget* CreateEditor(QWidget* parent) override;

private:
  CEntity*            mp_entity;
  CEntityProperty*    mp_property;
  QMetaObject::Connection m_editor_connection;
};

#endif
