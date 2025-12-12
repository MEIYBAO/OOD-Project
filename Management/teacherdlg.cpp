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
	ON_BN_CLICKED(changeUser, &teacherdlg::OnBnClickedchangeuser)
END_MESSAGE_MAP()


// 辅助：对简单 SQL 字符 ' 做转义，避免语句中断
CString EscapeSql(const CString& s)
{
	CString out = s;
	out.Replace(_T("'"), _T("''"));
	return out;
}

// 控制：只有当通过“教学课程”按钮填充 course 视图时才允许在 course 视图显示“删除”菜单
static bool s_allowDeleteInCourse = false; // 初始不允许

// 辅助：获取第 colIndex 列的列名（若失败返回空）
static CString GetListCtrlColumnName(CListCtrl& listCtrl, int colIndex)
{
	CString strColName;
	CHeaderCtrl* pHeader = listCtrl.GetHeaderCtrl();
	if (pHeader)
	{
		HDITEM hdi = { 0 };
		TCHAR szText[256] = { 0 };
		hdi.mask = HDI_TEXT;
		hdi.pszText = szText;
		hdi.cchTextMax = _countof(szText);
		if (pHeader->GetItem(colIndex, &hdi))
		{
			strColName = szText;
		}
	}
	return strColName;
}

// 辅助（通用）：在指定行上按惯例删除记录（寻找主键列、询问确认、执行删除并从界面移除）
// 返回 true 表示删除成功并已从界面移除
//static bool DeleteRecordAt(CListCtrl& listCtrl, CDatabaseHelper& dbHelper, int nItem, const CString& tableName)
//{
//	if (nItem < 0 || !listCtrl.GetSafeHwnd() || !dbHelper.IsConnected())
//		return false;
//
//	// 尝试找到主键列，优先查找 "course_uid"
//	int nColumnCount = 0;
//	CHeaderCtrl* pHeader = listCtrl.GetHeaderCtrl();
//	if (pHeader) nColumnCount = pHeader->GetItemCount();
//
//	int pkCol = -1;
//	TCHAR szText[256] = { 0 };
//	for (int i = 0; i < nColumnCount; ++i)
//	{
//		HDITEM hdi = { 0 };
//		hdi.mask = HDI_TEXT;
//		hdi.pszText = szText;
//		hdi.cchTextMax = _countof(szText);
//		if (pHeader->GetItem(i, &hdi))
//		{
//			CString colName = szText;
//			if (colName.CompareNoCase(_T("course_uid")) == 0 || colName.CompareNoCase(_T("course uid")) == 0)
//			{
//				pkCol = i;
//				break;
//			}
//		}
//	}
//
//	// 回退到第0列
//	if (pkCol == -1) pkCol = 0;
//
//	CString keyValue = listCtrl.GetItemText(nItem, pkCol);
//	if (keyValue.IsEmpty())
//	{
//		AfxMessageBox(_T("无法获取要删除记录的主键值，操作取消。"));
//		return false;
//	}
//
//	CString keyName = GetListCtrlColumnName(listCtrl, pkCol);
//	if (keyName.IsEmpty())
//	{
//		AfxMessageBox(_T("无法获取主键列名，操作取消。"));
//		return false;
//	}
//
//	// 确认删除
//	if (AfxMessageBox(_T("确定要删除该记录吗？"), MB_YESNO | MB_ICONQUESTION) != IDYES)
//		return false;
//
//	// 构造 WHERE 并执行删除（使用 EscapeSql 保持安全）
//	CString where;
//	where.Format(_T("%s = '%s'"), keyName, EscapeSql(keyValue));
//
//	if (dbHelper.DeleteRecord(tableName, where))
//	{
//		listCtrl.DeleteItem(nItem);
//		AfxMessageBox(_T("删除成功！"));
//		return true;
//	}
//	else
//	{
//		AfxMessageBox(_T("删除失败，请检查数据库或日志。"));
//		return false;
//	}
//}

