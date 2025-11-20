// class_students.cpp: 实现文件
//

#include "pch.h"
#include "Management.h"
#include "afxdialogex.h"
#include "class_students.h"


// class_students 对话框

IMPLEMENT_DYNAMIC(class_students, CDialogEx)

class_students::class_students(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_class_student, pParent)
{

}

class_students::~class_students()
{
}

void class_students::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, students_list, m_studentsList);
}


BEGIN_MESSAGE_MAP(class_students, CDialogEx)
	ON_NOTIFY(LVN_ITEMCHANGED, students_list, &class_students::OnLvnItemchangedlist)
	ON_NOTIFY(NM_DBLCLK, students_list, &class_students::OnDblclkList)
	ON_EN_KILLFOCUS(101, &class_students::OnEditKillFocus)
END_MESSAGE_MAP()


// 辅助：获取指定列的列名（从头部）
static CString GetListCtrlColumnName(CListCtrl& listCtrl, int colIndex)
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


// class_students 消息处理程序

BOOL class_students::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置列表样式
	m_studentsList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);

	// 如果有数据库和 course_uid，则显示对应选课记录
	if (m_pDb != nullptr && !m_courseUid.IsEmpty())
	{
		CString where;
		// 安全地按字符串匹配 course_uid
		where.Format(_T("course_uid = '%s'"), m_courseUid);
		// 使用表名与字段名称与数据库一致（大小写对 MySQL 不敏感）
		if (!m_pDb->DisplayTableData(m_studentsList, _T("courseSelection"), _T("*"), where))
		{
			AfxMessageBox(_T("显示选课学生数据失败！"));
		}
	}

	return TRUE;
}

void class_students::OnLvnItemchangedlist(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}

void class_students::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;
	LVHITTESTINFO info;
	info.pt = pNMItemActivate->ptAction;

	if (m_studentsList.SubItemHitTest(&info) != -1)
	{
		hitRow = info.iItem;
		hitCol = info.iSubItem;
		if (editItem.m_hWnd == NULL) {
			CRect dummyRect(0, 0, 0, 0);
			editItem.Create(WS_CHILD | ES_LEFT | WS_BORDER | ES_AUTOHSCROLL, dummyRect, this, 101);
			editItem.SetFont(this->GetFont(), FALSE);
		}
		// 获取单元格的精确矩形
		CRect rect;
		m_studentsList.GetSubItemRect(info.iItem, info.iSubItem, LVIR_LABEL, rect);

		// 将ListCtrl的坐标转换为对话框坐标
		CPoint pt(rect.TopLeft());
		m_studentsList.ClientToScreen(&pt);
		ScreenToClient(&pt);
		rect.MoveToXY(pt);

		// 设置编辑框内容和位置
		editItem.SetWindowText(m_studentsList.GetItemText(info.iItem, info.iSubItem));
		editItem.MoveWindow(&rect, TRUE);
		editItem.ShowWindow(SW_SHOW);
		editItem.SetFocus();
		editItem.SetSel(0, -1); // 全选文本，方便直接编辑
	}
}

void class_students::OnEditKillFocus()
{
	CString newText;
	editItem.GetWindowText(newText);

	// 获取原值
	if (hitRow < 0 || hitCol < 0) {
		editItem.ShowWindow(SW_HIDE);
		return;
	}

	CString oldText = m_studentsList.GetItemText(hitRow, hitCol);

	if (newText != oldText)
	{
		// 假设主键在第0列（与 manager 实现一致）
		CString keyValue = m_studentsList.GetItemText(hitRow, 0);
		CString keyName = GetListCtrlColumnName(m_studentsList, 0);
		CString colName = GetListCtrlColumnName(m_studentsList, hitCol);

		CStringArray fields, values;
		fields.Add(colName);
		values.Add(newText);

		CString where;
		where.Format(_T("%s='%s'"), keyName, keyValue);

		// 固定表名 courseSelection（本对话框显示的表）
		CString tableName = _T("courseSelection");

		if (m_pDb != nullptr && m_pDb->UpdateRecord(tableName, where, fields, values))
		{
			m_studentsList.SetItemText(hitRow, hitCol, newText);
			AfxMessageBox(_T("修改成功！"));
		}
		else
		{
			AfxMessageBox(_T("修改失败！"));
		}
	}

	editItem.ShowWindow(SW_HIDE);
}

void class_students::OnEditKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_RETURN)
	{
		OnEditKillFocus();
	}
	else
	{
		// 未处理的键交给基类（若需要）
		CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
	}
}

BOOL class_students::PreTranslateMessage(MSG* pMsg)
{
	// 判断消息是否为回车，并且焦点在editItem上
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		if (editItem.m_hWnd != NULL && GetFocus() == &editItem)
		{
			// 让editItem处理回车
			OnEditKeyDown(VK_RETURN, 1, 0);
			return TRUE; // 阻止对话框默认处理
		}
		// 其它控件时，依然拦截回车，防止关闭对话框
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}
