// manager.cpp: 实现文件
//

#include "pch.h"
#include "Management.h"
#include "afxdialogex.h"
#include "managerdlg.h"
#include "InputDlg.h"
#include <afxdb.h> // 添加此行以包含CDatabase定义
#include "ManagementDlg.h"


// manager 对话框

IMPLEMENT_DYNAMIC(manager, CDialogEx)

manager::manager(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MANAGER_DLG, pParent)
{

}

manager::~manager()
{
}

void manager::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, LIST, m_list);
    DDX_Control(pDX, IDC_semester, m_semesterEdit);
}


BEGIN_MESSAGE_MAP(manager, CDialogEx)
	ON_BN_CLICKED(student_show, &manager::OnBnClickedstudent_show)
	ON_BN_CLICKED(teacher_show, &manager::OnBnClickedteacher_show)
    ON_BN_CLICKED(course_show, &manager::OnBnClickedcourse_show)
    ON_BN_CLICKED(counselor_show, &manager::OnBnClickedcounselor_show)

    ON_NOTIFY(NM_RCLICK, LIST, &manager::OnListRClick)
    ON_NOTIFY(NM_DBLCLK, LIST, &manager::OnDblclkList)

    ON_EN_KILLFOCUS(101, &manager::OnEditKillFocus)

    ON_EN_CHANGE(IDC_semester, &manager::OnEnChangeSemesterEdit)

    ON_WM_KEYDOWN()
    ON_BN_CLICKED(IDC_change_id, &manager::OnBnClickedchangeid)
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

    // 初始化学期编辑框显示当前学期
    CString semCs(semester_now.c_str());
    if (m_semesterEdit.GetSafeHwnd()) {
        m_semesterEdit.SetWindowText(semCs);
    }

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
    // 判断List Control是否有数据
    if (m_currentTable == _T("无"))
    {
        *pResult = 0;
        return;
    }

    CMenu menu;
    menu.CreatePopupMenu();
    menu.AppendMenu(MF_STRING, 1001, _T("新增"));
    menu.AppendMenu(MF_STRING, 1002, _T("删除"));

    CPoint pt;
    GetCursorPos(&pt);
    int nCmd = menu.TrackPopupMenu(TPM_RETURNCMD | TPM_LEFTALIGN, pt.x, pt.y, this);

    switch (nCmd)
    {
    case 1001:
        OnMenuAdd();
        break;
    case 1002:
        OnMenuDelete();
        break;
    default:
        break;
    }

    *pResult = 0;
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

void manager::OnMenuAdd()
{
    // TODO: 弹出新增对话框，插入数据库并刷新列表
    CString hint;
    int colCount = m_list.GetHeaderCtrl()->GetItemCount();
    for (int i = 0; i < colCount; ++i)
    {
        CString colName = GetListCtrlColumnName(m_list, i);
        hint += colName;
        if (i != colCount - 1) hint += _T(" ");
    }

    InputDlg dlg(this);
    dlg.SetHint(_T("请输入以下字段（用空格分隔）：") + hint);

    if (dlg.DoModal() == IDOK)
    {
        CString input = dlg.m_strInput; // 用户输入的内容
        if (input.IsEmpty())
            return;

        // 按空格分割
        CStringArray arr;
        int cur = 0;
        while (true)
        {
            CString token = input.Tokenize(_T(" "), cur);
            if (token.IsEmpty() && cur == -1) break;
            if (!token.IsEmpty()) arr.Add(token);
        }

        int colCount = m_list.GetHeaderCtrl()->GetItemCount();
        if (arr.GetSize() < colCount)
        {
            AfxMessageBox(_T("输入项不足！"));
            return;
        }

        // 插入到List Control
        int nItem = m_list.InsertItem(m_list.GetItemCount(), arr[0]);
        for (int i = 1; i < colCount; ++i)
        {
            m_list.SetItemText(nItem, i, arr[i]);
        }

        // 插入数据库
        CStringArray fields, values;
        for (int i = 0; i < colCount; ++i)
        {
			CString cloumnname = GetListCtrlColumnName(m_list, i);
            fields.Add(cloumnname);
            values.Add(arr[i]);
        }
        if (m_dbHelper.InsertRecord(m_currentTable, fields, values))
        {
            AfxMessageBox(_T("新增成功！"));
        }
        else
        {
            AfxMessageBox(_T("数据库插入失败！"));
            m_list.DeleteItem(nItem);
        }
    }
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

void manager::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    *pResult = 0;
    LVHITTESTINFO info;
    info.pt = pNMItemActivate->ptAction;

    if (m_list.SubItemHitTest(&info) != -1)
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
        m_list.GetSubItemRect(info.iItem, info.iSubItem, LVIR_LABEL, rect);

        // 将ListCtrl的坐标转换为对话框坐标
        CPoint pt(rect.TopLeft());
        m_list.ClientToScreen(&pt);
        ScreenToClient(&pt);
        rect.MoveToXY(pt);

        // 设置编辑框内容和位置
        editItem.SetWindowText(m_list.GetItemText(info.iItem, info.iSubItem));
        editItem.MoveWindow(&rect, TRUE);
        editItem.ShowWindow(SW_SHOW);
        editItem.SetFocus();
        editItem.SetSel(0, -1); // 全选文本，方便直接编辑
    }
}

