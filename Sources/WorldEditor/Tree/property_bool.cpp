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
#include "property_bool.h"
#include "ui_property_factory.h"

Property_Bool::Property_Bool(BasePropertyTreeItem* parent)
  : BaseEntityPropertyTreeItem(parent)
{
}

QWidget* Property_Bool::CreateEditor(QWidget* parent)
{
  QObject::disconnect(m_editor_connection);
  auto* editor = new QCheckBox(parent);
  editor->setChecked(_CurrentPropValue() == TRUE);

  m_editor_connection = QObject::connect(editor, &QCheckBox::clicked, [this]
    (bool checked)
    {
      _WriteProperty(checked ? TRUE : FALSE);
    });
  return editor;
}

/*******************************************************************************************/
static UIPropertyFactory::Registrar g_registrar(CEntityProperty::PropertyType::EPT_BOOL,
  [](BasePropertyTreeItem* parent)
  {
    return new Property_Bool(parent);
  });
