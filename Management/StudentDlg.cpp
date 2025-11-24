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
    ON_NOTIFY(NM_DBLCLK, Student_List, &StudentDlg::OnDblclkList)
    ON_BN_CLICKED(choose_class, &StudentDlg::OnBnClickedChooseClass)
    ON_BN_CLICKED(courses, &StudentDlg::OnBnClickedcourses)
    ON_NOTIFY(NM_RCLICK, Student_List, &StudentDlg::OnListRClick)
    ON_BN_CLICKED(IDC_STU_CHANGE, &StudentDlg::OnBnClickedStuChange)
    ON_BN_CLICKED(IDC_SEARCH_GRADE, &StudentDlg::OnBnClickedSearchGrade)
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

void StudentDlg::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult)
{
    // 仅当当前视图为 student（即通过 person_show 按钮加载）时允许左键双击编辑
    if (m_currentTable.CompareNoCase(_T("student")) != 0)
    {
        *pResult = 0;
        return;
    }

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

BOOL StudentDlg::PreTranslateMessage(MSG* pMsg)
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

void StudentDlg::OnEditKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
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

void StudentDlg::OnEditKillFocus()
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
void StudentDlg::OnBnClickedChooseClass()
{
    // TODO: 在此添加控件通知处理程序代码
    m_allowUnselectInCourse = false;
    // 从全局 std::string 转为 CString 并转义
    CString semCs = CString(semester_now.c_str());
    CString semEsc = EscapeSql(semCs);

    int year = 0;
    int term = 0;

    // 解析 "2024-1" 这种格式
    _stscanf_s(semEsc, _T("%d-%d"), &year, &term);

    CString semDateStart;
    CString semDateEnd;
    if (term == 1)
    {
        // 1 学期 -> 3 月 1 日
        semDateStart.Format(_T("%d-3-1"), year);
        semDateEnd.Format(_T("%d-9-1"), year);
    }
    else if (term == 2)
    {
        // 2 学期 -> 9 月 1 日
        semDateStart.Format(_T("%d-9-1"), year);
        ++year;
        semDateEnd.Format(_T("%d-3-1"), year);
    }

    CString strWhere;
    strWhere.Format(_T("course_uid NOT IN (SELECT course_uid FROM courseselection WHERE student_uid = '%s' AND selection_date >=  '%s' AND selection_date < '%s')"), uid,semDateStart,semDateEnd);

    // 如果你的数据库不支持子查询，也可以改为先查询 teacher_course 再用 NOT IN 列表构造 WHERE
    if (!m_dbHelper.DisplayTableData(m_list, _T("course"), _T("*"), strWhere))
    {
        AfxMessageBox(_T("显示未被选择的课程失败！"));
    }
    else
    {
        // 标记当前 ListCtrl 正在显示 course
        m_currentTable = _T("course");
    }
}

void StudentDlg::OnBnClickedcourses()
{
    // TODO: 在此添加控件通知处理程序代码

    m_allowUnselectInCourse = true;

        // 从全局 std::string 转为 CString 并转义
    CString semCs = CString(semester_now.c_str());
    CString semEsc = EscapeSql(semCs);

    int year = 0;
    int term = 0;

    // 解析 "2024-1" 这种格式
    _stscanf_s(semEsc, _T("%d-%d"), &year, &term);

    CString semDateStart;
    CString semDateEnd;
    if (term == 1)
    {
        // 1 学期 -> 3 月 1 日
        semDateStart.Format(_T("%d-3-1"), year);
        semDateEnd.Format(_T("%d-9-1"), year);
    }
    else if (term == 2)
    {
        // 2 学期 -> 9 月 1 日
        semDateStart.Format(_T("%d-9-1"), year);
        ++year;
        semDateEnd.Format(_T("%d-3-1"), year);
    }

    CString strWhere;
    strWhere.Format(_T("course_uid IN (SELECT course_uid FROM courseselection WHERE student_uid = '%s' AND selection_date >=  '%s' AND selection_date < '%s')"), uid, semDateStart, semDateEnd);

    // 如果你的数据库不支持子查询，也可以改为先查询 teacher_course 再用 NOT IN 列表构造 WHERE
    if (!m_dbHelper.DisplayTableData(m_list, _T("course"), _T("*"), strWhere))
    {
        AfxMessageBox(_T("显示已选择的课程失败！"));
    }
    else
    {
        // 标记当前 ListCtrl 正在显示 course
        m_currentTable = _T("course");
    }
}

// 新增：右键菜单处理（类似 teacherdlg 的实现）
void StudentDlg::OnListRClick(NMHDR* pNMHDR, LRESULT* pResult)
{
    // 只有当当前表是 course 时显示选择/退选菜单
    if (m_currentTable.CompareNoCase(_T("course")) != 0)
    {
        *pResult = 0;
        return;
    }

    // 获取鼠标位置并命中行
    CPoint ptScreen;
    GetCursorPos(&ptScreen);

    CPoint ptClient = ptScreen;
    m_list.ScreenToClient(&ptClient);
    int nItem = m_list.HitTest(ptClient);
    if (nItem == -1)
    {
        *pResult = 0;
        return;
    }

    CMenu menu;
    menu.CreatePopupMenu();

    // 根据视图状态决定是“选择”还是“退选”
    if (m_allowUnselectInCourse)
        menu.AppendMenu(MF_STRING, 4002, _T("退选"));
    else
        menu.AppendMenu(MF_STRING, 4001, _T("选择"));

    int nCmd = menu.TrackPopupMenu(TPM_RETURNCMD | TPM_LEFTALIGN, ptScreen.x, ptScreen.y, this);

    switch (nCmd)
    {
    case 4001: // 选择 -> 插入 courseselection
    {
        // 可以传入学期信息或空（函数内部会使用当前日期）
        CString semDate; // 传空使用当前日期
        bool ok = InsertCourseSelectionAt(m_list, m_dbHelper, nItem, uid, semDate);
        if (ok)
        {
            // 插入成功后刷新当前视图（移除已被选择的课程）
            OnBnClickedChooseClass();
        }
    }
    break;
    case 4002: // 退选 -> 删除 courseselection
    {
        CString semDate; // 可传空或具体学期日期
        bool ok = DeleteCourseSelectionAt(m_list, m_dbHelper, nItem, uid, semDate);
        if (ok)
        {
            // 退选成功后刷新当前课程视图
            OnBnClickedcourses();
        }
    }
    break;
    default:
        break;
    }

    *pResult = 0;
}

// 新增：在指定行插入一条 courseselection 记录（student_uid, course_uid, selection_date）
// 返回 true 表示插入成功
bool StudentDlg::InsertCourseSelectionAt(CListCtrl& listCtrl, CDatabaseHelper& dbHelper, int nItem, const CString& studentUidParam, const CString& selectionDateParam)
{
    if (nItem < 0 || !listCtrl.GetSafeHwnd() || !dbHelper.IsConnected())
        return false;

    // 查找 course_uid 列索引（尝试匹配列名）
    int nColumnCount = 0;
    CHeaderCtrl* pHeader = listCtrl.GetHeaderCtrl();
    if (pHeader) nColumnCount = pHeader->GetItemCount();

    int colCourse = -1;
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
                colCourse = i;
                break;
            }
        }
    }

    // 回退策略：若未找到 course 列，则使用第0列
    if (colCourse == -1 && nColumnCount > 0) colCourse = 0;
    if (colCourse == -1) return false;

    CString courseUid = listCtrl.GetItemText(nItem, colCourse);
    CString studentUid = studentUidParam;
    if (studentUid.IsEmpty())
    {
        // 假设全局变量 uid 可用（你的代码里以前直接使用过 uid）
        studentUid = uid;
    }

    if (studentUid.IsEmpty() || courseUid.IsEmpty())
    {
        AfxMessageBox(_T("无法获取 student_uid 或 course_uid，操作取消。"));
        return false;
    }

    // 默认使用传入的 selectionDate，若为空则使用当前日期
    CString selDate = selectionDateParam;
    if (selDate.IsEmpty())
    {
        CTime now = CTime::GetCurrentTime();
        selDate = now.Format(_T("%Y-%m-%d"));
    }

    // 确认插入
    if (AfxMessageBox(_T("确定要为该学生选择此课程吗？"), MB_YESNO | MB_ICONQUESTION) != IDYES)
        return false;

    // 构造插入字段与值
    CStringArray fields, values;
    fields.Add(_T("student_uid"));
    fields.Add(_T("course_uid"));
    fields.Add(_T("selection_date"));
    fields.Add(_T("FirstRepair"));

    values.Add(studentUid);
    values.Add(courseUid);
    values.Add(selDate);
	values.Add(_T("1")); // FristRepair 初始为 1

    int index_last = values.GetSize() - 1;

    // 调用 InsertRecord（内部会做转义）
    if (dbHelper.InsertRecord(_T("courseselection"), fields, values))
    {
        AfxMessageBox(_T("选课成功，已加入 courseselection 表。"));
        return true;
    }
    values[index_last] = "0";
    if (dbHelper.InsertRecord(_T("courseselection"), fields, values))
    {
        AfxMessageBox(_T("选课成功，已加入 courseselection 表。"));
        return true;
    }
    else
    {
        AfxMessageBox(_T("选课失败，请检查数据库或日志。"));
        return false;
    }
}

