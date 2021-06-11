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

#include <QDoubleSpinBox>

namespace
{
  const char* g_spin_style = R"(
QDoubleSpinBox {
  background-color: transparent;
  border: 0px;
}
)";

  class _SpinBox : public QDoubleSpinBox
  {
  public:
    explicit _SpinBox(QWidget* parent) : QDoubleSpinBox(parent) {}

    QString textFromValue(double value) const override
    {
      QString text = QDoubleSpinBox::textFromValue(value);
      int num_trailing_zeros = 0;
      for (int i = 0; i < text.length(); ++i)
      {
        auto c = text.at(text.length() - i - 1);
        if (c == '0')
        {
          ++num_trailing_zeros;
          continue;
        }
        if (c == locale().decimalPoint())
          ++num_trailing_zeros;
        break;
      }
      text.chop(num_trailing_zeros);
      if (text.isEmpty())
        text = "0";
      return text;
    }
  };
}

class Property_Float_Base : public BaseEntityPropertyTreeItem
{
public:
  Property_Float_Base(BasePropertyTreeItem* parent)
    : BaseEntityPropertyTreeItem(parent)
  {
  }

  QWidget* CreateEditor(QWidget* parent) override
  {
    auto* editor = new _SpinBox(parent);
    editor->setStyleSheet(g_spin_style);
    editor->setRange(-99999999, 99999999);
    editor->setDecimals(4);
    editor->setSingleStep(0.25);
    editor->setValue(_WrappedValue(_CurrentPropValueT<FLOAT>()));

    QObject::connect(editor, &QDoubleSpinBox::editingFinished, this, [this, editor]
      {
        _WritePropertyT<FLOAT>(static_cast<FLOAT>(editor->value()));
      });

    _CustomizeEditor(editor);

    return editor;
  }

protected:
  virtual double _WrappedValue(FLOAT value)
  {
    return static_cast<double>(value);
  }

  virtual void _CustomizeEditor(QDoubleSpinBox* editor)
  {
    (void)editor;
  }
};

/*******************************************************************************************/
class Property_Float : public Property_Float_Base
{
public:
  Property_Float(BasePropertyTreeItem* parent)
    : Property_Float_Base(parent)
  {
  }

  IMPL_GENERIC_PROPERTY_FUNCTIONS(FLOAT)
};

/*******************************************************************************************/
class Property_Angle : public Property_Float_Base
{
public:
  Property_Angle(BasePropertyTreeItem* parent)
    : Property_Float_Base(parent)
  {
  }

  IMPL_GENERIC_PROPERTY_FUNCTIONS(ANGLE)

protected:
  double _WrappedValue(FLOAT value) override
  {
    return static_cast<double>(DegAngle(value));
  }

  void _CustomizeEditor(QDoubleSpinBox* editor) override
  {
    editor->setSuffix("°");
    editor->setSingleStep(1.0);
  }
};

/*******************************************************************************************/
class Property_Range : public Property_Float_Base
{
public:
  Property_Range(BasePropertyTreeItem* parent)
    : Property_Float_Base(parent)
  {
  }

  IMPL_GENERIC_PROPERTY_FUNCTIONS(RANGE)

protected:
  void _CustomizeEditor(QDoubleSpinBox* editor) override
  {
    editor->setSuffix(" m");
  }
};

/*******************************************************************************************/
static UIPropertyFactory::Registrar g_registrar_1(CEntityProperty::PropertyType::EPT_FLOAT,
  [](BasePropertyTreeItem* parent)
  {
    return new Property_Float(parent);
  });
static UIPropertyFactory::Registrar g_registrar_2(CEntityProperty::PropertyType::EPT_ANGLE,
  [](BasePropertyTreeItem* parent)
  {
    return new Property_Angle(parent);
  });

static UIPropertyFactory::Registrar g_registrar_3(CEntityProperty::PropertyType::EPT_RANGE,
  [](BasePropertyTreeItem* parent)
  {
    return new Property_Range(parent);
  });

