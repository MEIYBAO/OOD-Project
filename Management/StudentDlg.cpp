// StudentDlg.cpp: 实现文件
//

#include "pch.h"
#include "Management.h"
#include "afxdialogex.h"
#include "StudentDlg.h"


// StudentDlg 对话框

IMPLEMENT_DYNAMIC(StudentDlg, CDialogEx)

StudentDlg::StudentDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_STUDENT_DIALOG, pParent)
{

}

StudentDlg::~StudentDlg()
{
}

void StudentDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(StudentDlg, CDialogEx)
	ON_BN_CLICKED(person_show, &StudentDlg::OnBnClickedperson_show)
END_MESSAGE_MAP()

void StudentDlg::OnBnClickedperson_show()
{
	// TODO: 在此添加控件通知处理程序代码
    if (!m_dbHelper.DisplayTableData(m_list, _T("student")))
    {
        AfxMessageBox(_T("显示学生数据失败！"));
    }
    else
    {
        m_currentTable = _T("student");
    }
}
