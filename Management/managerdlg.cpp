// manager.cpp: 实现文件
//

#include "pch.h"
#include "Management.h"
#include "afxdialogex.h"
#include "managerdlg.h"
#include <afxdb.h> // 添加此行以包含CDatabase定义


// manager 对话框

IMPLEMENT_DYNAMIC(manager, CDialogEx)

manager::manager(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG2, pParent)
{

}

manager::~manager()
{
}

void manager::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, LIST, m_list);
}


BEGIN_MESSAGE_MAP(manager, CDialogEx)
	ON_BN_CLICKED(student_show, &manager::OnBnClickedstudent_show)
	ON_BN_CLICKED(teacher_show, &manager::OnBnClickedteacher_show)
    ON_BN_CLICKED(course_show, &manager::OnBnClickedcourse_show)
    ON_BN_CLICKED(counselor_show, &manager::OnBnClickedcounselor_show)
	ON_BN_CLICKED(student_counselor_show, &manager::OnBnClickedstudent_counselor_show)
END_MESSAGE_MAP()


// manager 消息处理程序


void manager::OnBnClickedstudent_show()
{
	// TODO: 在此添加控件通知处理程序代码
    // 显示student表的所有数据
    if (!m_dbHelper.DisplayTableData(m_list, _T("student")))
    {
        AfxMessageBox(_T("显示学生数据失败！"));
    }
}

void manager::OnBnClickedteacher_show()
{
    if (!m_dbHelper.DisplayTableData(m_list, _T("teacher")))
    {
        AfxMessageBox(_T("显示教师数据失败！"));
    }
}

BOOL manager::OnInitDialog()
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

void manager::OnBnClickedcourse_show()
{
    // TODO: 在此添加控件通知处理程序代码
    if (!m_dbHelper.DisplayTableData(m_list, _T("course")))
    {
        AfxMessageBox(_T("显示课程数据失败！"));
    }
}

void manager::OnBnClickedcounselor_show()
{
    // TODO: 在此添加控件通知处理程序代码
    if (!m_dbHelper.DisplayTableData(m_list, _T("counselor")))
    {
        AfxMessageBox(_T("显示辅导员数据失败！"));
    }
}

void manager::OnBnClickedstudent_counselor_show()
{
    if (!m_dbHelper.DisplayTableData(m_list, _T("student_counselor")))
    {
        AfxMessageBox(_T("显示辅导员分配数据失败！"));
    }
}
