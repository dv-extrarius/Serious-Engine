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
#ifndef PROPERTY_BOOL_H
#define PROPERTY_BOOL_H

#include "base_entity_property_tree_item.h"

#include <QPointer>
#include <QCheckBox>

class Property_Bool : public BaseEntityPropertyTreeItem
{
public:
  Property_Bool(BasePropertyTreeItem* parent);

  QWidget* CreateEditor(QWidget* parent) override;

  IMPL_GENERIC_PROPERTY_FUNCTIONS(BOOL)

private:
  QMetaObject::Connection m_editor_connection;
};

#endif
