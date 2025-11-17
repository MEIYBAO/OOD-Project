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
	: CDialogEx(IDD_MANAGER_DIALOG, pParent)
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

    ON_NOTIFY(NM_RCLICK, LIST, &manager::OnListRClick)
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
    else 
    {
        m_currentTable = _T("student");
    }
}

void manager::OnBnClickedteacher_show()
{
    if (!m_dbHelper.DisplayTableData(m_list, _T("teacher")))
    {
        AfxMessageBox(_T("显示教师数据失败！"));
    }
    else
    {
        m_currentTable = _T("teacher");
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
    else
    {
        m_currentTable = _T("course");
    }
}

void manager::OnBnClickedcounselor_show()
{
    // TODO: 在此添加控件通知处理程序代码
    if (!m_dbHelper.DisplayTableData(m_list, _T("counselor")))
    {
        AfxMessageBox(_T("显示辅导员数据失败！"));
    }
    else
    {
        m_currentTable = _T("counselor");
    }
}

void manager::OnListRClick(NMHDR* pNMHDR, LRESULT* pResult)
{
    CMenu menu;
    menu.CreatePopupMenu();
    menu.AppendMenu(MF_STRING, 1001, _T("新增"));
    menu.AppendMenu(MF_STRING, 1002, _T("修改"));
    menu.AppendMenu(MF_STRING, 1003, _T("删除"));

    CPoint pt;
    GetCursorPos(&pt);
    int nCmd = menu.TrackPopupMenu(TPM_RETURNCMD | TPM_LEFTALIGN, pt.x, pt.y, this);

    switch (nCmd)
    {
    case 1001:
        OnMenuAdd();
        break;
    case 1002:
        OnMenuEdit();
        break;
    case 1003:
        OnMenuDelete();
        break;
    default:
        break;
    }

    *pResult = 0;
}

void manager::OnMenuAdd()
{
    // TODO: 弹出新增对话框，插入数据库并刷新列表
    AfxMessageBox(_T("新增操作"));
}

void manager::OnMenuEdit()
{
    // TODO: 获取选中项，弹出编辑对话框，更新数据库并刷新列表
    AfxMessageBox(_T("修改操作"));
}

// 获取第colIndex列的列名
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

void manager::OnMenuDelete()
{
    // TODO: 获取选中项，删除数据库记录并刷新列表

    // 获取选中项
    POSITION pos = m_list.GetFirstSelectedItemPosition();
    if (pos == nullptr)
    {
        AfxMessageBox(_T("请先选择要删除的记录！"));
        return;
    }

    int nItem = m_list.GetNextSelectedItem(pos);
    CString strID = m_list.GetItemText(nItem, 0); // 假设主键在第0列
    int colIndex = 0; // 你要获取的列索引
    CString colName = GetListCtrlColumnName(m_list, colIndex);

    // 获取当前表名（可根据你的实际逻辑调整）
    CString tableName = m_currentTable;

    // 确认删除
    if (AfxMessageBox(_T("确定要删除该记录吗？"), MB_YESNO | MB_ICONQUESTION) != IDYES)
        return;

    CString strWhere;
    strWhere.Format(_T("%s = '%s'"), colName, strID);


    // 调用数据库删除
    if (m_dbHelper.DeleteRecord(tableName, strWhere))
    {
        m_list.DeleteItem(nItem); // 从界面移除
        AfxMessageBox(_T("删除成功！"));
    }
    else
    {
        AfxMessageBox(_T("删除失败！"));
    }
}