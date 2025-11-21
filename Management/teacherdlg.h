#pragma once
#include "afxdialogex.h"
#include "CDatabaseHelper.h"

// teacherdlg 对话框

class teacherdlg : public CDialogEx
{
	DECLARE_DYNAMIC(teacherdlg)

public:
	teacherdlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~teacherdlg();

	// 设置/获取当前登录教师的 UID（由创建/调用此对话框的上层代码传入）
	void SetTeacherUid(const CString& uid) { m_teacherUid = uid; }
	CString GetTeacherUid() const { return m_teacherUid; }

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_teacher_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	CDatabaseHelper m_dbHelper;
	CListCtrl m_list;

	// 保存当前登录教师的 UID，用于筛选 teach_class 表
	CString m_teacherUid;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedshowclass();

	virtual BOOL OnInitDialog();
	afx_msg void OnLvnItemchangedworkpanel(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void OnListRClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnShowClassStudents();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedChoosecourse();
};
