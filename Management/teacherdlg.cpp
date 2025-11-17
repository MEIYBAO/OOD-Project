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