// 新增：在 courseselection 删除匹配记录（按 student_uid + course_uid [+ 可选日期范围））
bool StudentDlg::DeleteCourseSelectionAt(CListCtrl& listCtrl, CDatabaseHelper& dbHelper, int nItem, const CString& studentUidParam, const CString& selectionDateParam)
{
    if (nItem < 0 || !listCtrl.GetSafeHwnd() || !dbHelper.IsConnected())
        return false;

    // 查找 course_uid 列索引
    int nColumnCount = 0;
    CHeaderCtrl* pHeader = listCtrl.GetHeaderCtrl();
    if (pHeader) nColumnCount = pHeader->GetItemCount();

    int colCourse = -1;
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
                colCourse = i;
                break;
            }
        }
    }

    // 回退策略
    if (colCourse == -1 && nColumnCount > 0) colCourse = 0;
    if (colCourse == -1) return false;

    CString courseUid = listCtrl.GetItemText(nItem, colCourse);
    CString studentUid = studentUidParam;
    if (studentUid.IsEmpty())
    {
        studentUid = uid;
    }

    if (studentUid.IsEmpty() || courseUid.IsEmpty())
    {
        AfxMessageBox(_T("无法获取 student_uid 或 course_uid，操作取消。"));
        return false;
    }

    // 构造 WHERE 子句（按 student_uid + course_uid）
    CString where;
    where.Format(_T("student_uid = '%s' AND course_uid = '%s'"), EscapeSql(studentUid), EscapeSql(courseUid));

    // 如果传入了 selectionDateParam，可进一步限定（例如删除指定学期/日期）
    if (!selectionDateParam.IsEmpty())
    {
        where += _T(" AND selection_date = '") + EscapeSql(selectionDateParam) + _T("'");
    }

    // 确认删除
    if (AfxMessageBox(_T("确定要退选该课程吗？"), MB_YESNO | MB_ICONQUESTION) != IDYES)
        return false;

    if (dbHelper.DeleteRecord(_T("courseselection"), where))
    {
        AfxMessageBox(_T("退选成功。"));
        return true;
    }
    else
    {
        AfxMessageBox(_T("退选失败，请检查数据库或日志。"));
        return false;
    }
}

