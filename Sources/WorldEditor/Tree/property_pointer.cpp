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
#include "ui_property_factory.h"
#include "base_entity_property_tree_item.h"

#include <QComboBox>

namespace
{
  const char* g_combo_style = R"(
QComboBox {
  background-color: transparent;
  border: 0px;
}
)";
}

class Property_Pointer : public BaseEntityPropertyTreeItem
{
public:
  Property_Pointer(BasePropertyTreeItem* parent)
    : BaseEntityPropertyTreeItem(parent)
  {
    QObject::connect(&EventHub::instance(), &EventHub::PropertyChanged, this,
      [this](const std::set<CEntity*>& entities, CPropertyID* prop, BasePropertyTreeItem* source)
      {
        if (source == this)
          return;

        if (prop->pid_eptType == CEntityProperty::EPT_STRING || prop->pid_eptType == CEntityProperty::EPT_STRINGTRANS)
        {
          std::set<CEntity*> pointers;
          for (auto* entity : m_entities)
          {
            CEntityProperty* actual_property = entity->PropertyForName(mp_property->pid_strName);
            pointers.insert(ENTITYPROPERTY(entity, actual_property->ep_slOffset, CEntityPointer).ep_pen);
          }

          std::vector<CEntity*> common_entities;
          std::set_intersection(entities.begin(), entities.end(),
            pointers.begin(), pointers.end(),
            std::back_inserter(common_entities));
          if (!common_entities.empty())
            Changed();
        }
      });
  }

  QWidget* CreateEditor(QWidget* parent) override final
  {
    QObject::disconnect(m_editor_connection);
    auto* editor = new QComboBox(parent);
    editor->setStyleSheet(g_combo_style);

    const CEntity* first_ptr = _CurrentPropValue().ep_pen;
    if (first_ptr)
      editor->addItem(first_ptr->GetName().str_String, true);
    editor->addItem("(none)", false);
    editor->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
    editor->installEventFilter(this);

    m_editor_connection = QObject::connect(editor, QOverload<int>::of(&QComboBox::currentIndexChanged), [this, editor]
      (int index)
      {
        // if (none) selected, drop pointer
        if (index != -1 && !editor->itemData(index).toBool())
          _WriteProperty(nullptr);
      });

    return editor;
  }

  void OnEntityPicked(CEntity* picked_entity) override final
  {
    if (picked_entity && picked_entity->IsTargetable())
    {
      for (auto* entity : m_entities)
      {
        auto* actual_property = entity->PropertyForName(mp_property->pid_strName);
        if (!entity->IsTargetValid(actual_property->ep_slOffset, picked_entity))
          return;
      }
      _WriteProperty(picked_entity);
    }
  }

  IMPL_GENERIC_PROPERTY_FUNCTIONS_IMPL(CEntityPointer, nullptr)

private:
  bool eventFilter(QObject* object, QEvent* event) override
  {
    if (event->type() == QEvent::Wheel)
    {
      event->ignore();
      return true;
    }
    return BaseEntityPropertyTreeItem::eventFilter(object, event);
  }

private:
  QMetaObject::Connection m_editor_connection;
};

/*******************************************************************************************/
static UIPropertyFactory::Registrar g_registrar(CEntityProperty::PropertyType::EPT_ENTITYPTR,
  [](BasePropertyTreeItem* parent)
  {
    return new Property_Pointer(parent);
  });
