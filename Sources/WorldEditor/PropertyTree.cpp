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

BEGIN_MESSAGE_MAP(PropertyTree_MFC_Host, CDialogBar)
  ON_WM_SIZE()
END_MESSAGE_MAP()

BOOL PropertyTree_MFC_Host::Create(CWnd* pParentWnd, UINT nIDTemplate, UINT nStyle, UINT nID)
{
  auto result = CDialogBar::Create(pParentWnd, nIDTemplate, nStyle, nID);
  m_sizeFloating = m_sizeDocked = m_sizeDefault;
  return result;
}

void PropertyTree_MFC_Host::OnSize(UINT nType, int cx, int cy)
{
}

CSize PropertyTree_MFC_Host::CalcDynamicLayout(int nLength, DWORD dwMode)
{
  // Return default if it is being docked or floated
  if ((dwMode & LM_VERTDOCK) || (dwMode & LM_HORZDOCK))
  {
    if (dwMode & LM_STRETCH) // if not docked stretch to fit
      return CSize((dwMode & LM_HORZ) ? 32767 : m_sizeDocked.cx,
        (dwMode & LM_HORZ) ? m_sizeDocked.cy : 32767);
    else
      return m_sizeDocked;
  }
  if (dwMode & LM_MRUWIDTH)
    return m_sizeFloating;
  // In all other cases, accept the dynamic length
  if (dwMode & LM_LENGTHY)
    return CSize(m_sizeFloating.cx,
      m_sizeFloating.cy = m_sizeDocked.cy = nLength);
  else
    return CSize( m_sizeFloating.cx = m_sizeDocked.cx = nLength);
}
