// teacherdlg.cpp: 实现文件
#include "pch.h"
#include "teacherdlg.h"



// teacherdlg 对话框

IMPLEMENT_DYNAMIC(teacherdlg, CDialogEx)

teacherdlg::teacherdlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_teacher_DLG, pParent)
	, m_currentTable(_T("")) // 初始化当前表为空
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
	ON_BN_CLICKED(teach_classes, &teacherdlg::OnBnClickedclasses)

	ON_NOTIFY(LVN_ITEMCHANGED, workpanel, &teacherdlg::OnLvnItemchangedworkpanel)
	ON_NOTIFY(NM_RCLICK, workpanel, &teacherdlg::OnListRClick)
END_MESSAGE_MAP()


// 辅助：对简单 SQL 字符 ' 做转义，避免语句中断
static CString EscapeSql(const CString& s)
{
	CString out = s;
	out.Replace(_T("'"), _T("''"));
	return out;
}

// teacherdlg 消息处理程序

void teacherdlg::OnBnClickedshowclass()
{
	// 如果未设置教师 UID，则提示并返回（上层应调用 SetTeacherUid）
	if (m_teacherUid.IsEmpty())
	{
		AfxMessageBox(_T("教师 UID 未设置，无法筛选教学班。"));
		return;
	}

	// 构造安全的 WHERE 子句，按 teacher_uid 筛选
	CString where;
	CString uidEsc = EscapeSql(m_teacherUid);
	where.Format(_T("teacher_uid = '%s'"), uidEsc);

	// 传入 where 条件以只显示当前教师的教学班
	if (!m_dbHelper.DisplayTableData(m_list, _T("teach_class"), _T("*"), where))
	{
		AfxMessageBox(_T("显示教学班数据失败！"));
	}
	else
	{
		// 标记当前 ListCtrl 正在显示 teach_class，从而允许右键菜单中的“显示学生列表”操作
		m_currentTable = _T("teach_class");
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
	// 限制：只有当当前列表是 teach_class（由 OnBnClickedshowclass 填充）才允许显示学生列表
	if (m_currentTable.CompareNoCase(_T("teach_class")) != 0)
	{
		AfxMessageBox(_T("请先点击“显示教学班”以载入教学班列表，然后在该列表中选择一行，再使用“显示学生列表”。"));
		return;
	}

	// 获取选中项
	POSITION pos = m_list.GetFirstSelectedItemPosition();
	if (pos == nullptr)
	{
		AfxMessageBox(_T("请先选择一个教学班记录！"));
		return;
	}
	int nItem = m_list.GetNextSelectedItem(pos);

	// 查找列名为 course_uid 的列索引（teach_class 表通常包含 course_uid 列）
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

	// 如果未找到 course_uid 列，则尝试使用常见列索引（例如索引1），但仍需提示以防错误
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
	// 只显示当前学期尚未被任何教师选择的课程
	// 依赖全局变量 semester_now（Global.cpp 中定义）
	// 生成形如：
	// course_uid NOT IN (SELECT course_uid FROM teacher_course WHERE semester='2024-1')

	// 从全局 std::string 转为 CString 并转义
	CString semCs = CString(semester_now.c_str());
	CString semEsc = EscapeSql(semCs);

	CString where;
	where.Format(_T("course_uid NOT IN (SELECT course_uid FROM teacher_course WHERE semester = '%s')"), semEsc);

	// 如果你的数据库不支持子查询，也可以改为先查询 teacher_course 再用 NOT IN 列表构造 WHERE
	if (!m_dbHelper.DisplayTableData(m_list, _T("course"), _T("*"), where))
	{
		AfxMessageBox(_T("显示未被选择的课程失败！"));
	}
	else
	{
		// 标记当前 ListCtrl 正在显示 course
		m_currentTable = _T("course");
	}
}


void teacherdlg::OnBnClickedclasses()
{
	// 显示当前教师已选择的课程（按学期筛选）
	if (m_teacherUid.IsEmpty())
	{
		AfxMessageBox(_T("教师 UID 未设置，无法显示已选课程。"));
		return;
	}

	// 从全局 std::string 转为 CString 并转义（与 OnBnClickedChoosecourse 保持一致）
	CString semCs = CString(semester_now.c_str());
	CString semEsc = EscapeSql(semCs);

	// 转义教师 UID
	CString uidEsc = EscapeSql(m_teacherUid);

	// 构造 WHERE：只显示当前学期该教师选择的课程
	// 形式：course_uid IN (SELECT course_uid FROM teacher_course WHERE teacher_uid = 'xxx' AND semester = '2024-1')
	CString where;
	where.Format(_T("course_uid IN (SELECT course_uid FROM teacher_course WHERE teacher_uid = '%s' AND semester = '%s')"),
		uidEsc, semEsc);

	// 显示 course 表中符合条件的记录（即该教师在当前学期所选课程的详细信息）
	if (!m_dbHelper.DisplayTableData(m_list, _T("course"), _T("*"), where))
	{
		AfxMessageBox(_T("显示该教师已选择的课程失败！"));
	}
	else
	{
		// 标记当前 ListCtrl 正在显示 course
		m_currentTable = _T("course");
	}
}
