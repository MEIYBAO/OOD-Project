#pragma once
#include "afxdialogex.h"
#include "CDatabaseHelper.h"
#include "ManagementDlg.h"


// StudentDlg 对话框

class StudentDlg : public CDialogEx
{
	DECLARE_DYNAMIC(StudentDlg)

public:
	StudentDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~StudentDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_STUDENT_DIALOG };
#endif

protected:
	CListCtrl m_list;
	CDatabaseHelper m_dbHelper;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnClickedPersonShow();

private:
	CString m_currentTable = _T("无");
	CEdit editItem;
	int hitRow;
	int hitCol;
	bool m_allowUnselectInCourse;

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnEditKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnEditKillFocus();
	afx_msg void OnBnClickedChooseClass();
	afx_msg void OnBnClickedcourses();
	// 右键菜单处理
	afx_msg void OnListRClick(NMHDR* pNMHDR, LRESULT* pResult);
	static bool InsertCourseSelectionAt(CListCtrl& listCtrl, CDatabaseHelper& dbHelper, int nItem, const CString& studentUidParam, const CString& selectionDateParam);
	static bool DeleteCourseSelectionAt(CListCtrl& listCtrl, CDatabaseHelper& dbHelper, int nItem, const CString& studentUidParam, const CString& selectionDateParam);

	afx_msg void OnBnClickedStuChange();
	afx_msg void OnBnClickedSearchGrade();
};