// 新增辅助：删除 teacher_course 表中对应记录（使用传入的 teacherUid 与 semester 优先，若为空再从列表读取）
// 返回 true 表示删除成功并从界面移除
static bool DeleteTeacherCourseAt(CListCtrl& listCtrl, CDatabaseHelper& dbHelper, int nItem, const CString& teacherUidParam, const CString& semesterParam)
{
	if (nItem < 0 || !listCtrl.GetSafeHwnd() || !dbHelper.IsConnected())
		return false;

	// 查找 course_uid 列索引（teacher_uid / semester 可能不在 course 列视图）
	int nColumnCount = 0;
	CHeaderCtrl* pHeader = listCtrl.GetHeaderCtrl();
	if (pHeader) nColumnCount = pHeader->GetItemCount();

	int colTeacher = -1, colCourse = -1, colSemester = -1;
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
			if (colName.CompareNoCase(_T("teacher_uid")) == 0 || colName.CompareNoCase(_T("teacher uid")) == 0)
			{
				colTeacher = i;
			}
			else if (colName.CompareNoCase(_T("course_uid")) == 0 || colName.CompareNoCase(_T("course uid")) == 0)
			{
				colCourse = i;
			}
			else if (colName.CompareNoCase(_T("semester")) == 0)
			{
				colSemester = i;
			}
		}
	}

	// 回退策略：若未找到 course 列，则尝试使用第0或第1列
	if (colCourse == -1 && nColumnCount > 0) colCourse = 0;
	if (colCourse == -1) return false;

	// 优先使用传入的 teacherUidParam / semesterParam（这些由调用方提供）
	CString teacherUid = teacherUidParam;
	if (teacherUid.IsEmpty() && colTeacher != -1)
		teacherUid = listCtrl.GetItemText(nItem, colTeacher);

	CString courseUid = listCtrl.GetItemText(nItem, colCourse);

	CString semesterVal = semesterParam;
	if (semesterVal.IsEmpty() && colSemester != -1)
		semesterVal = listCtrl.GetItemText(nItem, colSemester);

	if (teacherUid.IsEmpty() || courseUid.IsEmpty())
	{
		AfxMessageBox(_T("无法获取 teacher_uid 或 course_uid，操作取消。"));
		return false;
	}

	// 构造 WHERE 子句：至少 teacher_uid + course_uid
	CString where;
	where.Format(_T("teacher_uid = '%s' AND course_uid = '%s'"), EscapeSql(teacherUid), EscapeSql(courseUid));

	// 如果 semester 有值则加上筛选条件（避免误删跨学期记录）
	if (!semesterVal.IsEmpty())
	{
		where += _T(" AND semester = '") + EscapeSql(semesterVal) + _T("'");
	}

	// 确认删除
	if (AfxMessageBox(_T("确定要删除该讲授课吗？"), MB_YESNO | MB_ICONQUESTION) != IDYES)
		return false;

	// 执行删除
	if (dbHelper.DeleteRecord(_T("teacher_course"), where))
	{
		listCtrl.DeleteItem(nItem);
		AfxMessageBox(_T("删除课程成功！"));
		return true;
	}
	else
	{
		AfxMessageBox(_T("删除失败，请检查数据库或日志。"));
		return false;
	}
}

