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

#include <string>
#include <stdexcept>

UIPropertyFactory::UIPropertyFactory()
{
}

UIPropertyFactory& UIPropertyFactory::Instance()
{
  static UIPropertyFactory instance;
  return instance;
}

void UIPropertyFactory::Register(CEntityProperty::PropertyType prop_type, TFactory&& factory)
{
  auto [inserted_pos, was_new] = m_factories.emplace(prop_type, std::move(factory));
  if (!was_new)
    throw std::runtime_error(std::string("Factory for ") + std::to_string(static_cast<int>(prop_type)) + " already registered!");
}

const UIPropertyFactory::TFactory& UIPropertyFactory::GetFactoryFor(CEntityProperty::PropertyType prop_type) const
{
  auto found_pos = m_factories.find(prop_type);
  if (found_pos == m_factories.end())
    throw std::runtime_error(std::string("Factory for ") + std::to_string(static_cast<int>(prop_type)) + " is not registered!");

  return found_pos->second;
}

bool UIPropertyFactory::HasFactoryFor(CEntityProperty::PropertyType prop_type) const
{
  return m_factories.find(prop_type) != m_factories.end();
}

UIPropertyFactory::Registrar::Registrar(CEntityProperty::PropertyType prop_type, TFactory&& factory)
{
  UIPropertyFactory::Instance().Register(prop_type, std::move(factory));
}
