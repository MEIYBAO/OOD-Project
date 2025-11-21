// teacherdlg.cpp: 实现文件
//

#include "pch.h"
#include "Management.h"
#include "afxdialogex.h"
#include "teacherdlg.h"
#include "class_students.h" // 添加
#include <afxdb.h>


// teacherdlg 对话框

IMPLEMENT_DYNAMIC(teacherdlg, CDialogEx)

teacherdlg::teacherdlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_teacher_DLG, pParent)
{

}

teacherdlg::~teacherdlg()
{
}

void teacherdlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX,workpanel , m_list);
}


BEGIN_MESSAGE_MAP(teacherdlg, CDialogEx)
	ON_BN_CLICKED(showClass, &teacherdlg::OnBnClickedshowclass)
	ON_BN_CLICKED(ChooseCourse, &teacherdlg::OnBnClickedChoosecourse)

	ON_NOTIFY(LVN_ITEMCHANGED, workpanel, &teacherdlg::OnLvnItemchangedworkpanel)
	ON_NOTIFY(NM_RCLICK, workpanel, &teacherdlg::OnListRClick)
END_MESSAGE_MAP()


// teacherdlg 消息处理程序

void teacherdlg::OnBnClickedshowclass()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!m_dbHelper.DisplayTableData(m_list, _T("teach_class")))
	{
		AfxMessageBox(_T("显示课程数据失败！"));
	}

}

BOOL teacherdlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 初始化数据库连接
	if (!m_dbHelper.Connect())
	{
		AfxMessageBox(_T("数据库连接失败！"));
		return FALSE;
	}

	// 设置List Control样式
	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void teacherdlg::OnLvnItemchangedworkpanel(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}

void teacherdlg::OnListRClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING, 1001, _T("显示学生列表"));

	CPoint pt;
	GetCursorPos(&pt);
	int nCmd = menu.TrackPopupMenu(TPM_RETURNCMD | TPM_LEFTALIGN, pt.x, pt.y, this);

	switch (nCmd)
	{
	case 1001:
		OnShowClassStudents();
		break;
	default:
		break;
	}

	*pResult = 0;
}

void teacherdlg::OnShowClassStudents()
{
	// 获取选中项
	POSITION pos = m_list.GetFirstSelectedItemPosition();
	if (pos == nullptr)
	{
		AfxMessageBox(_T("请先选择一个教学班记录！"));
		return;
	}
	int nItem = m_list.GetNextSelectedItem(pos);

	// 查找列名为 course_uid 的列索引
	int nColumnCount = 0;
	CHeaderCtrl* pHeader = m_list.GetHeaderCtrl();
	if (pHeader)
		nColumnCount = pHeader->GetItemCount();

	int colIndex = -1;
	TCHAR szText[256] = { 0 };
	for (int i = 0; i < nColumnCount; ++i)
	{
		HDITEM hdi = { 0 };
		hdi.mask = HDI_TEXT;
		hdi.pszText = szText;
		hdi.cchTextMax = _countof(szText);
		if (pHeader->GetItem(i, &hdi))
		{
			CString colName = szText;
			if (colName.CompareNoCase(_T("course_uid")) == 0 || colName.CompareNoCase(_T("course uid")) == 0)
			{
				colIndex = i;
				break;
			}
		}
	}

	// 如果未找到 course_uid 列，则尝试使用第二列作为回退（常见布局）
	if (colIndex == -1)
	{
		if (nColumnCount > 1) colIndex = 1;
		else colIndex = 0;
	}

	CString courseUid = m_list.GetItemText(nItem, colIndex);
	if (courseUid.IsEmpty())
	{
		AfxMessageBox(_T("无法获取所选记录的 course_uid！"));
		return;
	}

	// 打开 class_students 对话框并传入数据库指针与 course_uid
	class_students dlg(this);
	dlg.SetDatabaseHelper(&m_dbHelper);
	dlg.SetCourseUid(courseUid);
	dlg.DoModal();

	// 不需要刷新 teach_class 列表，除非在子对话框做了修改
}


void teacherdlg::OnBnClickedChoosecourse()
{
	// 显示 course 表的数据到列表控件
	if (!m_dbHelper.DisplayTableData(m_list, _T("course")))
	{
		AfxMessageBox(_T("显示课程数据失败！"));
	}
}
