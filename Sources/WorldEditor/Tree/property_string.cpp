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
#include "property_string.h"
#include "ui_property_factory.h"

#include <QLineEdit>

Property_String::Property_String(BasePropertyTreeItem* parent)
  : BaseEntityPropertyTreeItem(parent)
{
}

bool Property_String::_ChangesDocument() const
{
  return true;
}

QWidget* Property_String::CreateEditor(QWidget* parent)
{
  QObject::disconnect(m_editor_connection);
  auto* editor = new QLineEdit(parent);
  editor->setStyleSheet("background-color: transparent;border: 0px;");
  editor->setText(_CurrentPropValue().str_String);

  m_editor_connection = QObject::connect(editor, &QLineEdit::editingFinished, [this, editor]
    {
      CTString new_value = editor->text().toLocal8Bit().data();
      _WriteProperty(new_value);
    });
  return editor;
}

/*******************************************************************************************/
static UIPropertyFactory::Registrar g_registrar(CEntityProperty::PropertyType::EPT_STRING,
  [](BasePropertyTreeItem* parent)
  {
    return new Property_String(parent);
  });
static UIPropertyFactory::Registrar g_registrar_translatable(CEntityProperty::PropertyType::EPT_STRINGTRANS,
  [](BasePropertyTreeItem* parent)
  {
    return new Property_String(parent);
  });
