#include "pch.h"
#include "tool.h"

CString GetListCtrlColumnName(CListCtrl& listCtrl, int colIndex)
{
    CString strColName;
    CHeaderCtrl* pHeader = listCtrl.GetHeaderCtrl();
    if (pHeader)
    {
        HDITEM hdi = { 0 };
        TCHAR szText[256] = { 0 };
        hdi.mask = HDI_TEXT;
        hdi.pszText = szText;
        hdi.cchTextMax = 255;
        if (pHeader->GetItem(colIndex, &hdi))
        {
            strColName = szText;
        }
    }
    return strColName;
}