void manager::OnEditKillFocus()
{
    CString newText;
    editItem.GetWindowText(newText);

    // 获取原值
    CString oldText = m_list.GetItemText(hitRow, hitCol);

    if (newText != oldText)
    {
        // 获取主键信息
        CString keyValue = m_list.GetItemText(hitRow, 0); // 假设主键在第0列
        CString keyName = GetListCtrlColumnName(m_list, 0);
        CString colName = GetListCtrlColumnName(m_list, hitCol);

        CStringArray fields, values;
        fields.Add(colName);
        values.Add(newText);

        CString where;
        where.Format(_T("%s='%s'"), keyName, keyValue);

        if (m_dbHelper.UpdateRecord(m_currentTable, where, fields, values))
        {
            m_list.SetItemText(hitRow, hitCol, newText);
            AfxMessageBox(_T("修改成功！"));
        }
        else
        {
            AfxMessageBox(_T("修改失败！"));
        }
    }

    editItem.ShowWindow(SW_HIDE);
}

void manager::OnEditKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (nChar == VK_RETURN)
    {
        OnEditKillFocus();
    }
    else
    {
        CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
    }
}

BOOL manager::PreTranslateMessage(MSG* pMsg)
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

        // 学期编辑框：按回车显式保存
        if (m_semesterEdit.m_hWnd != NULL && GetFocus() == &m_semesterEdit)
        {
            SaveSemesterEdit();
            return TRUE; // 拦截
        }

        // 其它控件时，依然拦截回车，防止关闭对话框
        return TRUE;
    }
    return CDialogEx::PreTranslateMessage(pMsg);
}

void manager::OnEnChangeSemesterEdit()
{
    if (!m_semesterEdit.GetSafeHwnd()) return;

    CString sem;
    m_semesterEdit.GetWindowText(sem);
    sem.Trim(); // 去掉首尾空格，规范化输入

    int year = 0, term = 0;
    if (_stscanf_s(sem, _T("%d-%d"), &year, &term) == 2)
    {
        if ((term == 1 || term == 2) && year >= 2000 && year <= 2100)
        {
            // 规范化为 "YYYY-T" 格式，避免因为空格或前导零等差异导致判断失败
            CString norm;
            norm.Format(_T("%04d-%d"), year, term);

            CStringA semA(norm);
            std::string newSem = semA.GetString();

            // 仅在确实变化时写回全局变量
            if (newSem != semester_now)
            {
                semester_now = newSem;
                // 实时更新时不弹出大量窗口 —— 仅静默更新
                // 如果你希望实时也提示，请打开下面一行：
                // AfxMessageBox(CString(_T("学期已更新为: ")) + norm);
            }
        }
        // 非法格式不更新全局变量
    }
}

void manager::SaveSemesterEdit()
{
    if (!m_semesterEdit.GetSafeHwnd())
        return;

    CString sem;
    m_semesterEdit.GetWindowText(sem);
    sem.Trim(); // 去掉首尾空格

    int year = 0, term = 0;
    if (_stscanf_s(sem, _T("%d-%d"), &year, &term) == 2
        && (term == 1 || term == 2)
        && year >= 2000 && year <= 2100)
    {
        // 规范化字符串再写回，保证一致性
        CString norm;
        norm.Format(_T("%04d-%d"), year, term);
        CStringA semA(norm);
        std::string newSem = semA.GetString();

        if (newSem != semester_now)
        {
            std::string oldSem = semester_now;
            semester_now = newSem;

            CString msg;
            msg.Format(_T("学期已从 %S 更新为 %s"), oldSem.c_str(), norm);
            AfxMessageBox(msg);
        }
        else
        {
            // 值相同，静默返回（可改为提示）
            // AfxMessageBox(_T("学期未发生变化"));
        }

        // 把编辑框显示为规范化后的字符串（消除用户输入差异）
        m_semesterEdit.SetWindowText(norm);
    }
    else
    {
        // 格式错误：提示并回滚显示为当前全局学期，恢复焦点和全选
        AfxMessageBox(_T("学期格式错误，应为 YYYY-1 或 YYYY-2（例如 2024-2）"));
        CString semCs(semester_now.c_str());
        m_semesterEdit.SetWindowText(semCs);
        m_semesterEdit.SetFocus();
        m_semesterEdit.SetSel(0, -1);
    }
}
void manager::OnBnClickedchangeid()
{
    // TODO: 在此添加控件通知处理程序代码
    CManagementDlg dlg;
    EndDialog(IDOK);
	dlg.DoModal();

}