// 新增辅助：在 teacher_course 插入一条记录（teacher_uid, course_uid, semester）
// 返回 true 表示插入成功
static bool InsertTeacherCourseAt(CListCtrl& listCtrl, CDatabaseHelper& dbHelper, int nItem, const CString& teacherUidParam, const CString& semesterParam)
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

	// 回退策略：若未找到 course 列，则使用第0列
	if (colCourse == -1 && nColumnCount > 0) colCourse = 0;
	if (colCourse == -1) return false;

	CString courseUid = listCtrl.GetItemText(nItem, colCourse);
	CString teacherUid = teacherUidParam;
	if (teacherUid.IsEmpty())
	{
		AfxMessageBox(_T("当前教师 UID 未设置，无法选择课程。"));
		return false;
	}

	CString semesterVal = semesterParam;

	// 确认选择
	if (AfxMessageBox(_T("确定要选择该课程进行讲授吗？"), MB_YESNO | MB_ICONQUESTION) != IDYES)
		return false;

	// 构造插入字段与值
	CStringArray fields, values;
	fields.Add(_T("teacher_uid"));
	fields.Add(_T("course_uid"));
	fields.Add(_T("semester"));

	values.Add(teacherUid);
	values.Add(courseUid);
	values.Add(semesterVal);

	// 调用 InsertRecord（内部会做转义）
	if (dbHelper.InsertRecord(_T("teacher_course"), fields, values))
	{
		AfxMessageBox(_T("选课成功！"));
		return true;
	}
	else
	{
		AfxMessageBox(_T("选课失败，请检查数据库或日志。"));
		return false;
	}
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
	if (!m_dbHelper.DisplayTableData(m_list, _T("teach_class"), _T("class_id,course_uid,teacher_uid,semester"), where))
	{
		AfxMessageBox(_T("显示教学班数据失败！"));
	}
	else
	{
		// 标记当前 ListCtrl 正在显示 teach_class，从而允许右键菜单中的“显示学生列表”操作
		m_currentTable = _T("teach_class");
		// 点击“显示教学班”时不影响 s_allowDeleteInCourse（与课程视图无关）
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

	// 强制设置为报表视图，避免样式导致 NM_RCLICK/HitTest 行为异常
	m_list.ModifyStyle(0, LVS_REPORT);

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
	// 仅在有意义的表类型上弹出对应的右键菜单
	// 支持：teach_class -> "显示学生列表"
	//       course      -> "删除" 或 "选择"
	if (m_currentTable.IsEmpty())
	{
		*pResult = 0;
		return;
	}

	// 获取鼠标位置，转换到 list 客户区并 HitTest 确认是在某一项上右击
	CPoint ptScreen;
	GetCursorPos(&ptScreen);

	CPoint ptClient = ptScreen;
	m_list.ScreenToClient(&ptClient);
	int nItem = m_list.HitTest(ptClient);
	if (nItem == -1)
	{
		// 在空白区域不弹菜单
		*pResult = 0;
		return;
	}

	CMenu menu;
	menu.CreatePopupMenu();

	if (m_currentTable.CompareNoCase(_T("teach_class")) == 0)
	{
		menu.AppendMenu(MF_STRING, 1001, _T("显示学生列表"));
	}
	else if (m_currentTable.CompareNoCase(_T("course")) == 0)
	{
		// 在课程视图：由“教学课程”填充时显示“删除”，由“选择课程”填充时显示“选择”
		if (s_allowDeleteInCourse)
			menu.AppendMenu(MF_STRING, 2001, _T("删除"));
		else
			menu.AppendMenu(MF_STRING, 3001, _T("选择"));
	}
	else
	{
		// 其他表类型暂不显示菜单
		*pResult = 0;
		return;
	}

	int nCmd = menu.TrackPopupMenu(TPM_RETURNCMD | TPM_LEFTALIGN, ptScreen.x, ptScreen.y, this);

	switch (nCmd)
	{
	case 1001:
		OnShowClassStudents();
		break;
	case 2001:
	{
		// 使用专门的函数删除 teacher_course 表中的记录
		CString semCs = CString(semester_now.c_str());
		bool ok = DeleteTeacherCourseAt(m_list, m_dbHelper, nItem, m_teacherUid, semCs);
		if (ok)
		{
			// 删除成功后通过“教学课程”按钮刷新视图
			OnBnClickedclasses();
		}
	}
	break;
	case 3001:
	{
		// 在“选择课程”视图下，插入 teacher_course 记录
		CString semCs = CString(semester_now.c_str());
		bool ok = InsertTeacherCourseAt(m_list, m_dbHelper, nItem, m_teacherUid, semCs);
		if (ok)
		{
			// 插入成功后刷新“选择课程”视图，移除已被选择的课程
			OnBnClickedChoosecourse();
		}
	}
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

	// 查找列名为 course_uid 与 semester 的列索引（teach_class 表通常包含这些列）
	int nColumnCount = 0;
	CHeaderCtrl* pHeader = m_list.GetHeaderCtrl();
	if (pHeader)
		nColumnCount = pHeader->GetItemCount();

	int colCourse = -1;
	int colSemester = -1;
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
			}
			else if (colName.CompareNoCase(_T("semester")) == 0)
			{
				colSemester = i;
			}
		}
	}

	// 回退策略：若未找到 course_uid 列，则尝试使用常见列索引（例如索引1）
	if (colCourse == -1)
	{
		if (nColumnCount > 1) colCourse = 1;
		else colCourse = 0;
	}

	CString courseUid = m_list.GetItemText(nItem, colCourse);
	if (courseUid.IsEmpty())
	{
		AfxMessageBox(_T("无法获取所选记录的 course_uid！"));
		return;
	}

	// 读取 teach_class 行的 semester 值（若未找到列则为空，child 对话框会回退到全局）
	CString semesterVal;
	if (colSemester != -1)
		semesterVal = m_list.GetItemText(nItem, colSemester);
	else
		semesterVal = _T(""); // 子对话框会回退到全局 semester_now

	// 打开 class_students 对话框并传入数据库指针、course_uid 与 semester（优先使用 teach_class 的 semester）
	class_students dlg(this);
	dlg.SetDatabaseHelper(&m_dbHelper);
	dlg.SetCourseUid(courseUid);
	dlg.SetSemester(semesterVal);
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
		// 由“选择课程”填充时，不允许显示删除菜单
		s_allowDeleteInCourse = false;
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
		// 由“教学课程”填充时，允许显示删除菜单
		s_allowDeleteInCourse = true;
	}
}


void teacherdlg::OnBnClickedchangeuser()
{
	// TODO: 在此添加控件通知处理程序代码
	CManagementDlg dlg;
	EndDialog(IDOK);
	dlg.DoModal();
}

