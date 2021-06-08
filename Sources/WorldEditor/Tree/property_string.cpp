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
#include "../EventHub.h"

#include <QLineEdit>

Property_String::Property_String(CEntity* entity, CEntityProperty* prop, BasePropertyTreeItem* parent)
  : BasePropertyTreeItem(parent)
  , mp_entity(entity)
  , mp_property(prop)
{
}

QVariant Property_String::data(int column, int role) const
{
  if (role != Qt::DisplayRole || (column != 0 && column != 2))
    return QVariant();

  if (column == 0)
    return QString(mp_property->ep_strName);

  return QString("CTString");
}

bool Property_String::editable() const
{
  return true;
}

QWidget* Property_String::CreateEditor(QWidget* parent)
{
  QObject::disconnect(m_editor_connection);
  auto* editor = new QLineEdit(parent);
  editor->setStyleSheet("background-color: transparent;");
  editor->setText(ENTITYPROPERTY(mp_entity, mp_property->ep_slOffset, CTString).str_String);

  m_editor_connection = QObject::connect(editor, &QLineEdit::editingFinished, [this, editor]
    {
      mp_entity->End();
      ENTITYPROPERTY(mp_entity, mp_property->ep_slOffset, CTString) = editor->text().toLocal8Bit().data();
      mp_entity->Initialize();

      CWorldEditorDoc* pDoc = theApp.GetDocument();
      pDoc->SetModifiedFlag(TRUE);
      pDoc->UpdateAllViews(NULL);
      // mark that document changed so that OnIdle on CSG destination combo would
      // refresh combo entries (because we could be changing world name)
      pDoc->m_chDocument.MarkChanged();

      EventHub::instance().PropertyChanged({ mp_entity }, mp_property);
      Changed();
    });
  return editor;
}

/*******************************************************************************************/
static UIPropertyFactory::Registrar g_registrar(CEntityProperty::PropertyType::EPT_STRING,
  [](CEntity* entity, CEntityProperty* prop, BasePropertyTreeItem* parent)
  {
    return new Property_String(entity, prop, parent);
  });
static UIPropertyFactory::Registrar g_registrar_translatable(CEntityProperty::PropertyType::EPT_STRINGTRANS,
  [](CEntity* entity, CEntityProperty* prop, BasePropertyTreeItem* parent)
  {
    return new Property_String(entity, prop, parent);
  });
