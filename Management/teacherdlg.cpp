// teacherdlg.cpp: 实现文件
//

#include "pch.h"
#include "Management.h"
#include "afxdialogex.h"
#include "teacherdlg.h"
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

	// TODO:  在此添加额外的初始化
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
	// TODO: 弹出新增对话框，插入数据库并刷新列表
	CDialogEx dlg(IDD_class_student, this);
	// 以模态方式显示对话框，用户按下确定后刷新列表数据
	if (dlg.DoModal() == IDOK)
	{
		if (!m_dbHelper.DisplayTableData(m_list, _T("teach_class")))
		{
			AfxMessageBox(_T("刷新课程数据失败！"));
		}
	}
}
