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
#include "properties_entity_root.h"
#include "../EventHub.h"

EntityRootProperties::EntityRootProperties(BasePropertyTreeItem* parent, const std::vector<CEntity*>& entities)
  : BasePropertyTreeItem(parent)
  , m_entities(entities)
{
  QObject::connect(&EventHub::instance(), &EventHub::PropertyChanged, this,
    [this](const std::vector<CEntity*>& entities, CEntityProperty* prop)
    {
      if (prop->ep_eptType == CEntityProperty::PropertyType::EPT_STRING ||
          prop->ep_eptType == CEntityProperty::PropertyType::EPT_STRINGTRANS)
      {
        std::vector<CEntity*> common_entities;
        std::set_intersection(entities.begin(), entities.end(),
          m_entities.begin(), m_entities.end(),
          std::back_inserter(common_entities));
        if (!common_entities.empty())
          Changed();
      }
    });
}

QVariant EntityRootProperties::data(int column, int role) const
{
  if (column < 0 || column >= 3)
    return QVariant();

  QString common_value;
  if (column == 0 || column == 2)
  {
    common_value = m_entities.front()->GetClass()->ec_pdecDLLClass->dec_strName;
    for (size_t i = 1; i < m_entities.size(); ++i)
    {
      if (common_value != m_entities[i]->GetClass()->ec_pdecDLLClass->dec_strName)
      {
        common_value = "(mixed selection)";
        break;
      }
    }
  }
  else if (column == 1)
  {
    common_value = m_entities.front()->GetName();
    for (size_t i = 1; i < m_entities.size(); ++i)
    {
      if (common_value != m_entities[i]->GetName())
      {
        common_value = "(mixed names)";
        break;
      }
    }
  }
  return common_value;
}
