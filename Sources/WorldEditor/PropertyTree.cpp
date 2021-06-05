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
#include "PropertyTree.h"
#include "Tree/treemodel.h"

#include <QTreeView>
#include <QVBoxLayout>

BEGIN_MESSAGE_MAP(PropertyTree_MFC_Host, CDialogBar)
  ON_WM_SIZE()
  ON_MESSAGE(WM_INITDIALOG, OnInitDialog)
END_MESSAGE_MAP()

const char* input_test =
R"(Getting Started				How to familiarize yourself with Qt Designer
    Launching Designer			Running the Qt Designer application
    The User Interface			How to interact with Qt Designer

Designing a Component			Creating a GUI for your application
    Creating a Dialog			How to create a dialog
    Composing the Dialog		Putting widgets into the dialog example
    Creating a Layout			Arranging widgets on a form
    Signal and Slot Connections		Making widget communicate with each other

Using a Component in Your Application	Generating code from forms
    The Direct Approach			Using a form without any adjustments
    The Single Inheritance Approach	Subclassing a form's base class
    The Multiple Inheritance Approach	Subclassing the form itself
    Automatic Connections		Connecting widgets using a naming scheme
        A Dialog Without Auto-Connect	How to connect widgets without a naming scheme
        A Dialog With Auto-Connect	Using automatic connections

Form Editing Mode			How to edit a form in Qt Designer
    Managing Forms			Loading and saving forms
    Editing a Form			Basic editing techniques
    The Property Editor			Changing widget properties
    The Object Inspector		Examining the hierarchy of objects on a form
    Layouts				Objects that arrange widgets on a form
        Applying and Breaking Layouts	Managing widgets in layouts 
        Horizontal and Vertical Layouts	Standard row and column layouts
        The Grid Layout			Arranging widgets in a matrix
    Previewing Forms			Checking that the design works

Using Containers			How to group widgets together
    General Features			Common container features
    Frames				QFrame
    Group Boxes				QGroupBox
    Stacked Widgets			QStackedWidget
    Tab Widgets				QTabWidget
    Toolbox Widgets			QToolBox

Connection Editing Mode			Connecting widgets together with signals and slots
    Connecting Objects			Making connections in Qt Designer
    Editing Connections			Changing existing connections
)";

BOOL PropertyTree_MFC_Host::Create(CWnd* pParentWnd, UINT nIDTemplate, UINT nStyle, UINT nID)
{
  auto result = CDialogBar::Create(pParentWnd, nIDTemplate, nStyle, nID);
  m_Size = m_sizeDefault;
  return result;
}

void PropertyTree_MFC_Host::OnSize(UINT, int cx, int cy)
{
  if (mp_winWidget)
    mp_winWidget->resize(cx - 16, cy - 16);
}

LONG PropertyTree_MFC_Host::OnInitDialog(UINT wParam, LONG lParam)
{
  BOOL bRet = HandleInitDialog(wParam, lParam);

  if (!UpdateData(FALSE))
    TRACE0("Warning: UpdateData failed during dialog init.\n");

  if (!mp_winWidget)
  {
    mp_winWidget = std::make_unique<QWinWidget>(this, nullptr, Qt::WindowFlags{});

    auto* tree_view = new QTreeView(mp_winWidget.get());
    auto* tree_model = new TreeModel(input_test, mp_winWidget.get());
    tree_view->setModel(tree_model);

    auto* vertical_layout = new QVBoxLayout(mp_winWidget.get());
    vertical_layout->addWidget(tree_view);
    vertical_layout->setContentsMargins(0, 0, 0, 0);
    mp_winWidget->move(8, 8);
    mp_winWidget->show();
  }
  return bRet;
}

CSize PropertyTree_MFC_Host::CalcDynamicLayout(int nLength, DWORD dwMode)
{
  // Return default if it is being docked or floated
  if ((dwMode & LM_VERTDOCK) || (dwMode & LM_HORZDOCK))
  {
    if (dwMode & LM_STRETCH) // if not docked stretch to fit
      return CSize((dwMode & LM_HORZ) ? 32767 : m_Size.cx,
        (dwMode & LM_HORZ) ? m_Size.cy : 32767);
    else
      return m_Size;
  }
  if (dwMode & LM_MRUWIDTH)
    return m_Size;
  // In all other cases, accept the dynamic length
  if (dwMode & LM_LENGTHY)
    return CSize(m_Size.cx,
      m_Size.cy = nLength);
  else
    return CSize(m_Size.cx = nLength);
}
