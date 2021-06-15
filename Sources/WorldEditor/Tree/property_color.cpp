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
#include "color_widget.h"
#include "WorldEditor.h"

#include <QWinWidget>
#include <QColorDialog>
#include <QPainter>

namespace
{
  QColor _FromCTCol(COLOR col)
  {
    return QColor::fromRgba((col >> 8) | (col << 24));
  }

  COLOR _FromQtCol(QColor col)
  {
    return (col.rgba() << 8) | (col.rgba() >> 24);
  }
}

class Property_Color : public BaseEntityPropertyTreeItem
{
public:
  Property_Color(BasePropertyTreeItem* parent)
    : BaseEntityPropertyTreeItem(parent)
  {
  }

  QWidget* CreateEditor(QWidget* parent) override
  {
    auto* editor = new ColorWidget(_FromCTCol(_CurrentPropValue()), parent);

    QObject::connect(editor, &ColorWidget::editingStarted, this, [this]
      {
        if (!mp_signal_blocker)
          mp_signal_blocker = std::make_unique<QSignalBlocker>(this);
      });

    QObject::connect(editor, &ColorWidget::editingFinished, this, [this, editor]
      {
        mp_signal_blocker.reset();
        _WriteProperty(_FromQtCol(editor->GetColor()));
      });

    QObject::connect(editor, &ColorWidget::colorChanged, this, [this, editor]
      {
        _WriteProperty(_FromQtCol(editor->GetColor()));
        editor->SetColor(_FromCTCol(_CurrentPropValue()));
      });

    QObject::connect(editor, &ColorWidget::clicked, this, [this]
      {
        CWorldEditorApp::ModalGuard guard;
        QWinWidget modal_widget(theApp.m_pMainWnd->GetSafeHwnd(), nullptr, Qt::WindowFlags{});
        QColorDialog color_dialog(&modal_widget);
        color_dialog.setOptions(QColorDialog::ShowAlphaChannel);
        color_dialog.setWindowTitle("Choose color");
        color_dialog.setCurrentColor(_FromCTCol(_CurrentPropValue()));
        color_dialog.exec();
        if (color_dialog.result() == QDialog::Accepted)
          _WriteProperty(_FromQtCol(color_dialog.selectedColor()));
      });

    return editor;
  }

  IMPL_GENERIC_PROPERTY_FUNCTIONS(COLOR)

private:
  std::unique_ptr<QSignalBlocker> mp_signal_blocker;
};


/*******************************************************************************************/
static UIPropertyFactory::Registrar g_registrar(CEntityProperty::PropertyType::EPT_COLOR,
  [](BasePropertyTreeItem* parent)
  {
    return new Property_Color(parent);
  });
