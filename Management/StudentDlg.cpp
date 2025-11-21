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
    DDX_Control(pDX, Student_List, m_list);
}


BEGIN_MESSAGE_MAP(StudentDlg, CDialogEx)
	ON_BN_CLICKED(person_show, &StudentDlg::OnClickedPersonShow)
END_MESSAGE_MAP()



// StudentDlg 消息处理程序
void StudentDlg::OnClickedPersonShow()
{
	// TODO: 在此添加控件通知处理程序代码
    CString strWhere;
    strWhere.Format(_T("%s = '%s'"), _T("student_uid"), uid);

    if (!m_dbHelper.DisplayTableData(m_list, _T("student"),_T("*"),strWhere))
    {
        AfxMessageBox(_T("显示学生数据失败！"));
    }
    else
    {
        m_currentTable = _T("student");
    }
}

BOOL StudentDlg::OnInitDialog()
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
