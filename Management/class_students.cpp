// class_students.cpp: 实现文件
//

#include "pch.h"
#include "Management.h"
#include "afxdialogex.h"
#include "class_students.h"


// class_students 对话框

IMPLEMENT_DYNAMIC(class_students, CDialogEx)

class_students::class_students(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_class_student, pParent)
{

}

class_students::~class_students()
{
}

void class_students::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, students_list, m_studentsList);
}


BEGIN_MESSAGE_MAP(class_students, CDialogEx)
	ON_NOTIFY(LVN_ITEMCHANGED, students_list, &class_students::OnLvnItemchangedlist)
END_MESSAGE_MAP()


// class_students 消息处理程序

BOOL class_students::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置列表样式
	m_studentsList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);

	// 如果有数据库和 course_uid，则显示对应选课记录
	if (m_pDb != nullptr && !m_courseUid.IsEmpty())
	{
		CString where;
		// 安全地按字符串匹配 course_uid
		where.Format(_T("course_uid = '%s'"), m_courseUid);
		// 使用表名与字段名称与数据库一致（大小写对 MySQL 不敏感）
		if (!m_pDb->DisplayTableData(m_studentsList, _T("courseSelection"), _T("*"), where))
		{
			AfxMessageBox(_T("显示选课学生数据失败！"));
		}
	}

	return TRUE;
}

void class_students::OnLvnItemchangedlist(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}