void StudentDlg::OnBnClickedStuChange()
{
    // TODO: 在此添加控件通知处理程序代码
    CManagementDlg dlg;
    EndDialog(IDOK);
    dlg.DoModal();
}

void StudentDlg::OnBnClickedSearchGrade()
{
    // TODO: 在此添加控件通知处理程序代码
    m_allowUnselectInCourse = true;

    // 从全局 std::string 转为 CString 并转义
    CString semCs = CString(semester_now.c_str());
    CString semEsc = EscapeSql(semCs);

    int year = 0;
    int term = 0;

    // 解析 "2024-1" 这种格式
    _stscanf_s(semEsc, _T("%d-%d"), &year, &term);

    CString semDateStart;
    if (term == 1)
    {
        // 1 学期 -> 3 月 1 日
        semDateStart.Format(_T("%d-3-1"), year);
    }
    else if (term == 2)
    {
        // 2 学期 -> 9 月 1 日
        semDateStart.Format(_T("%d-9-1"), year);
    }

	CString strTables = _T("course JOIN courseselection ON course.course_uid=courseselection.course_uid");
	CString  strColumns = _T("course.course_uid,course_name,grade,credits,category,FirstRepair");

    CString strWhere;
    strWhere.Format(_T("student_uid = '%s' AND selection_date < '%s'"), uid, semDateStart);

    // 如果你的数据库不支持子查询，也可以改为先查询 teacher_course 再用 NOT IN 列表构造 WHERE
    if (!m_dbHelper.DisplayTableData(m_list, strTables,strColumns, strWhere))
    {
        AfxMessageBox(_T("显示课程成绩失败！"));
    }
    else
    {
        // 标记当前 ListCtrl 正在显示 course
        m_currentTable = _T("course,courseselection");
    }
}
